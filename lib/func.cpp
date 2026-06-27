// SPDX-License-Identifier: MIT
#include "func.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>

#include "data.h"

namespace pp::msg {

namespace {

constexpr const char* kLogPath    = "/app/log/info.log";
constexpr const char* kDataDir    = "/app/log/dataLog/";

// Inclusive random integer in [min, max].
int randomInRange(int min, int max) {
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

std::string pick(const std::string* arr, std::size_t n) {
    return arr[randomInRange(0, static_cast<int>(n) - 1)];
}

std::string padNumber(int value, int width) {
    std::ostringstream oss;
    oss << std::setw(width) << std::setfill('0') << value;
    return oss.str();
}

pp::model::Company companyFor(const std::string& name) {
    for (const auto& c : pp::data::kCompanies) {
        if (c.name == name) return c;
    }
    throw std::runtime_error("unknown company: " + name);
}

}  // namespace

pp::model::Message createRandomMessage() {
    pp::model::Message m;
    m.person.name    = pick(pp::data::kFirstNames, std::size(pp::data::kFirstNames));
    m.person.name   += "|" + pick(pp::data::kSurnames, std::size(pp::data::kSurnames));
    m.person.age     = pick(pp::data::kAges,       std::size(pp::data::kAges));
    m.person.dni     = pick(pp::data::kDnis,       std::size(pp::data::kDnis));
    m.person.address = pick(pp::data::kAddresses,  std::size(pp::data::kAddresses));
    m.person.phone   = pick(pp::data::kPhones,     std::size(pp::data::kPhones));
    m.person.email   = pick(pp::data::kEmails,     std::size(pp::data::kEmails));
    m.person.job     = pick(pp::data::kJobs,       std::size(pp::data::kJobs));
    m.person.company = pick(pp::data::kCompanyNames, std::size(pp::data::kCompanyNames));
    m.company        = companyFor(m.person.company);
    return m;
}

std::string encode(const pp::model::Message& m) {
    const int nameSize     = static_cast<int>(m.person.name.size());
    const int ageSize      = static_cast<int>(m.person.age.size());
    const int dniSize      = static_cast<int>(m.person.dni.size());
    const int addressSize  = static_cast<int>(m.person.address.size());
    const int phoneSize    = static_cast<int>(m.person.phone.size());
    const int emailSize    = static_cast<int>(m.person.email.size());
    const int jobSize      = static_cast<int>(m.person.job.size());
    const int companySize  = static_cast<int>(m.person.company.size());
    const int cAddressSize = static_cast<int>(m.company.address.size());
    const int cPhoneSize   = static_cast<int>(m.company.phone.size());
    const int cEmailSize   = static_cast<int>(m.company.email.size());
    const int cWebSize     = static_cast<int>(m.company.web.size());

    const int personSize = nameSize + ageSize + dniSize + addressSize
                         + phoneSize + emailSize + jobSize + companySize;
    const int totalSize  = personSize + cAddressSize + cPhoneSize
                         + cEmailSize   + cWebSize;

    std::string out;
    out.reserve(33 + personSize + cAddressSize + cPhoneSize
                + cEmailSize + cWebSize);
    out += padNumber(totalSize,     3);
    out += padNumber(personSize,    3);
    out += padNumber(cAddressSize + cPhoneSize + cEmailSize + cWebSize, 3);
    out += padNumber(nameSize,      2);
    out += padNumber(ageSize,       2);
    out += padNumber(dniSize,       2);
    out += padNumber(addressSize,   2);
    out += padNumber(phoneSize,     2);
    out += padNumber(emailSize,     2);
    out += padNumber(jobSize,       2);
    out += padNumber(companySize,   2);
    out += padNumber(cAddressSize,  2);
    out += padNumber(cPhoneSize,    2);
    out += padNumber(cEmailSize,    2);
    out += padNumber(cWebSize,      2);
    out += m.person.name;
    out += m.person.age;
    out += m.person.dni;
    out += m.person.address;
    out += m.person.phone;
    out += m.person.email;
    out += m.person.job;
    out += m.person.company;
    out += m.company.address;
    out += m.company.phone;
    out += m.company.email;
    out += m.company.web;
    return out;
}

pp::model::Message decode(const std::string& wire) {
    if (wire.size() < 33) {
        throw std::runtime_error("wire format too short");
    }
    const int nameSize     = std::stoi(wire.substr( 9, 2));
    const int ageSize      = std::stoi(wire.substr(11, 2));
    const int dniSize      = std::stoi(wire.substr(13, 2));
    const int addressSize  = std::stoi(wire.substr(15, 2));
    const int phoneSize    = std::stoi(wire.substr(17, 2));
    const int emailSize    = std::stoi(wire.substr(19, 2));
    const int jobSize      = std::stoi(wire.substr(21, 2));
    const int companySize  = std::stoi(wire.substr(23, 2));
    const int cAddressSize = std::stoi(wire.substr(25, 2));
    const int cPhoneSize   = std::stoi(wire.substr(27, 2));
    const int cEmailSize   = std::stoi(wire.substr(29, 2));
    const int cWebSize     = std::stoi(wire.substr(31, 2));

    std::size_t cursor = 33;
    pp::model::Message m;
    m.person.name    = wire.substr(cursor, nameSize);                          cursor += nameSize;
    m.person.age     = wire.substr(cursor, ageSize);                           cursor += ageSize;
    m.person.dni     = wire.substr(cursor, dniSize);                           cursor += dniSize;
    m.person.address = wire.substr(cursor, addressSize);                       cursor += addressSize;
    m.person.phone   = wire.substr(cursor, phoneSize);                         cursor += phoneSize;
    m.person.email   = wire.substr(cursor, emailSize);                         cursor += emailSize;
    m.person.job     = wire.substr(cursor, jobSize);                           cursor += jobSize;
    m.person.company = wire.substr(cursor, companySize);                       cursor += companySize;
    m.company.name   = m.person.company;
    m.company.address = wire.substr(cursor, cAddressSize);                     cursor += cAddressSize;
    m.company.phone   = wire.substr(cursor, cPhoneSize);                       cursor += cPhoneSize;
    m.company.email   = wire.substr(cursor, cEmailSize);                       cursor += cEmailSize;
    m.company.web     = wire.substr(cursor, cWebSize);
    return m;
}

void writeInfo(const pp::model::Message& m) {
    const std::string path = std::string(kDataDir) + m.person.company + ".data";
    std::ofstream out(path, std::ios::app);
    if (!out) {
        std::cerr << "Failed to open " << path << " for writing.\n";
        return;
    }
    out << "\nNombre: " << m.person.name
        << "\nEdad: "   << m.person.age
        << "\nDNI: "    << m.person.dni
        << "\nDireccion: " << m.person.address
        << "\nTelefono: "  << m.person.phone
        << "\nEmail: "     << m.person.email
        << "\nTrabajo: "   << m.person.job
        << "\nCompania: "  << m.person.company
        << "\nDireccion compania: " << m.company.address
        << "\nTelefono compania: "  << m.company.phone
        << "\nEmail compania: "     << m.company.email
        << "\nWeb compania: "       << m.company.web
        << "\n-----------------------------------\n";
}

void writeLog(const std::string& message) {
    const std::time_t now = std::time(nullptr);
    std::tm tm_buf{};
    localtime_r(&now, &tm_buf);
    char ts[32];
    std::strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm_buf);

    std::ofstream out(kLogPath, std::ios::app);
    if (!out) {
        std::cerr << "Failed to open " << kLogPath << " for writing.\n";
        return;
    }
    out << ts << " - " << message << '\n';
}

void printMessage(const pp::model::Message& m) {
    std::cout << "\n\n\tMessage:\n"
              << "\tHeader: "         << m.header
              << "\n\tName: "          << m.person.name
              << "\n\tAge: "           << m.person.age
              << "\n\tDNI: "           << m.person.dni
              << "\n\tAddress: "       << m.person.address
              << "\n\tPhone: "         << m.person.phone
              << "\n\tEmail: "         << m.person.email
              << "\n\tJob: "           << m.person.job
              << "\n\tCompany: "       << m.person.company
              << "\n\tCompany name: "  << m.company.name
              << "\n\tCompany addr: "  << m.company.address
              << "\n\tCompany phone: " << m.company.phone
              << "\n\tCompany email: " << m.company.email
              << "\n\tCompany web: "   << m.company.web
              << "\n\n\tPress enter to continue\n";
    std::cin.ignore();
    std::cin.get();
}

}  // namespace pp::msg
