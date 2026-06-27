// SPDX-License-Identifier: MIT
// Message construction, encoding, and persistence helpers.
#pragma once

#include <string>

#include "inc.h"

namespace pp::msg {

// Build a fresh message populated with random data drawn from pp::data.
pp::model::Message createRandomMessage();

// Encode the message as a compact, fixed-width string ready for the wire.
// Format: 16 header fields (3+3+3+2*13) followed by the body.
std::string encode(const pp::model::Message& msg);

// Decode a wire-format string back into a message.
pp::model::Message decode(const std::string& wire);

// Persist a message to the per-company data file. The log file is updated
// with a timestamped line for observability.
void writeInfo(const pp::model::Message& msg);
void writeLog(const std::string& message);

// Human-readable debug print of the message fields.
void printMessage(const pp::model::Message& msg);

}  // namespace pp::msg
