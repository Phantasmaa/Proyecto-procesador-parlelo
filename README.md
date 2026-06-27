# Procesador Paralelo v2

Producer/consumer pipeline that demonstrates thread-level parallelism
over a System V IPC message queue.

```
+--------------------+        System V msg queue         +------------------+
|   sender (5 thr)   |  --base64--encode(msg)--> [ ... ]  | receiver (N thr) |
|   randomPerson     |                                  |   writeInfo()    |
+--------------------+                                  +------------------+
                                                                |
                                                                v
                                                  /app/log/dataLog/<company>.data
```

## What changed from v1

| Area | v1 | v2 |
| ---- | -- | -- |
| Build | hand-rolled `g++` one-liner | CMake (`cmake -S . -B build && cmake --build build`) |
| Memory in `coder.h` | leaked `BUF_MEM`, raw `BIO*` | `std::unique_ptr` RAII |
| Globals in `func.h` | `std::rand`, magic numbers | `std::mt19937`, named constants |
| Threading | detached `std::thread` with shared `cambio` int | `WorkQueue` with `std::condition_variable` + `std::atomic` |
| Sources | duplicate `reciver.cpp` (typo) and `receiver.cpp` | one `receiver.cpp`, typo file removed |
| Names | `adress`, `cambio`, `lanzarMsg` | `address`, `processed_`, `runBurst` |
| IPC | raw `msgsnd/msgrcv` per file | `pp::ipc::Queue` wrapper, single key, RAII |
| Logging | `std::asctime` (deprecated) | `std::strftime` with thread-safe `localtime_r` |
| Image | `g++` + libs in final image (300 MB) | multi-stage, runtime-only (≈ 90 MB) |
| User | root | non-root `pp` user |
| Tooling | none | `.dockerignore`, `.gitignore`, `CMakeLists.txt` |

## Layout

```
lib/        public domain types + helpers (header + impl split)
data/       seed data (names, companies, …) in one place
src/        sender.cpp, receiver.cpp — both depend only on lib/
Dockerfile  multi-stage, non-root
CMakeLists.txt
```

## Build (native)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bin/sender --help
./build/bin/receiver --help
```

## Build (Docker)

```bash
docker build -t proyecto-paralelo:latest .
```

## Run

```bash
# Terminal 1 — produce 100 000 messages with 5 producer threads
docker run --rm -it --name sender proyecto-paralelo:latest \
    ./bin/sender --count 100000

# Terminal 2 — consume with all available CPU threads
docker run --rm -it --name receiver --volumes-from sender \
    -v $(pwd)/out:/app/log proyecto-paralelo:latest \
    ./bin/receiver --listen --threads 4
```

`--bench` mode processes exactly N messages and reports a throughput number:

```bash
./bin/sender   --count 50000   &  # background
./bin/receiver --bench --count 50000 --threads 4
```

## Public surface

`lib/inc.h` exposes `pp::model::{Person,Company,Message}`.
`lib/func.h` exposes `pp::msg::{createRandomMessage,encode,decode,writeInfo,writeLog}`.
`lib/ipc.h` exposes `pp::ipc::Queue` (RAII wrapper over `msgsnd/msgrcv`).
`lib/coder.h` exposes `pp::codec::{base64_encode,base64_decode}`.
