#ifndef XM_HASH_H_
#define XM_HASH_H_ 1

#include <stdint.h>

#include "basics.h"

namespace xm {

    // MurmurHash64A from Austin Appleby.  This was copied from a public
    // domain version of the code, and modified slightly for formatting.
    static inline size_t hash(
        const void *ptr, int64 len, unsigned long long seed
    ) {
        typedef unsigned char uint8_t;
        typedef unsigned long long uint64_t;
        const uint64_t mult = 0xC6A4A7935BD1E995ull;
        uint64_t code = seed ^ len*mult;

        const uint64_t* words = (const uint64_t*)ptr;
        for (int64 ii = 0; ii<len/8; ii++) {
            uint64_t word = *words++;
            word *= mult; 
            word ^= word >> 47; 
            word *= mult; 
            code ^= word;
            code *= mult; 
        }

        const uint8_t *bytes = (const uint8_t*)words;
        switch (len & 7) {
            case 7: code ^= (uint64_t)bytes[6] << 48; // falls through
            case 6: code ^= (uint64_t)bytes[5] << 40; // falls through
            case 5: code ^= (uint64_t)bytes[4] << 32; // falls through
            case 4: code ^= (uint64_t)bytes[3] << 24; // falls through
            case 3: code ^= (uint64_t)bytes[2] << 16; // falls through
            case 2: code ^= (uint64_t)bytes[1] <<  8; // falls through
            case 1: code ^= (uint64_t)bytes[0];       // falls through
            code *= mult;
        };
 
        code ^= code >> 47;
        code *= mult;
        code ^= code >> 47;

        return code;
    } 

    size_t hash(char     val) { return hash(&val, sizeof(char    ), 1); }
    size_t hash(int8_t   val) { return hash(&val, sizeof(int8_t  ), 1); }
    size_t hash(int16_t  val) { return hash(&val, sizeof(int16_t ), 1); }
    size_t hash(int32_t  val) { return hash(&val, sizeof(int32_t ), 1); }
    size_t hash(int64_t  val) { return hash(&val, sizeof(int64_t ), 1); }
    size_t hash(uint8_t  val) { return hash(&val, sizeof(uint8_t ), 1); }
    size_t hash(uint16_t val) { return hash(&val, sizeof(uint16_t), 1); }
    size_t hash(uint32_t val) { return hash(&val, sizeof(uint32_t), 1); }
    size_t hash(uint64_t val) { return hash(&val, sizeof(uint64_t), 1); }
    size_t hash(int64    val) { return hash(&val, sizeof(int64   ), 1); }

}

#endif // XM_HASH_H_
