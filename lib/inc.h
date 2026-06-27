// SPDX-License-Identifier: MIT
// Domain types for the message-processing pipeline.
#pragma once

#include <cstdint>
#include <string>

namespace pp::model {

struct Person {
    std::string name;
    std::string age;
    std::string dni;
    std::string address;   // "adress" was a typo — English word is "address"
    std::string phone;
    std::string email;
    std::string job;
    std::string company;
};

struct Company {
    std::string name;
    std::string address;
    std::string phone;
    std::string email;
    std::string web;
};

struct Message {
    std::string  header;
    Person       person;
    Company      company;
};

}  // namespace pp::model
