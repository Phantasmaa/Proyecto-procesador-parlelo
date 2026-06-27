// SPDX-License-Identifier: MIT
// Base64 codec implementation using OpenSSL.
#include "coder.h"

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include <cstring>
#include <memory>
#include <vector>

namespace pp::codec {

namespace {

// RAII wrapper for OpenSSL BIO to prevent leaks.
struct BioDeleter {
    void operator()(BIO* b) const noexcept {
        if (b) BIO_free_all(b);
    }
};
using BioPtr = std::unique_ptr<BIO, BioDeleter>;

// RAII wrapper for BUF_MEM.
struct BufMemDeleter {
    void operator()(BUF_MEM* m) const noexcept {
        if (m) BUF_MEM_free(m);
    }
};
using BufMemPtr = std::unique_ptr<BUF_MEM, BufMemDeleter>;

}  // namespace

std::string base64_encode(const std::string& in) {
    BioPtr chain(BIO_new(BIO_s_mem()));
    BioPtr b64(BIO_new(BIO_f_base64()));
    if (!chain || !b64) return {};

    // Push the base64 filter on top of the mem sink.
    BIO* bio = BIO_push(b64.get(), chain.release());
    b64.release();  // ownership transferred to bio

    if (BIO_write(bio, in.data(), static_cast<int>(in.size())) <= 0) {
        BIO_free_all(bio);
        return {};
    }
    BIO_flush(bio);

    BUF_MEM* raw = nullptr;
    BIO_get_mem_ptr(bio, &raw);
    BufMemPtr mem(raw);
    BIO_set_close(bio, BIO_NOCLOSE);

    std::string out;
    if (mem) {
        out.assign(mem->data, mem->length);
    }
    BIO_free_all(bio);
    return out;
}

std::string base64_decode(const std::string& in) {
    if (in.empty()) return {};

    BioPtr mem(BIO_new_mem_buf(in.data(), static_cast<int>(in.size())));
    BioPtr b64(BIO_new(BIO_f_base64()));
    if (!mem || !b64) return {};

    BIO* bio = BIO_push(b64.get(), mem.release());
    b64.release();

    std::vector<char> buffer(in.size(), '\0');
    const int decoded = BIO_read(bio, buffer.data(), static_cast<int>(buffer.size()));
    BIO_free_all(bio);

    if (decoded <= 0) return {};
    return std::string(buffer.data(), static_cast<std::size_t>(decoded));
}

}  // namespace pp::codec
