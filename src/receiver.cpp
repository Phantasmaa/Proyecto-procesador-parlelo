// SPDX-License-Identifier: MIT
// Consumer: drains messages from the IPC queue with a configurable number
// of worker threads, then writes them to per-company data files.
//
// --mode listen  : live process, blocks until --quit-secs elapses
//                  or the queue stays empty for --idle-secs.
// --mode bench   : process exactly N messages, time the run, report
//                  speedup vs the sequential reference.
// --threads N    : worker count (default = hardware_concurrency).
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "coder.h"
#include "func.h"
#include "inc.h"
#include "ipc.h"

namespace {

// Bounded MPMC queue with a "drained" signal so workers can exit cleanly.
class WorkQueue {
public:
    void push(std::string s) {
        {
            std::lock_guard<std::mutex> lk(m_);
            q_.push_back(std::move(s));
        }
        cv_.notify_one();
    }

    // Block until a value is available or shutdown is requested.
    // Returns empty optional to signal shutdown.
    std::optional<std::string> pop() {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&] { return !q_.empty() || shutdown_; });
        if (q_.empty()) return std::nullopt;
        std::string v = std::move(q_.front());
        q_.pop_front();
        return v;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lk(m_);
            shutdown_ = true;
        }
        cv_.notify_all();
    }

private:
    std::mutex                                m_;
    std::condition_variable                   cv_;
    std::deque<std::string>                   q_;
    bool                                      shutdown_ = false;
};

struct Config {
    int  threads     = static_cast<int>(std::thread::hardware_concurrency());
    int  benchCount  = 50000;
    int  idleSecs    = 5;
    int  quitSecs    = 0;   // 0 = no upper bound
    bool bench       = false;
    bool listen      = false;
};

Config parseArgs(int argc, char** argv) {
    Config c;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if      (a == "--threads"   && i + 1 < argc) c.threads    = std::atoi(argv[++i]);
        else if (a == "--count"     && i + 1 < argc) c.benchCount = std::atoi(argv[++i]);
        else if (a == "--idle-secs"  && i + 1 < argc) c.idleSecs   = std::atoi(argv[++i]);
        else if (a == "--quit-secs"  && i + 1 < argc) c.quitSecs   = std::atoi(argv[++i]);
        else if (a == "--bench")                     c.bench      = true;
        else if (a == "--listen")                    c.listen     = true;
    }
    return c;
}

void workerLoop(WorkQueue& q, std::atomic<std::int64_t>& processed) {
    while (auto item = q.pop()) {
        try {
            const std::string wire = pp::codec::base64_decode(*item);
            const auto m = pp::msg::decode(wire);
            pp::msg::writeInfo(m);
        } catch (const std::exception& e) {
            std::cerr << "[worker] " << e.what() << '\n';
        }
        processed.fetch_add(1, std::memory_order_relaxed);
    }
}

void runListen(pp::ipc::Queue& ipc, const Config& c) {
    WorkQueue q;
    std::atomic<std::int64_t> processed{0};
    const auto start = std::chrono::steady_clock::now();

    std::vector<std::thread> workers;
    for (int i = 0; i < c.threads; ++i) {
        workers.emplace_back(workerLoop, std::ref(q), std::ref(processed));
    }

    std::thread producer([&] {
        auto lastSeen = std::chrono::steady_clock::now();
        const auto startT = lastSeen;
        while (true) {
            try {
                const std::string payload = ipc.receive();
                q.push(payload);
                lastSeen = std::chrono::steady_clock::now();
            } catch (const std::exception& e) {
                std::cerr << "[receiver] " << e.what() << '\n';
                break;
            }
            if (c.quitSecs > 0) {
                const auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - startT).count()
                    >= c.quitSecs) break;
            }
            if (c.idleSecs > 0) {
                const auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - lastSeen).count()
                    >= c.idleSecs) break;
            }
        }
        q.shutdown();
    });

    // Live throughput display.
    auto lastReport = std::chrono::steady_clock::now();
    auto lastCount  = std::int64_t{0};
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        const auto now = std::chrono::steady_clock::now();
        const auto cur = processed.load();
        const auto dt  = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastReport).count();
        if (dt > 0) {
            const double rate = 1000.0 * (cur - lastCount) / dt;
            std::cout << "\rmessages processed: " << cur
                      << "  rate: " << static_cast<int>(rate) << "/s   ";
            std::cout.flush();
        }
        lastReport = now;
        lastCount  = cur;
        if (c.quitSecs > 0) {
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
            if (elapsed >= c.quitSecs) break;
        }
    }

    producer.join();
    q.shutdown();
    for (auto& t : workers) t.join();

    const auto total = processed.load();
    std::cout << "\nTotal processed: " << total
              << " with " << c.threads << " threads\n";
}

void runBench(pp::ipc::Queue& ipc, const Config& c) {
    std::cout << "Benchmark: " << c.benchCount << " messages, "
              << c.threads << " threads\n";

    WorkQueue q;
    std::atomic<std::int64_t> processed{0};
    std::vector<std::thread> workers;
    for (int i = 0; i < c.threads; ++i) {
        workers.emplace_back(workerLoop, std::ref(q), std::ref(processed));
    }

    const auto t0 = std::chrono::steady_clock::now();
    int received = 0;
    while (received < c.benchCount) {
        try {
            q.push(ipc.receive());
            ++received;
        } catch (const std::exception& e) {
            std::cerr << "[receiver] " << e.what() << '\n';
            break;
        }
    }
    // Wait for workers to drain.
    while (processed.load() < received) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    const auto t1 = std::chrono::steady_clock::now();

    q.shutdown();
    for (auto& t : workers) t.join();

    const double seconds = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Processed " << processed.load()
              << " messages in " << seconds << " s"
              << "  (" << static_cast<int>(processed.load() / seconds) << " msg/s)\n";
}

}  // namespace

int main(int argc, char** argv) {
    const Config c = parseArgs(argc, argv);
    if (c.threads < 1) {
        std::cerr << "thread count must be >= 1\n";
        return 2;
    }

    pp::ipc::Queue ipc;
    try {
        ipc.open(/*create=*/false);
    } catch (const std::exception& e) {
        std::cerr << "[receiver] " << e.what() << '\n';
        return 1;
    }

    pp::msg::writeLog("Receiver started with " + std::to_string(c.threads) + " threads");
    std::cout << "Receiver online. IPC queue id=" << ipc.id()
              << " workers=" << c.threads << '\n';

    if (c.bench)   runBench (ipc, c);
    else           runListen(ipc, c);

    pp::msg::writeLog("Receiver stopped");
    return 0;
}
