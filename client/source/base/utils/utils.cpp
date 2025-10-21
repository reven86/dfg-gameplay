#include "pch.h"
#include "utils.h"
#include "zlib.h"
#include "uuid.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <iomanip>




namespace Utils {


std::string generateUUID( )
{
    std::random_device rd;
    auto seed_data = std::array<int, std::mt19937::state_size>{};
    std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::mt19937 generator(seq);
    return uuids::to_string(uuids::uuid_random_generator{ generator }());
}





bool serializeString(gameplay::Stream * stream, const std::string& str)
{
    int32_t size = static_cast<int32_t>(str.size());
    if (stream->write(&size, sizeof(size), 1) != 1)
        return false;
    if (stream->write(str.c_str(), sizeof(char), size) != size)
        return false;
    return true;
}

bool deserializeString(gameplay::Stream * stream, std::string * str)
{
    int32_t size = 0;
    if (stream->read(&size, sizeof(size), 1) != 1)
        return false;

    if (size <= 0)
        return false;

    std::unique_ptr<char[]> buf(new char[size]);
    if (stream->read(buf.get(), 1, size) != size)
        return false;

    str->assign(buf.get(), size);
    return true;
}


unsigned long compressToStream(const void * data, size_t dataLength, gameplay::Stream * stream, void * tmpBuf, size_t tmpBufSize)
{
    z_stream defstream;

    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    defstream.avail_in = dataLength;
    defstream.next_in = (Bytef *)data;
    deflateInit(&defstream, Z_BEST_COMPRESSION);

    do
    {
        defstream.avail_out = tmpBufSize;
        defstream.next_out = (Bytef *)tmpBuf;

        deflate(&defstream, Z_FINISH);

        if (stream)
            stream->write(tmpBuf, tmpBufSize - defstream.avail_out, 1);
    } while (defstream.avail_out == 0);

    deflateEnd(&defstream);

    return defstream.total_out;
}


// Function to calculate HMAC-SHA256
std::string calculateHMAC_SHA256(const std::string& key, const std::string& data) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    // Calculate HMAC
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key.c_str(), key.length(), EVP_sha256(), nullptr);
    HMAC_Update(ctx, reinterpret_cast<const unsigned char*>(data.c_str()), data.length());
    HMAC_Final(ctx, hash, &hash_len);
    HMAC_CTX_free(ctx);

    // Convert the hash to a hexadecimal string
    std::stringstream ss;
    for (unsigned int i = 0; i < hash_len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

}