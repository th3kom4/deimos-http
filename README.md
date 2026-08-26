# deimos-http 

A high-performance, multi-threaded HTTP/1.1-subset web server built from scratch in C++.

It provides a lightweight backend platform designed for handling concurrent client connections, featuring a non-blocking event-driven reactor architecture and a dedicated worker thread pool.

## Features

- **Event-Driven Concurrency:** Uses an asynchronous reactor pattern for efficient, non-blocking socket multiplexing under heavy load.
- **Dedicated Worker Pool:** Dynamically scales worker threads to match available CPU cores minus one (`cores - 1`), keeping the event loop unblocked on its own core.
- **Thread-Safe Handoff:** Seamlessly transfers client connections from the network polling loop to available background workers.
- **Modular Request Pipeline:** Fast, request parsing paired with clean request handling and routing.

[Project Architecture](docs/ARCHITECTURE.md)

## Build

Requires a Linux environment with a C++23 compiler and CMake.

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Run

```bash
STATIC_DIR=../public ./server <port> [threads]
```

### Examples

```bash
# Run on port 8080 using the default core-scaled worker pool
STATIC_DIR=../public ./server 8080

# Run on port 8080 with a fixed pool of 4 worker threads
STATIC_DIR=../public ./server 8080 4
```
