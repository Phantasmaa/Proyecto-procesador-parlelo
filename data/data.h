// SPDX-License-Identifier: MIT
// Demo / seed data for the message pipeline. Kept in a header so the
// data symbols are visible across translation units without a separate .cpp.
#pragma once

#include <string>

#include "inc.h"

namespace pp::data {

inline const pp::model::Company kCompanies[] = {
    {"google",    "1600 Amphitheatre Parkway, Mountain View, CA",
     "+1-650-253-0000", "press@google.com",       "google.com"},
    {"facebook",  "1 Hacker Way, Menlo Park, CA",
     "+1-650-543-4800", "press@fb.com",           "facebook.com"},
    {"microsoft", "One Microsoft Way, Redmond, WA",
     "+1-425-882-8080", "press@microsoft.com",    "microsoft.com"},
    {"apple",     "One Apple Park Way, Cupertino, CA",
     "+1-408-996-1010", "press@apple.com",        "apple.com"},
};

inline const std::string kFirstNames[] = {
    "pepe", "juan", "luis", "paco", "jose",
    "lucas", "maria", "lucia", "luisa", "paca"
};
inline const std::string kSurnames[] = {
    "garcia", "fernandez", "lopez", "martinez", "sanchez",
    "perez",  "gomez",     "martin", "jimenez",  "ruiz"
};
inline const std::string kAges[] = {
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29"
};
inline const std::string kDnis[] = {
    "73214521", "12214521", "73214521",
    "73298721", "74564521", "73211231"
};
inline const std::string kAddresses[] = {
    "calle1", "calle2", "calle3", "calle4",  "calle5",
    "calle6", "calle7", "calle8", "calle9",  "calle10"
};
inline const std::string kPhones[] = {
    "97214521", "922167821", "93214521",
    "93298788", "94564599",  "93211255"
};
inline const std::string kEmails[] = {
    "chichico@gmail.com", "lolo12@outlook.com",
    "example1@gmail.com", "example2@gmail.com",
    "asfbasoffice@gmail.com"
};
inline const std::string kJobs[] = {
    "developer", "tester", "manager", "director", "ceo",
    "cto",       "cfo",    "coo",     "cso",      "cdo"
};
inline const std::string kCompanyNames[] = {
    "google", "facebook", "microsoft", "apple"
};

}  // namespace pp::data
