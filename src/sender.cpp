// SPDX-License-Identifier: MIT
// Producer: generates random messages and pushes them into the IPC queue
// either as a fixed batch (--count N) or continuously (--infinite).
// Multiple producer threads can run concurrently to demonstrate fan-out.
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "func.h"
#include "ipc.h"
#include "coder.h"
#include "inc.h"

namespace {

constexpr int kBatchSize = 20000;

void runProducer(pp::ipc::Queue& queue, std::atomic<bool>& keepRunning) {
    while (keepRunning.load()) {
        pp::model::Message m = pp::msg::createRandomMessage();
        const std::string wire = pp::msg::encode(m);
        try {
            queue.send(pp::codec::base64_encode(wire));
        } catch (const std::exception& e) {
            std::cerr << "[sender] " << e.what() << '\n';
        }
    }
}

void runBurst(pp::ipc::Queue& queue, int count) {
    for (int i = 0; i < count; ++i) {
        pp::model::Message m = pp::msg::createRandomMessage();
        const std::string wire = pp::msg::encode(m);
        queue.send(pp::codec::base64_encode(wire));
    }
}

void printUsage(const char* argv0) {
    std::cout <<
        "Usage: " << argv0 << " [options]\n"
        "  --count N        send N messages from 5 producer threads (default: " << kBatchSize << ")\n"
        "  --infinite       send messages forever (Ctrl-C to stop)\n"
        "  --demo           show one decoded message and exit\n"
        "  --help           show this help\n";
}

}  // namespace

int main(int argc, char** argv) {
    bool infinite = false;
    bool demo     = false;
    int  count    = kBatchSize;

    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--count" && i + 1 < argc)      count = std::atoi(argv[++i]);
        else if (a == "--infinite")              infinite = true;
        else if (a == "--demo")                  demo = true;
        else if (a == "--help" || a == "-h")     { printUsage(argv[0]); return 0; }
        else {
            std::cerr << "Unknown option: " << a << "\n";
            printUsage(argv[0]);
            return 2;
        }
    }

    if (demo) {
        const auto m = pp::msg::createRandomMessage();
        pp::msg::printMessage(m);
        return 0;
    }

    pp::ipc::Queue queue;
    try {
        queue.open(/*create=*/true);
    } catch (const std::exception& e) {
        std::cerr << "[sender] " << e.what() << '\n';
        return 1;
    }

    pp::msg::writeLog("Sender started");
    std::cout << "Sender online. IPC queue id=" << queue.id() << '\n';

    if (infinite) {
        std::atomic<bool> keepRunning{true};
        std::vector<std::thread> producers;
        for (int t = 0; t < 5; ++t) {
            producers.emplace_back(runProducer, std::ref(queue), std::ref(keepRunning));
        }
        std::cout << "Streaming 5 producers. Press Enter to stop...\n";
        std::cin.get();
        keepRunning.store(false);
        for (auto& th : producers) th.join();
    } else {
        std::cout << "Bursting " << count << " messages from 5 producer threads...\n";
        std::vector<std::thread> producers;
        for (int t = 0; t < 5; ++t) {
            producers.emplace_back(runBurst, std::ref(queue), count);
        }
        for (auto& th : producers) th.join();
    }

    pp::msg::writeLog("Sender stopped");
    return 0;
}
