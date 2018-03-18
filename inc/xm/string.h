#ifndef XM_STRING_H_
#define XM_STRING_H_ 1

#include "basics.h"
#include "list.h"

namespace xm {

    struct string;
    static inline void swap(string& flip, string& flop);
    static inline string substr(const string& ss, int64 off, int64 len);
    static inline string join(const list<string>& ll);

    struct string {
        inline ~string();
        inline string();
        inline string(const string& other);
        inline string(const char* ptr);
        inline string& operator =(const string& other);
        inline string& operator =(const char* ptr);

        inline int64 size() const;
        inline int64 codes() const;
        inline const char* data() const;

        private:
            // only friend functions can see these two
            inline string(int64 len);
            inline char* ptr();

            friend string substr(const string& ss, int64 off, int64 len);
            friend string join(const list<string>& ll);

            friend void swap(string& flip, string& flop);
            char* storage;
    };

    //{{{ internal
    namespace internal {

        static inline bool utf8trail(
            const char*& ptr, int32_t mask, int64 len, int32_t& code
        ) {
            int32_t sum = *ptr++ & mask;
            for (int64 ii = 0; ii<len; ii++) {
                if (*ptr == 0) return false;
                if ((*ptr & 0xC0) != 0x80) {
                    ++ptr;
                    return false;
                }
                sum <<= 6;
                sum |= *ptr++ & 0x3F;
            }
            code = sum;
            return true;
        }

        static inline bool utf8decode(const char*& ptr, int32_t& code) {
            check(*ptr != 0, "can't decode null terminator");
            if ((0x80 & *ptr) == 0) {
                code = *ptr++;
                return true;
            }
            if ((0xE0 & *ptr) == 0xC0) return utf8trail(ptr, 0x1F, 1, code);
            if ((0xF0 & *ptr) == 0xE0) return utf8trail(ptr, 0x0F, 2, code);
            if ((0xF8 & *ptr) == 0xF0) return utf8trail(ptr, 0x07, 3, code);
            if ((0xFC & *ptr) == 0xF8) return utf8trail(ptr, 0x03, 4, code);
            if ((0xFE & *ptr) == 0xFC) return utf8trail(ptr, 0x01, 5, code);
            ++ptr;
            code = 0xFFFD;
            return false;
        }

        static inline int64 utf8bytes(int32_t code) {
            check(code >= 0, "non-negative code point");
            if (code == 0) return 2; // modified UTF-8 with embedded nulls
            if (code < 0x80) return 1;
            if (code < 0x800) return 2;
            if (code < 0x10000) return 3;
            if (code < 0x200000) return 4;
            if (code < 0x4000000) return 5;
            return 6;
        }

        static inline int64 utf8encode(char* ptr, int32_t code) {
            check(code >= 0, "non-negative code point");
            if (code == 0) { // modified UTF-8 with embedded nulls
                ptr[0] = 0xC0;
                ptr[1] = 0x80;
                return 2;
            }
            if (code < 0x80) {
                ptr[0] = code;
                return 1;
            }
            if (code < 0x800) {
                ptr[0] = (code>>6) | 0xC0;
                ptr[1] = (code & 0x3F) | 0x80;
                return 2;
            }
            if (code < 0x10000) {
                ptr[0] =  (code>>12) | 0xE0;
                ptr[1] = ((code>> 6) & 0x3F) | 0x80;
                ptr[2] = ((code    ) & 0x3F) | 0x80;
                return 3;
            }
            if (code < 0x200000) {
                ptr[0] =  (code>>18) | 0xF0;
                ptr[1] = ((code>>12) & 0x3F) | 0x80;
                ptr[2] = ((code>> 6) & 0x3F) | 0x80;
                ptr[3] = ((code    ) & 0x3F) | 0x80;
                return 4;
            }
            if (code < 0x4000000) {
                ptr[0] =  (code>>24) | 0xF8;
                ptr[1] = ((code>>18) & 0x3F) | 0x80;
                ptr[2] = ((code>>12) & 0x3F) | 0x80;
                ptr[3] = ((code>> 6) & 0x3F) | 0x80;
                ptr[4] = ((code    ) & 0x3F) | 0x80;
                return 5;
            }
            ptr[0] =  (code>>30) | 0xFC;
            ptr[1] = ((code>>24) & 0x3F) | 0x80;
            ptr[2] = ((code>>18) & 0x3F) | 0x80;
            ptr[3] = ((code>>12) & 0x3F) | 0x80;
            ptr[4] = ((code>> 6) & 0x3F) | 0x80;
            ptr[5] = ((code    ) & 0x3F) | 0x80;
            return 6;
        }

        static inline int64 utf8sanitize(char* writer, const char* reader) {
            int64 len = 0;
            // We use the "Replacement Character" for all decoding errors
            const int32_t error = 0xFFFD;
            while (*reader) {
                int32_t code = -1;
                if (utf8decode(reader, code)) {
                    // check for the first of a Surrogate Pair
                    if (0xD800 <= code && code < 0xDC00) {
                        // grab the second of a Surrogate Pair
                        int32_t pair = -1;
                        if (*reader && utf8decode(reader, pair)) {
                            if (0xDC00 <= pair && pair < 0xE000) {
                                // it's a valid pair
                                code = ((code-0xD800)<<10) | (pair-0xDC00);
                            } else {
                                code = error;
                            }
                        } else {
                            code = error;
                        }
                    }
                    // check these after surrogate processing
                    // quietly discard any Byte Order Marks
                    if (code == 0xFEFF) continue;
                    if (code == 0xFFFE) continue;
                    // surrogate encoded surrogates become errors
                    if (0xD800 <= code && code < 0xE000) code = error;
                    // this would be pedantic and discard non-unicode
                    // if (code > 0x10FFFF) code = error;
                } else {
                    code = error;
                }
                len += utf8bytes(code);
                if (writer) writer += utf8encode(writer, code);
            }
            if (writer) *writer = 0;
            return len;
        }
    }
    //}}}

    string::~string() { if (storage) free(storage); }

    string::string() : storage(0) {}

    string::string(const string& other) : storage(0) {
        if (other.storage) {
            int64 len = ::strlen(other.storage);
            storage = alloc<char>(len*sizeof(char));
            memcpy(storage, other.storage, len + 1);
        }
    }

    string::string(const char* ptr) : storage(0) {
        using namespace internal;
        if (*ptr) {
            int64 len = utf8sanitize(0, ptr);
            storage = alloc<char>(len*sizeof(char));
            utf8sanitize(storage, ptr);
        }
    }

    string::string(int64 len) : storage(alloc<char>(len*sizeof(char))) {
        storage[len] = 0;
    }

    string& string::operator =(const string& other) {
        if (&other == this) return *this;
        if (storage) free(storage);
        if (other.storage) {
            int64 len = ::strlen(other.storage);
            storage = alloc<char>(len*sizeof(char));
            memcpy(storage, other.storage, len + 1);
        } else {
            storage = 0;
        }
        return *this;
    }

    string& string::operator =(const char* ptr) {
        using namespace internal;
        if (storage) free(storage);
        if (*ptr) {
            int64 len = utf8sanitize(0, ptr);
            storage = alloc<char>(len*sizeof(char));
            utf8sanitize(storage, ptr);
        } else {
            storage = 0;
        }
        return *this;
    }

    int64 string::size() const {
        return storage ? ::strlen(storage) : 0;
    }

    int64 string::codes() const {
        using namespace internal;
        if (!storage) return 0;
        const char* ptr = storage;
        int64 len = 0;
        while (*ptr) {
            int32_t code;
            utf8decode(ptr, code);
            ++len;
        }
        return len;
    }

    const char* string::data() const {
        return storage ? storage : "";
    }

    char* string::ptr() {
        return storage;
    }

    static inline string operator +(const string& aa, const string& bb) {
        list<string> ll;
        ll.append(aa);
        ll.append(bb);
        return join(ll);
    }

    static inline bool operator <(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) < 0;
    }

    static inline bool operator <=(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) <= 0;
    }

    static inline bool operator >=(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) >= 0;
    }

    static inline bool operator >(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) > 0;
    }

    static inline bool operator ==(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) == 0;
    }

    static inline bool operator !=(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) != 0;
    }

    __attribute__ ((format (printf, 1, 2))) 
    static inline string format(const char* fmt, ...) {
        char buffer[64];
        va_list args;
        va_start(args, fmt);
        int64 bytes;
        if ((bytes = vsnprintf(buffer, 64, fmt, args)) < 64) {
            va_end(args);
            return string(buffer);
        }
        va_end(args);

        va_list again;
        va_start(again, fmt);
        char* scratch = alloc<char>(bytes*sizeof(char));
        vsnprintf(scratch, bytes + 1, fmt, again);
        va_end(again);
        string result(scratch);
        free(scratch);
        return result;
    }

    static inline string substr(const string& ss, int64 off, int64 len) {
        string result(len);
        memcpy(result.ptr(), ss.data() + off, len);
        return result;
    }

    static inline string strip(const string& text) {
        // XXX: Is this correct for UTF-8 codes?
        const char* ptr = text.data();
        int64 off = 0;
        int64 len = text.size();

        while (len > 0 && ::isspace(ptr[off])) { off++; len--; }
        while (len > 0 && ::isspace(ptr[off + len - 1])) { len--; }

        return substr(text, off, len);
    }

    static inline list<string> split(const string& text) {
        int64 off = 0;
        int64 len = text.size();
        const char* ptr = text.data();

        while (len > 0 && isspace(ptr[off])) { off++; len--; }

        list<string> result;

        while (len > 0) {
            int64 cut = 0;
            while (cut < len && !isspace(ptr[off + cut])) { cut++; }
            if (cut > 0) result.append(substr(text, off, cut));
            off += cut;
            len -= cut;
            while (len > 0 && isspace(ptr[off])) { off++; len--; }
        }

        return result;
    }

    /* XXX: finish this?
    static inline list<string> split(const string& text, const string& regex, int64 max = -1) {
        list<string> result;
        int64 off = 0;

        regex_t pattern;
        check(regcomp(&pattern, regex.data(), REG_EXTENDED) == 0, "compiling regex");
        for (;;) {
            regmatch_t match;
            int rc = regexec(&pattern, text.data() + off, 1, &match, 0);
            if (rc == REG_NOMATCH) break;
            result.append(substr(text, off, match.rm_so));
            off += match.rm_eo;

            if (max >= 0 && result.size() >= max - 1) {
                break;
            }

        }
        regfree(&pattern);

        result.append(substr(text, off)); // XXX: need the length

        return result;
    }
    */

    static inline string join(const list<string>& ll) {
        int64 max = 0;
        for (int64 ii = 0; ii<ll.size(); ii++) {
            max += ll[ii].size();
        }
        string result(max);
        char* ptr = result.ptr();
        for (int64 ii = 0; ii<ll.size(); ii++) {
            int64 len = ll[ii].size();
            ::memcpy(ptr, ll[ii].data(), len);
            ptr += len;
        }
        return result;
    }

    static inline int64 rfind(const string& haystack, const string& needle) {
        int64 hlen = haystack.size();
        int64 nlen = needle.size();
        int64 off = hlen - nlen;
        while (off >= 0) {
            if (memcmp(haystack.data() + off, needle.data(), nlen) == 0) {
                return off;
            }
            --off;
        }
        return -1;
    }

    static inline void swap(string& flip, string& flop) {
        swap(flip.storage, flop.storage);
    }

    static inline size_t hash(const string& ss) {
        return hash(ss.data(), ss.size(), 0);
    }

}

#endif // XM_STRING_H_

