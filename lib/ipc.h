// SPDX-License-Identifier: MIT
// Thin wrapper around System V IPC message queues shared by sender and receiver.
#pragma once

#include <cstddef>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>

namespace pp::ipc {

// Project-wide message-queue key. Any process that wants to talk to the
// pipeline must agree on the same value.
constexpr key_t kQueueKey = 0x1A2B3C4D;

// Wire-format envelope that travels through the System V message queue.
struct Envelope {
    long  type;       // must be > 0
    char  text[1024]; // base64-encoded payload
};
constexpr std::size_t kPayloadSize = sizeof(Envelope::text);

class Queue {
public:
    Queue() = default;
    ~Queue();

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    // Create-or-attach the queue. `create` controls whether the call also
    // creates the queue if it does not exist yet.
    void open(bool create);

    // Send a payload to the queue. Blocks when the queue is full.
    void send(const std::string& payload);

    // Receive a payload. Blocks until a message arrives.
    std::string receive();

    // Remove the queue from the system. Useful for cleanup in tests.
    void unlink();

    int id() const { return id_; }

private:
    int id_ = -1;
};

}  // namespace pp::ipc
