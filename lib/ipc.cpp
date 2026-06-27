// SPDX-License-Identifier: MIT
#include "ipc.h"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

namespace pp::ipc {

Queue::~Queue() = default;

void Queue::open(bool create) {
    const int flags = 0666 | (create ? IPC_CREAT : 0);
    id_ = msgget(kQueueKey, flags);
    if (id_ == -1) {
        throw std::runtime_error(std::string("msgget failed: ") + std::strerror(errno));
    }
}

void Queue::send(const std::string& payload) {
    if (id_ == -1) {
        throw std::logic_error("Queue::send called before open()");
    }
    Envelope env{};
    env.type = 1;
    const std::size_t n = std::min(payload.size(), kPayloadSize - 1);
    std::memcpy(env.text, payload.data(), n);
    env.text[n] = '\0';

    if (msgsnd(id_, &env, sizeof(env.text), 0) == -1) {
        throw std::runtime_error(std::string("msgsnd failed: ") + std::strerror(errno));
    }
}

std::string Queue::receive() {
    if (id_ == -1) {
        throw std::logic_error("Queue::receive called before open()");
    }
    Envelope env{};
    if (msgrcv(id_, &env, sizeof(env.text), 1, 0) == -1) {
        throw std::runtime_error(std::string("msgrcv failed: ") + std::strerror(errno));
    }
    return std::string(env.text);
}

void Queue::unlink() {
    if (id_ != -1) {
        msgctl(id_, IPC_RMID, nullptr);
        id_ = -1;
    }
}

}  // namespace pp::ipc
