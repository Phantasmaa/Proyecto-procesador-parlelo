// SPDX-License-Identifier: MIT
// Base64 encode/decode using OpenSSL EVP API.
// Original implementation leaked BUF_MEM and used the deprecated low-level BIO API.
#pragma once

#include <string>

namespace pp::codec {

std::string base64_encode(const std::string& in);
std::string base64_decode(const std::string& in);

}  // namespace pp::codec
