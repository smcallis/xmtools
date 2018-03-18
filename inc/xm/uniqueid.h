#ifndef XM_UNIQUEID_H_
#define XM_UNIQUEID_H_ 1

#include <stdint.h>

#include "basics.h"
#include "string.h"
#include "rawfile.h"

namespace xm {

    struct uniqueid {
        uint8_t bytes[16];

        static inline uniqueid parse(const string& text);
    };

    static inline size_t hash(const uniqueid& uuid) {
        return hash(uuid.bytes, 16, 0);
    }

    static inline bool operator == (const uniqueid& aa, const uniqueid& bb) {
        return memcmp(aa.bytes, bb.bytes, 16) == 0;
    }

    static inline bool operator != (const uniqueid& aa, const uniqueid& bb) {
        return memcmp(aa.bytes, bb.bytes, 16) != 0;
    }

    static inline uniqueid genuuid() {
        using namespace internal;
        int fd = open("/dev/urandom", O_RDONLY);
        check(fd >= 0, "opening '/dev/urandom' for reading");
        rawfile file(fd);

        uniqueid result;
        check(file.read(result.bytes, 16), "reading /dev/urandom");
        // this is the mask for randomly generated uuids
        // ffffffff-ffff-4fff-bfff-ffffffffffff
        result.bytes[6] &= 0x0f;
        result.bytes[6] |= 0x40;
        result.bytes[8] &= 0x0f;
        result.bytes[8] |= 0xb0;
        return result;
    }

    static inline string format(const uniqueid& uuid) {
        return format(
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid.bytes[ 0], uuid.bytes[ 1], uuid.bytes[ 2], uuid.bytes[ 3],
            uuid.bytes[ 4], uuid.bytes[ 5], uuid.bytes[ 6], uuid.bytes[ 7],
            uuid.bytes[ 8], uuid.bytes[ 9], uuid.bytes[10], uuid.bytes[11],
            uuid.bytes[12], uuid.bytes[13], uuid.bytes[14], uuid.bytes[15]
        );
    }

    inline uniqueid uniqueid::parse(const string &text) {
        uniqueid uuid;
        if(text.size() == 32) {
            check(sscanf(
                text.data(), "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx"
                "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                uuid.bytes +  0, uuid.bytes +  1, uuid.bytes +  2, uuid.bytes +  3,
                uuid.bytes +  4, uuid.bytes +  5, uuid.bytes +  6, uuid.bytes +  7,
                uuid.bytes +  8, uuid.bytes +  9, uuid.bytes + 10, uuid.bytes + 11,
                uuid.bytes + 12, uuid.bytes + 13, uuid.bytes + 14, uuid.bytes + 15
            ) == 16, "expected 16 pairs of hex chars with no hyphens");
        }
        else {
            check(text.size() == 36, "expecting a 36 byte string");
            check(sscanf(
                text.data(), "%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-"
                "%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                uuid.bytes +  0, uuid.bytes +  1, uuid.bytes +  2, uuid.bytes +  3,
                uuid.bytes +  4, uuid.bytes +  5, uuid.bytes +  6, uuid.bytes +  7,
                uuid.bytes +  8, uuid.bytes +  9, uuid.bytes + 10, uuid.bytes + 11,
                uuid.bytes + 12, uuid.bytes + 13, uuid.bytes + 14, uuid.bytes + 15
            ) == 16, "expected 16 pairs of hex chars with hyphens in the right places");
        }
        return uuid;
    }


}

#endif // XM_UNIQUEID_H_

