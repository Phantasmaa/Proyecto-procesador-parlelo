#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <string.h>
#include <string>

std::string base64_encode(const std::string &in) {

    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, in.c_str(), in.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    std::string out(bufferPtr->data, bufferPtr->length);
    BUF_MEM_free(bufferPtr);
    return out;
}

std::string base64_decode(const std::string &in) {

    BIO *bio, *b64;
    char *buffer = (char *)malloc(in.length());
    memset(buffer, 0, in.length());

    bio = BIO_new_mem_buf(in.c_str(), in.length());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_read(bio, buffer, in.length());
    BIO_free_all(bio);

    std::string out(buffer, in.length());
    free(buffer);
    return out;
}