#ifndef XM_BLUEFILES_H_
#define XM_BLUEFILES_H_ 1

namespace xm {

    //{{{ xmheader
    namespace internal {
        // We put all of this stuff in an internal namespace just
        // because it's not typically needed directly.

        static int64 xmsuffix (char suffix) {
            switch (suffix) {
                case 'S': return sizeof(char);
                case 'A': return sizeof(char);
                case 'B': return sizeof(int8_t);
                case 'I': return sizeof(int16_t);
                case 'L': return sizeof(int32_t);
                case 'X': return sizeof(int64_t);
                case 'F': return sizeof(float);
                case 'D': return sizeof(double);
                case 'O': return sizeof(uint8_t);
                case 'U': return sizeof(uint16_t);
                case 'V': return sizeof(uint32_t);
            }
            return 0;
        }

        static int64 xmprefix(char prefix) {
            switch (prefix) {
                case 'S': return  1;
                case 'C': return  2;
                case 'V': return  3;
                case 'Q': return  4;
                case 'M': return  9;
                case 'T': return 16;
                case '1': return  1;
                case '2': return  2;
                case '3': return  3;
                case '4': return  4;
                case '5': return  5;
                case '6': return  6;
                case '7': return  7;
                case '8': return  8;
                case '9': return  9;
                case 'X': return 10;
                case 'A': return 32;
            }
            return 0;
        }

        static int64 xmbytesize(const string& format) {
            //if (memcmp(format, "NH", 2) == 0)  // record files
            //if (memcmp(format, "KW", 2) == 0)  // keyword files
            check(format.size() == 2, "expected two characters '%s'", format.data());
            return xmprefix(format.data()[0]) * xmsuffix(format.data()[1]);
        }

        struct xmheader {
            char     version[4];    // "BLUE"
            char     head_rep[4];   // BigEndian ? "IEEE" : "EEEI"
            char     data_rep[4];   // BigEndian ? "IEEE" : "EEEI"
            int32_t  detached;      // 1 for detached, 0 otherwise
            char     ignore0_[8];
            uint32_t ext_start;     // Offset to extended header /512
            uint32_t ext_size;      // Size of extended header in bytes
            double   data_start;    // Offset to data in bytes
            double   data_size;     // Size of data in bytes
            int32_t  type;          // 1000, 2000, 3000, ...
            char     format[2];     // "SI", "CF", ...
            char     ignore1_[2];
            double   timecode;      // J1950 accurate to microseconds
            char     ignore2_[96];
            int32_t  keylength;     // keyword length in bytes */
            char     keywords[92];  // "KEY1=val1\0KEY2=val2\0"

            double  xstart;        // seconds since the timecode
            double  xdelta;        // delta increments in seconds
            int32_t xunits;
            int32_t subsize;       // frame size for Type 2000 files or
                                   // field count for Type 3000 files
            double  ystart;        // seconds since the timecode
            double  ydelta;        // delta increments in seconds
            int32_t yunits;        // 'xm help units' for more info
            int32_t recsize;       // record size for Type 3000 files

            union {
                struct {
                    /// Record descriptors for Type 3000 files
                    struct {
                        char name[4];
                        char format[2];
                        int16_t offset;
                    } fields[26];
                } std;

                struct {
                    // Record descriptor for Type 5000 statevector files
                    struct {
                        char name[4];
                        char format[2];
                        int8_t type;
                        int8_t units;
                    } fields[14];
                    char frame[8];
                    double quads[11];
                } eph;
            } u;
        };

        static void byteswap8 (void* ptr) {
            char* dst = (char*)ptr;
            char src[8];
            memcpy(src, ptr, 8);
            dst[0] = src[7];
            dst[1] = src[6];
            dst[2] = src[5];
            dst[3] = src[4];
            dst[4] = src[3];
            dst[5] = src[2];
            dst[6] = src[1];
            dst[7] = src[0];
        }

        static void byteswap4(void* ptr) {
            char* dst = (char*)ptr;
            char src[4];
            memcpy(src, ptr, 4);
            dst[0] = src[3];
            dst[1] = src[2];
            dst[2] = src[1];
            dst[3] = src[0];
        }

        static void byteswap2(void* ptr) {
            char* dst = (char*)ptr;
            char src[2];
            memcpy(src, ptr, 2);
            dst[0] = src[1];
            dst[1] = src[0];
        }

#if   __BYTE_ORDER == __LITTLE_ENDIAN
            static const char* xmnative = "EEEI";
#elif __BYTE_ORDER == __BIG_ENDIAN
            static const char* xmnative = "IEEE";
#else
#  error "can't determine the endian"
#endif

        // returns true if the header had to be byteswapped - this means
        // the extended header keywords will have to be byteswapped too.
        static bool fix_header(xmheader* blue) {
            check(memcmp(blue->version, "BLUE", 4) == 0, "expected 'BLUE' header");
            check(
                memcmp(blue->head_rep, "EEEI", 4) == 0 || memcmp(blue->head_rep, "IEEE", 4) == 0,
                "expected 'EEEI' or 'IEEE' for header representation"
            );
            check(
                memcmp(blue->data_rep, "EEEI", 4) == 0 || memcmp(blue->data_rep, "IEEE", 4) == 0,
                "expected 'EEEI' or 'IEEE' for data representation"
            );

            bool byteswap = memcmp(blue->head_rep, xmnative, 4) != 0;
            if (byteswap) {
                // only swap the fields we use
                byteswap4(&blue->ext_start);
                byteswap4(&blue->ext_size);
                byteswap8(&blue->data_start);
                byteswap8(&blue->data_size);
                byteswap4(&blue->type);
                byteswap8(&blue->timecode);
                byteswap4(&blue->keylength);
                // these are common to all BLUE types
                byteswap8(&blue->xstart);
                byteswap8(&blue->xdelta);
                byteswap4(&blue->xunits);
                byteswap4(&blue->subsize);
                byteswap8(&blue->ystart);
                byteswap8(&blue->ydelta);
                byteswap4(&blue->yunits);
                byteswap4(&blue->recsize);

                int64 ii;
                switch (blue->type/1000) {
                    case 1: break; // nothing needed
                    case 2: break; // nothing needed
                    case 3: // fix the fields
                        check(blue->subsize <= 26, "at most 26 fields for Type 3000 (got %d)", blue->subsize);
                        for (ii = 0; ii<blue->subsize; ii++) {
                            byteswap2(&blue->u.std.fields[ii].offset);
                        }
                        break;
                    case 5: // check field count
                        check(blue->subsize <= 14, "at most 14 fields for Type 5000 (got %d)", blue->subsize);
                        for (ii = 0; ii<11; ii++) {
                            byteswap8(&blue->u.eph.quads[ii]);
                        }
                        break;
                    default:
                        check(false, "type %d files are not currently supported", blue->type);
                        break;
                }
            }

            return byteswap;
        }

        /*
        static inline bluefile_t* bluefile_defaults (void) {
            bluefile_t* blue = (bluefile_t*)xalloc(sizeof(bluefile_t));
            memset(blue, 0, sizeof(bluefile_t));
            memcpy(blue->version, "BLUE", 4);
            const char* native = bluefile_native_();
            memcpy(blue->head_rep, native, 4);
            memcpy(blue->data_rep, native, 4);
            blue->type = 1000;
            memcpy(blue->format, "CF", 2);
            blue->xdelta = 1.0;
            blue->xunits = 1;
            blue->ydelta = 1.0;
            blue->yunits = 1;
            return blue;
        }
        */

        static double xmtcprec(const xmheader* blue) {
            const char* ptr = blue->keywords;
            int64 len = blue->keylength;
            for (;;) {
                if (len <= 0 || *ptr == 0) break;
                double tcprec = 0;
                if (sscanf(ptr, " TC_PREC = %lf", &tcprec) == 1) {
                    return tcprec;
                }
                ptr += ::strlen(ptr) + 1;
                len -= ::strlen(ptr) + 1;
            }
            return 0;
        }

        static timecode xmgettime(const xmheader* blue) {
            double tcode = blue->timecode;
            double tcprec = xmtcprec(blue);
            double whole, fract = modf(tcode, &whole);
            fract = round(fract*1e6)*1e-6 + tcprec;
            return normalize((timecode){ (int64)whole, fract });
        }

        static void xmsettime(xmheader* blue, timecode time) {
            double whole, fract = modf(time.fract*1e6, &whole);
            blue->timecode = time.whole + whole*1e-6;
            sprintf(blue->keywords, "TC_PREC=%.6le", fract*1e-6);
            blue->keylength = ::strlen(blue->keywords) + 1;
        }

        static int64 xmxcount(const xmheader* blue) {
            switch (blue->type/1000) {
                case 1: return (int64)floor(
                    blue->data_size / xmbytesize(blue->format)
                );
                case 2: return blue->subsize;
                case 3: // fall through
                case 5: return (int64)floor(
                    blue->data_size / blue->recsize
                );
            }

            return 0;
        }

        static int64 xmycount(const xmheader* blue) {
            if (blue->type/1000 == 2) {
                return (int64)floor(
                    blue->data_size / (
                        blue->subsize * xmbytesize(blue->format)
                    )
                );
            }

            return 1;
        }

        static string terminated(const char* ptr, int64 len) {
            vector<char> temp(len + 1);
            memcpy(temp.data(), ptr, len);
            temp[len] = 0;
            return string(temp.data());
        }

        // Field names are fixed length, but a space or a null means
        // that the string is shorter than the total length.
        static string fortranstr(const char* ptr, int64 len) {
            vector<char> temp(len + 1);
            memcpy(temp.data(), ptr, len);
            temp[len] = 0;
            for (int64 ii = 0; ii<len; ii++) {
                if (temp[ii] == ' ') {
                    temp[ii] = 0;
                    break;
                }
            }
            return string(temp.data());
        }

    }
    //}}}
    //{{{ bluekeyword

    //
    // Keywords only support a single character represenation.  For example,
    // there are no 'CD' (complex double) keywords, just 'D' (double).  All
    // keywords are stored as an array of the type, but frequently there is
    // only one item in the array.  The following codes are understood by this
    // library:
    //
    //  'A' - ascii string
    //  'S' - UTF-8 string
    //
    //  'B' - int8_t
    //  'I' - int16_t
    //  'L' - int32_t
    //  'X' - int64_t
    //
    //  'O' - uint8_t
    //  'U' - uint16_t
    //  'V' - uint32_t
    //
    //  'F' - float
    //  'D' - double
    //
    // 'A', 'L', and 'D' are the most common
    //

    struct bluekeyword {
        string name;
        char code;
        vector<char> bytes;

        inline string       getstr() const;
        inline vector<double>  getflt() const;
        inline vector<int64>   getint() const;
    };

    namespace internal {
        template<class type>
        string intkwdtostr(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            int64 show = min(len, (int64)3);
            list<string> out;
            for (int64 ii = 0; ii<show; ++ii) {
                int64_t value = *(type*)&kwd.bytes[ii*sizeof(type)];
                out.append(format("%ld", value));
                if (ii != show - 1) {
                    out.append(",");
                }
            }
            if (len > show) {
                out.append(", ...");
            }
            return join(out);
        }

        template<class type>
        string unskwdtostr(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            int64 show = min(len, (int64)3);
            list<string> out;
            for (int64 ii = 0; ii<show; ++ii) {
                uint64_t value = *(type*)&kwd.bytes[ii*sizeof(type)];
                out.append(format("%lu", value));
                if (ii != show - 1) {
                    out.append(",");
                }
            }
            if (len > show) {
                out.append(", ...");
            }
            return join(out);
        }

        template<class type>
        string fltkwdtostr(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            int64 show = min(len, (int64)3);
            list<string> out;
            for (int64 ii = 0; ii<show; ++ii) {
                double value = *(type*)&kwd.bytes[ii*sizeof(type)];
                out.append(format("%.18lf", value));
                if (ii != show - 1) {
                    out.append(",");
                }
            }
            if (len > show) {
                out.append(", ...");
            }
            return join(out);
        }

        template<class type>
        vector<double> kwdtoflt(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            vector<double> result(len);

            for (int64 ii = 0; ii<len; ++ii) {
                type value = *(type*)&kwd.bytes[ii*sizeof(type)];
                result[ii] = value;
            }
            return result;
        }

        template<class type>
        vector<int64> intkwdtoint(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            vector<int64_t> result(len);

            for (int64 ii = 0; ii<len; ++ii) {
                type value = *(type*)&kwd.bytes[ii*sizeof(type)];
                result[ii] = value;
            }
            return result;
        }

        template<class type>
        vector<int64> fltkwdtoint(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            vector<int64_t> result(len);

            for (int64 ii = 0; ii<len; ++ii) {
                type value = *(type*)&kwd.bytes[ii*sizeof(type)];
                result[ii] = llrint(value);
            }
            return result;
        }
    }

    string bluekeyword::getstr() const {
        using namespace internal;
        switch (code) {
            case 'S': case 's':
            case 'A': case 'a': {
                int64 len = bytes.size();
                return terminated(bytes.data(), len);
            }
            case 'B': case 'b': return intkwdtostr<int8_t>(*this);
            case 'I': case 'i': return intkwdtostr<int16_t>(*this);
            case 'L': case 'l': return intkwdtostr<int32_t>(*this);
            case 'X': case 'x': return intkwdtostr<int64_t>(*this);
            case 'O': case 'o': return unskwdtostr<uint8_t>(*this);
            case 'U': case 'u': return unskwdtostr<uint16_t>(*this);
            case 'V': case 'v': return unskwdtostr<uint32_t>(*this);
            case 'F': case 'f': return fltkwdtostr<float>(*this);
            case 'D': case 'd': return fltkwdtostr<double>(*this);
        }
        // XXX: Is this the right thing to do?
        return string();
    }

    vector<double> bluekeyword::getflt() const {
        using namespace internal;
        switch (code) {
            case 'S': case 's':
            case 'A': case 'a': {
                string str = getstr();
                char* end = 0;
                double val = ::strtod(str.data(), &end);
                check(end != 0 && (end == str.data() + str.size()), "convert '%s' to double", str.data());
                vector<double> result(1);
                result[0] = val;
                return result;
            }
            case 'B': case 'b': return kwdtoflt<int8_t>(*this);
            case 'I': case 'i': return kwdtoflt<int16_t>(*this);
            case 'L': case 'l': return kwdtoflt<int32_t>(*this);
            case 'X': case 'x': return kwdtoflt<int64_t>(*this);
            case 'O': case 'o': return kwdtoflt<uint8_t>(*this);
            case 'U': case 'u': return kwdtoflt<uint16_t>(*this);
            case 'V': case 'v': return kwdtoflt<uint32_t>(*this);
            case 'F': case 'f': return kwdtoflt<float>(*this);
            case 'D': case 'd': return kwdtoflt<double>(*this);
        }
        return vector<double>();
    }

    vector<int64> bluekeyword::getint() const {
        using namespace internal;
        switch (code) {
            case 'S': case 's':
            case 'A': case 'a': {
                string str = getstr();
                char* end = 0;
                int64 val = ::strtoll(str.data(), &end, 0);
                if (end != str.data() + str.size()) {
                    double flt = strtod(str.data(), &end);
                    check(end != 0 && end == str.data() + str.size(), "convert '%s' to double", str.data());
                    val = llrint(flt);
                }
                vector<int64_t> result(1);
                result[0] = val;
                return result;
            }
            case 'B': case 'b': return intkwdtoint<int8_t>(*this);
            case 'I': case 'i': return intkwdtoint<int16_t>(*this);
            case 'L': case 'l': return intkwdtoint<int32_t>(*this);
            case 'X': case 'x': return intkwdtoint<int64_t>(*this);
            case 'O': case 'o': return intkwdtoint<uint8_t>(*this);
            case 'U': case 'u': return intkwdtoint<uint16_t>(*this);
            case 'V': case 'v': return intkwdtoint<uint32_t>(*this);
            case 'F': case 'f': return fltkwdtoint<float>(*this);
            case 'D': case 'd': return fltkwdtoint<double>(*this);
        }
        return vector<int64>();
    }
    //}}}
    //{{{ bluekeywords

    struct bluekeywords {
        list<bluekeyword> storage;

        inline int64 count(const string& name) const;

        inline bluekeyword  getkwd(const string& name, int64 which=0) const;
        inline string       getstr(const string& name, int64 which=0) const;
        inline vector<double>  getflt(const string& name, int64 which=0) const;
        inline vector<int64>   getint(const string& name, int64 which=0) const;

        inline void update(const string& name, char code, const void* data, int64 len, int64 which);
        inline void update(const string& name, const string&   value,  int64 which=0);
        inline void update(const string& name, double       value,  int64 which=0);
        inline void update(const string& name, vector<double>  values, int64 which=0);
        inline void update(const string& name, int32_t      value,  int64 which=0);
        inline void update(const string& name, vector<int32_t> values, int64 which=0);
        inline void update(const string& name, int64_t      value,  int64 which=0);
        inline void update(const string& name, vector<int64_t> values, int64 which=0);

        inline void insert(const string& name, char code, const void* data, int64 len, int64 where);
        inline void insert(const string& name, const string&   value,  int64 where=-1);
        inline void insert(const string& name, double       value,  int64 where=-1);
        inline void insert(const string& name, vector<double>  values, int64 where=-1);
        inline void insert(const string& name, int32_t      value,  int64 where=-1);
        inline void insert(const string& name, vector<int32_t> values, int64 where=-1);
        inline void insert(const string& name, int64_t      value,  int64 where=-1);
        inline void insert(const string& name, vector<int64_t> values, int64 where=-1);

        inline void remove(const string& name, int64 which=0);

    };

    int64 bluekeywords::count(const string& name) const {
        int64 total = 0;
        for (int64 ii = 0; ii<storage.size(); ii++) {
            if (storage[ii].name == name) total++;
        }
        return total;
    }

    namespace internal {
        // these are some helper routines for the get functions below

        struct xmkeyword {
            uint32_t next_offset;
            uint16_t non_value;
            uint8_t  key_length;
            char     format_code;
            char     buffer[1]; // variable length
        };

        static int64 write_kwds(rawfile* pfile, const bluekeywords& kwds) {
            int64 total_bytes = 0;

            for (int64 ii = 0; ii<kwds.storage.size(); ii++) {
                const bluekeyword& kwd = kwds.storage[ii];
                int64 val_length = kwd.bytes.size();
                int64 key_length = kwd.name.size();
                check(key_length <= 255, "keyword name less than 255 chars (%lld)", key_length);
                int64 data_length = 8 + val_length + key_length;
                int64 padding = (data_length%8) ? 8 - data_length%8 : 0;
                total_bytes += data_length + padding;

                if (pfile) {
                    xmkeyword tmp = xmkeyword();
                    tmp.next_offset = data_length + padding;
                    tmp.non_value = data_length + padding - val_length;
                    tmp.key_length = key_length;
                    tmp.format_code = kwd.code;
                    check(pfile->write(&tmp, 8), "writing keyword descriptor");
                    check(pfile->write(kwd.bytes.data(), val_length), "writing keyword value");
                    check(pfile->write(kwd.name.data(), key_length), "writing keyword name");
                    uint64_t scratch = 0;
                    check(pfile->write(&scratch, padding), "writing keyword padding");
                }
            }

            return total_bytes;
        }

        static bluekeywords readkwds(rawfile& file, int64 len, bool byteswap) {
            vector<char> block(len);
            check(file.read(block.data(), len), "reading keywords");

            bluekeywords result;
            char* ptr = block.data();
            char* end = ptr + len;

            //print("\t\t", "keywords", byteswap ? "are" : "not", "byte swapped");
            while (ptr < end) {
                xmkeyword* kwd = (xmkeyword*)ptr;
                if (byteswap) byteswap4(&kwd->next_offset);
                if (byteswap) byteswap2(&kwd->non_value);
                int64 data_len = kwd->next_offset - kwd->non_value;
                string name = terminated(kwd->buffer + data_len, kwd->key_length);
                vector<char> bytes(data_len);
                memcpy(bytes.data(), kwd->buffer, data_len);
                result.storage.append((bluekeyword){ name, kwd->format_code, bytes});
                ptr += kwd->next_offset;
            }

            if (byteswap) {
                for (int64 ii = 0; ii<result.storage.size(); ii++) {
                    bluekeyword& kwd = result.storage[ii];
                    switch (kwd.code) {
                        case 'I':
                        case 'U': {
                            int64 count = kwd.bytes.size()/2;
                            for (int64 jj = 0; jj<count; jj++) {
                                byteswap2(&kwd.bytes[jj*2]);
                            }
                        } break;

                        case 'L':
                        case 'V':
                        case 'F': {
                            int64 count = kwd.bytes.size()/4;
                            for (int64 jj = 0; jj<count; jj++) {
                                byteswap4(&kwd.bytes[jj*4]);
                            }
                        } break;

                        case 'X':
                        case 'D': {
                            int64 count = kwd.bytes.size()/8;
                            for (int64 jj = 0; jj<count; jj++) {
                                byteswap8(&kwd.bytes[jj*8]);
                            }
                        } break;
                    }
                }
            }

            return result;
        }
    }

    bluekeyword bluekeywords::getkwd(const string& name, int64 which) const {
        check(which >= 0, "keyword index must be non-negative (%lld)", which);
        for (int64 ii = 0; ii<storage.size(); ii++) {
            const bluekeyword& kwd = storage[ii];
            if (kwd.name == name) {
                if (which == 0) {
                    return kwd;
                }
                --which;
            }
        }
        check(false, "requested non-existent keyword '%s'", name.data());
        return (bluekeyword){ "", 0, vector<char>() };
    }

    string bluekeywords::getstr(const string& name, int64 which) const {
        return getkwd(name, which).getstr();
    }

    vector<int64> bluekeywords::getint(const string& name, int64 which) const {
        return getkwd(name, which).getint();
    }

    vector<double> bluekeywords::getflt(const string& name, int64 which) const {
        return getkwd(name, which).getflt();
    }

    void bluekeywords::update(const string& name, char code, const void* data, int64 len,  int64 which) {
        check(which >= 0, "must specify non-negative keyword index (%lld)", which);
        for (int64 ii = 0; ii<storage.size(); ii++) {
            bluekeyword& kwd = storage[ii];
            if (kwd.name == name) {
                if (which == 0) {
                    vector<char> bytes(len);
                    memcpy(&bytes[0], data, len);
                    kwd.code = code;
                    swap(kwd.bytes, bytes);
                    return;
                } else {
                    --which;
                }
            }
        }
        check(false, "keyword to update not found '%s' (%lld)", name.data(), which);
    }

    void bluekeywords::update(const string& name, const string& value, int64 which) {
        update(name, 'A', value.data(), value.size(), which);
    }

    void bluekeywords::update(const string& name, double value, int64 which) {
        update(name, 'D', &value, sizeof(double), which);
    }

    void bluekeywords::update(const string& name, vector<double> values, int64 which) {
        update(name, 'D', values.data(), values.size()*sizeof(double), which);
    }

    void bluekeywords::update(const string& name, int32_t value, int64 which) {
        update(name, 'L', &value, sizeof(int32_t), which);
    }

    void bluekeywords::update(const string& name, vector<int32_t> values, int64 which) {
        update(name, 'L', values.data(), values.size()*sizeof(int32_t), which);
    }

    void bluekeywords::update(const string& name, int64_t value, int64 which) {
        update(name, 'X', &value, sizeof(int64), which);
    }

    void bluekeywords::update(const string& name, vector<int64_t> values, int64 which) {
        update(name, 'X', values.data(), values.size()*sizeof(int64), which);
    }

    void bluekeywords::insert(const string& name, char code, const void* data, int64 len, int64 where) {
        if (where < 0) where = storage.size();
        check(where <= storage.size(), "index in bounds %lld [0, %lld]", where, storage.size());
        vector<char> bytes(len);
        memcpy(&bytes[0], data, len);
        storage.insert(where, (bluekeyword){ name, code, bytes });
    }

    void bluekeywords::insert(const string& name, const string& value, int64 where) {
        insert(name, 'A', value.data(), value.size(), where);
    }

    void bluekeywords::insert(const string& name, double value,  int64 where) {
        insert(name, 'D', &value, sizeof(double), where);
    }

    void bluekeywords::insert(const string& name, vector<double>  values, int64 where) {
        insert(name, 'D', values.data(), values.size()*sizeof(double), where);
    }

    void bluekeywords::insert(const string& name, int64_t value, int64 where) {
        insert(name, 'X', &value, sizeof(int64), where);
    }

    void bluekeywords::insert(const string& name, vector<int64_t> values, int64 where) {
        insert(name, 'X', values.data(), values.size()*sizeof(int64), where);
    }

    void bluekeywords::insert(const string& name, int32_t value, int64 where) {
        insert(name, 'L', &value, sizeof(int32_t), where);
    }

    void bluekeywords::insert(const string& name, vector<int32_t> values, int64 where) {
        insert(name, 'L', values.data(), values.size()*sizeof(int32_t), where);
    }

    void bluekeywords::remove(const string& name, int64 which) {
        check(which >= 0, "must specify non-negative keyword index (%lld)", which);
        for (int64 ii = 0; ii<storage.size(); ii++) {
            if (storage[ii].name == name) {
                storage.remove(ii);
                return;
            }
        }
        check(false, "keyword to remove not found '%s' (%lld)", name.data(), which);
    }

    //}}}
    //{{{ bluefield
    struct bluefield {
        string name;
        string format;
        int16_t offset;
    };
    //}}}
    //{{{ bluemeta

    namespace blueunits {
        enum {
            none    =  0, // unitless
            time    =  1, // seconds
            delay   =  2, // seconds
            freq    =  3, // Hertz
            pos     =  5, // meters
            vel     =  6, // meters/sec
            acc     =  7, // meters/s^2
            jerk    =  8, // meters/s^3
            doppler =  9, // Hertz
            chirp   = 10, // Hertz/sec
            radians = 23, // radians
            degrees = 24, // degrees
            cycles  = 25, // cycles
            decibel = 35, // unitless
            lat     = 60, // degrees
            lon     = 61, // degrees
            alt     = 63, // meters
        };
    }

    namespace internal {
        static inline string xmunits (uint8_t code) {
            switch (code) {
                case blueunits::none:    return "none (unitless)";
                case blueunits::time:    return "time (seconds)";
                case blueunits::delay:   return "delay (seconds)";
                case blueunits::freq:    return "freq (Hertz)";
                case blueunits::pos:     return "pos (meters)";
                case blueunits::vel:     return "vel (meters/sec)";
                case blueunits::acc:     return "acc (meters/s^2)";
                case blueunits::jerk:    return "jerk (meters/s^3)";
                case blueunits::doppler: return "doppler (Hertz)";
                case blueunits::chirp:   return "chirp (Hertz/sec)";
                case blueunits::radians: return "angle (radians)";
                case blueunits::degrees: return "angle (degrees)";
                case blueunits::cycles:  return "angle (cycles)";
                case blueunits::decibel: return "dB (unitless)";
                case blueunits::lat:     return "lat (degrees)";
                case blueunits::lon:     return "lon (degrees)";
                case blueunits::alt:     return "alt (meters)";
                default: break;
            }
            return "other unknown";
        }
    }

    struct bluemeta {
        int32_t type;
        timecode time;
        string format;
        int32_t itemsize;
        int64   xcount, ycount;
        double  xstart, ystart;
        double  xdelta, ydelta;
        int32_t xunits, yunits;
        bluekeywords kwds;
        list<bluefield> fields;
    };

    namespace internal {
        static inline xmheader init_header(const bluemeta& meta) {
            check(sizeof(xmheader) == 512, "sanity");
            // need to write out the header, keywords, and padding
            xmheader hdr = xmheader();
            memcpy(hdr.version, "BLUE", 4);
            memcpy(hdr.head_rep, xmnative, 4);
            memcpy(hdr.data_rep, xmnative, 4);
            xmsettime(&hdr, meta.time);

            hdr.type = meta.type;
            check(meta.format.size() == 2, "expect digraph");
            memcpy(hdr.format, meta.format.data(), 2);

            int64 keyword_bytes = write_kwds(0, meta.kwds);
            //int64 padding_bytes = keyword_bytes%512 ? 512 - keyword_bytes%512 : 0;
            if (keyword_bytes) {
                hdr.ext_start = 1; // times 512
                hdr.ext_size = keyword_bytes;
            }

            hdr.data_start = 512 + keyword_bytes;// + padding_bytes;

            switch (hdr.type/1000) {
                case 1: {
                    int64 itemsize = xmbytesize(meta.format);
                    check(itemsize > 0, "supported sample format '%s'", meta.format.data());
                    check(meta.xcount >= 0, "non-negative xcount (%lld)", meta.xcount);
                    hdr.data_size = meta.xcount * itemsize;
                    hdr.xstart = meta.xstart;
                    hdr.xdelta = meta.xdelta;
                    hdr.xunits = meta.xunits;
                } break;

                case 2: {
                    int64 itemsize = xmbytesize(meta.format);
                    check(itemsize > 0, "supported sample format '%s'", meta.format.data());
                    check(meta.xcount >= 0, "non-negative xcount (%lld)", meta.xcount);
                    check(meta.ycount >= 0, "non-negative ycount (%lld)", meta.ycount);
                    hdr.data_size = meta.ycount * meta.xcount * itemsize;
                    hdr.xstart = meta.xstart;
                    hdr.xdelta = meta.xdelta;
                    hdr.xunits = meta.xunits;
                    hdr.subsize = meta.xcount;
                    hdr.ystart = meta.ystart;
                    hdr.ydelta = meta.ydelta;
                    hdr.yunits = meta.yunits;
                } break;

                case 3: {
                    int64 itemsize = meta.itemsize;
                    check(itemsize > 0, "need positive itemsize (%lld)", itemsize);
                    check(meta.xcount >= 0, "non-negative xcount (%lld)", meta.xcount);
                    check(meta.fields.size() > 0, "need some fields");
                    hdr.data_size = meta.xcount * itemsize;
                    hdr.xstart = meta.xstart;
                    hdr.xdelta = meta.xdelta;
                    hdr.xunits = meta.xunits;
                    hdr.subsize = meta.fields.size();
                    hdr.recsize = itemsize;
                    check(meta.fields.size() <= 26, "can't have more than 26 fields");
                    for (int64 ii = 0; ii<meta.fields.size(); ii++) {
                        const bluefield& field = meta.fields[ii];
                        check(field.name.size() <= 4, "max of 4 character field name '%s'", field.name.data());
                        memset(hdr.u.std.fields[ii].name, ' ', 4);
                        memcpy(hdr.u.std.fields[ii].name, field.name.data(), field.name.size());
                        check(field.format.size() == 2, "need digraph for field type '%s'", field.format.data());
                        memcpy(hdr.u.std.fields[ii].format, field.format.data(), 2);
                        hdr.u.std.fields[ii].offset = field.offset;
                    }

                } break;

                case 5: {
                    int64 itemsize = meta.itemsize;
                    check(itemsize > 0, "need positive itemsize (%lld)", itemsize);
                    check(meta.xcount >= 0, "non-negative xcount (%lld)", meta.xcount);
                    check(meta.fields.size() > 0, "need some fields");
                    hdr.data_size = meta.xcount * itemsize;
                    hdr.xstart = meta.xstart;
                    hdr.xdelta = meta.xdelta;
                    hdr.xunits = meta.xunits;
                    hdr.subsize = meta.fields.size();
                    hdr.recsize = itemsize;
                    check(meta.fields.size() <= 14, "can't have more than 14 fields");
                    int64 offset = 0;
                    for (int64 ii = 0; ii<meta.fields.size(); ii++) {
                        const bluefield& field = meta.fields[ii];
                        check(field.name.size() <= 4, "max of 4 character field name '%s'", field.name.data());
                        memset(hdr.u.eph.fields[ii].name, ' ', 4);
                        memcpy(hdr.u.eph.fields[ii].name, field.name.data(), field.name.size());
                        check(field.format.size() == 2, "need digraph for field type '%s'", field.format.data());
                        memcpy(hdr.u.eph.fields[ii].format, field.format.data(), 2);
                        check(offset == field.offset, "data offset (%lld) == field offset (%d)", offset, field.offset);
                        int64 bytes = xmbytesize(field.format.data());
                        check(bytes > 0, "need supported data format '%s'", field.format.data());
                        if (memcmp(field.name.data(), "POS", 3) == 0) {
                            hdr.u.eph.fields[ii].type  = 2;
                            hdr.u.eph.fields[ii].units = 5;
                        }
                        if (memcmp(field.name.data(), "VEL", 3) == 0) {
                            hdr.u.eph.fields[ii].type  = 2;
                            hdr.u.eph.fields[ii].units = 6;
                        }
                        if (memcmp(field.name.data(), "ACC", 3) == 0) {
                            hdr.u.eph.fields[ii].type  = 2;
                            hdr.u.eph.fields[ii].units = 7;
                        }
                        if (memcmp(field.name.data(), "TIME", 3) == 0) {
                            hdr.u.eph.fields[ii].type  = 1;
                            hdr.u.eph.fields[ii].units = 1;
                        }
                        offset += bytes;
                    }
                    check(itemsize == offset, "total itemsize (%lld) == sum of records (%lld)", itemsize, offset);
                    memcpy(hdr.u.eph.frame, "ECR     ", 8); // for building ephemeris
                } break;

                default:
                    check(false, "Type %d files are not supported", hdr.type);
            }

            return hdr;
        }

    }

    //}}}
    //{{{ bluereader

    struct bluereader {
        //inline ~bluereader() = default;
        //inline bluereader() = default;
        //inline bluereader(const bluereader&) = default;
        inline bluereader(const string& path);

        //inline bluereader& operator =(const bluereader&) = default;

        inline bool kwds_ready() const;
        inline bool is_swapped() const;

        inline const bluemeta* operator ->() const;
        inline const bluemeta& operator *() const;

        // this is the low level grab function, it works on bytes
        inline void grab(int64 offset, void* buffer, int64 length);

        // this grab function will do conversion and byteswapping
        inline void grabcf(int64 offset, cfloat* data, int64 length);

        inline const void* mmap();

        inline int64 byte_offset(int64 sample);

        private:
            struct implementation {
                internal::rawfile file;
                bluemeta meta;
                int64 data_offset;
                int64 data_length;
                int64 error_offset;
                int64 cache_offset;
                int64 cache_length;
                list<vector<char> > cache;
                vector<char> scratch;
                bool is_swapped;
                bool kwds_ready;
            };
            shared<implementation*> pimpl;
    };

    bluereader::bluereader(const string& path) {
        using namespace internal;
        check(sizeof(xmheader) == 512, "sanity");

        // open the file or duplicate stdin
        int fd = path == "-" ? dup(0) : open(path.data(), O_RDONLY);
        check(fd >= 0, "opening '%s' for reading", path.data());
        rawfile file(fd);

        // read the header
        xmheader hdr;
        check(file.read(&hdr, sizeof(xmheader)), "reading X-Midas header");
        bool byteswap_kwds = fix_header(&hdr);

        int64 kwds_offset = hdr.ext_start * 512LL;
        int64 kwds_length = hdr.ext_size;
        int64 data_offset = llrint(hdr.data_start);
        int64 data_length = llrint(hdr.data_size);

        bluekeywords keywords;
        bool kwds_ready = false;
        if (hdr.detached) {
            if (kwds_length > 0) {
                check(file.skip(kwds_offset - 512), "skipping to keywords");
                keywords = readkwds(file, kwds_length, byteswap_kwds);
            }
            kwds_ready = true;

            // open the detached data portion of the file
            int64 pos = rfind(path, ".");
            check(pos >= 0, "need file extension: %s", path.data());
            string detpath = substr(path, 0, pos) + ".det";
            int detfd = open(detpath.data(), O_RDONLY);
            check(detfd >= 0, "opening '%s' for reading", detpath.data());
            rawfile detfile(detfd);
            swap(file, detfile);
            check(file.seek(data_offset), "seeking to detached data start");

        } else {
            if (kwds_length > 0) {
                if (kwds_offset < data_offset) {
                    // it's in pipe order, we can skip to the keywords
                    check(file.skip(kwds_offset - 512), "skipping to keywords");
                    keywords = readkwds(file, kwds_length, byteswap_kwds);
                    // now skip to the data
                    check(
                        file.skip(data_offset - (kwds_offset + kwds_length)),
                        "skipping to data"
                    );
                    kwds_ready = true;
                } else if (file.isfile()) {
                    // it's not pipe order, but  we can seek to the
                    // keywords and seek back to the data afterwards
                    check(file.seek(kwds_offset), "seeking to keywords");
                    keywords = readkwds(file, kwds_length, byteswap_kwds);
                    check(file.seek(data_offset), "seeking to data start");
                    kwds_ready = true;
                } else {
                    // the keywords come after the data, and it's in a pipe
                    // all we can do is skip to the data and ignore keywords
                    check(file.skip(data_offset - 512), "skipping to data offset");
                }
            } else {
                // no keywords, so we just skip to the offset
                check(file.skip(data_offset - 512), "skipping to data offset");
                kwds_ready = true;
            }
        }

        shared<implementation*> tmp(new implementation());
        swap(pimpl, tmp);
        swap(pimpl.value()->file, file);
        pimpl.value()->meta.type     = hdr.type;
        pimpl.value()->meta.time     = xmgettime(&hdr);
        pimpl.value()->meta.format   = terminated(hdr.format, 2);
        pimpl.value()->meta.kwds     = keywords;
        pimpl.value()->data_offset   = data_offset;
        pimpl.value()->data_length   = data_length;
        pimpl.value()->error_offset  = INT64_MIN;
        pimpl.value()->cache_offset  = 0;
        pimpl.value()->cache_length  = 0;
        pimpl.value()->is_swapped    = memcmp(hdr.data_rep, xmnative, 4) != 0;
        pimpl.value()->kwds_ready    = kwds_ready;

        switch (hdr.type/1000) {
            case 1:
                pimpl.value()->meta.itemsize = xmbytesize(hdr.format);
                check(pimpl.value()->meta.itemsize != 0, "recognized sample type");
                pimpl.value()->meta.xcount   = xmxcount(&hdr);
                pimpl.value()->meta.xstart   = hdr.xstart;
                pimpl.value()->meta.xdelta   = hdr.xdelta;
                pimpl.value()->meta.xunits   = hdr.xunits;
                pimpl.value()->meta.ycount   = 1;
                break;

            case 2:
                pimpl.value()->meta.itemsize = xmbytesize(hdr.format);
                check(pimpl.value()->meta.itemsize != 0, "recognized sample type");
                pimpl.value()->meta.xcount   = xmxcount(&hdr);
                pimpl.value()->meta.xstart   = hdr.xstart;
                pimpl.value()->meta.xdelta   = hdr.xdelta;
                pimpl.value()->meta.xunits   = hdr.xunits;
                pimpl.value()->meta.ycount   = xmycount(&hdr);
                pimpl.value()->meta.ystart   = hdr.ystart;
                pimpl.value()->meta.ydelta   = hdr.ydelta;
                pimpl.value()->meta.yunits   = hdr.yunits;
                break;

            case 3:
                pimpl.value()->meta.itemsize = hdr.recsize;
                check(pimpl.value()->meta.itemsize > 0, "positive record size");
                pimpl.value()->meta.xcount   = llrint(hdr.data_size)/hdr.recsize;
                pimpl.value()->meta.xstart   = hdr.xstart;
                pimpl.value()->meta.xdelta   = hdr.xdelta;
                pimpl.value()->meta.xunits   = hdr.xunits;
                pimpl.value()->meta.ycount   = 1;

                for (int64 ii = 0; ii<hdr.subsize; ii++) {
                    pimpl.value()->meta.fields.append(
                        (bluefield){
                            fortranstr(hdr.u.std.fields[ii].name, 4),
                            terminated(hdr.u.std.fields[ii].format, 2),
                            hdr.u.std.fields[ii].offset
                        }
                    );
                }
                break;

            case 5:
                pimpl.value()->meta.itemsize = hdr.recsize;
                check(pimpl.value()->meta.itemsize > 0, "positive record size");
                pimpl.value()->meta.xcount   = llrint(hdr.data_size)/hdr.recsize;
                pimpl.value()->meta.xstart   = hdr.xstart;
                pimpl.value()->meta.xdelta   = hdr.xdelta;
                pimpl.value()->meta.xunits   = hdr.xunits;
                pimpl.value()->meta.ycount   = 1;
                // This doesn't really belong here, but we assume that all Type
                // 5000 files are ephemeris in the ECR (ECF) coordinate system.
                // It'll be good to catch errors if we ever find one that isn't.
                check(memcmp("ECR", hdr.u.eph.frame, 3) == 0, "ECR coordinates");

                /*
                for (int64 ii = 0; ii<3; ii++) {
                    printf("type: %d, units: %d\n", hdr.u.eph.fields[ii].type, hdr.u.eph.fields[ii].units);
                }

                printf("frame: ");
                for (int64 ii = 0; ii<8; ii++) {
                    printf("'%c' ", hdr.u.eph.frame[ii]);
                }
                printf("\n");
                for (int64 ii = 0; ii<11; ii++) {
                    printf("quads[%lld] == %lf\n", ii, hdr.u.eph.quads[ii]);
                }
                */

                for (int64 offset = 0, ii = 0; ii<hdr.subsize; ii++) {
                    string format = terminated(hdr.u.eph.fields[ii].format, 2);
                    int64 bytes = xmbytesize(format);
                    pimpl.value()->meta.fields.append(
                        (bluefield){
                            fortranstr(hdr.u.eph.fields[ii].name, 4),
                            format, (int16_t)offset
                        }
                    );
                    check(bytes > 0, "need supported format '%s'", format.data());
                    offset += bytes;
                }
                break;

            default:
                check(false, "Type %d files are not supported", hdr.type);
        }
    }

    bool bluereader::kwds_ready() const {
        check(pimpl.valid(), "need an opened file");
        return pimpl.value()->kwds_ready;
    }

    bool bluereader::is_swapped() const {
        check(pimpl.valid(), "need an opened file");
        return pimpl.value()->is_swapped;
    }

    const bluemeta* bluereader::operator ->() const {
        check(pimpl.valid(), "need an opened file");
        return &pimpl.value()->meta;
    }

    const bluemeta& bluereader::operator *() const {
        check(pimpl.valid(), "need an opened file");
        return pimpl.value()->meta;
    }

    void bluereader::grab(int64 offset, void* buffer, int64 length) {
        char* pointer = (char*)buffer;
        check(pimpl.valid(), "need an opened file");
        // first we check that they don't go backwards
        check(
            offset >= pimpl.value()->error_offset,
            "not allowed to read backward in file %lld < %lld",
            offset, pimpl.value()->error_offset
        );
        pimpl.value()->error_offset = offset;
        if (length == 0) return;

        // zeros before the start of the file
        if (offset < 0) {
            int64 amount = min(length, -offset);
            memset(pointer, 0, amount);
            offset  += amount;
            pointer += amount;
            length  -= amount;
            if (length == 0) return;
        }

        // discard blocks before our request
        while (pimpl.value()->cache.size() != 0) {
            vector<char>& block = pimpl.value()->cache[0];
            if (offset < pimpl.value()->cache_offset + (int64)block.size()) break;
            pimpl.value()->cache_offset += block.size();
            pimpl.value()->cache_length -= block.size();
            pimpl.value()->cache.remove(0);
        }

        // skip to the start if it's after our cache
        if (offset > pimpl.value()->cache_offset + pimpl.value()->cache_length) {
            check(pimpl.value()->cache.size() == 0, "sanity");
            check(pimpl.value()->cache_length == 0, "sanity");
            check(pimpl.value()->cache_offset <= pimpl.value()->data_length, "sanity");
            int64 amount = min(
                offset - pimpl.value()->cache_offset,
                pimpl.value()->data_length - pimpl.value()->cache_offset
            );
            check(pimpl.value()->file.skip(amount), "skip %lld", amount);
            pimpl.value()->cache_offset += amount;
        }

        // read one new block to satisfy our request
        int64 cache_ending = pimpl.value()->cache_offset + pimpl.value()->cache_length;
        if (offset + length > cache_ending && cache_ending < pimpl.value()->data_length) {
            const int64 page_size = 65536;
            int64 needed = (offset + length) - cache_ending;
            int64 extra = needed % page_size;
            int64 wanted = needed + (extra ? page_size - extra : 0);
            int64 remaining = pimpl.value()->data_length - cache_ending;
            int64 amount = min(wanted, remaining);
            vector<char> block(amount);
            check(pimpl.value()->file.read(block.data(), amount), "read %lld", amount);
            pimpl.value()->cache.append(vector<char>());
            swap(pimpl.value()->cache[pimpl.value()->cache.size() - 1], block);
            pimpl.value()->cache_length += amount;
            // XXX: Here is where we could check to see if we should read
            // keywords that come at the end of a pipe, but it isn't really
            // needed in practice.
        }

        // copy from the cache to the output
        int64_t block_offset = pimpl.value()->cache_offset;
        for (int64 ii = 0; ii<pimpl.value()->cache.size(); ii++) {
            vector<char>& block = pimpl.value()->cache[ii];
            int64 start = offset - block_offset;
            int64 amount = min(length, (int64)block.size() - start);
            memcpy(pointer, block.data() + start, amount);
            offset  += amount;
            pointer += amount;
            length  -= amount;
            if (length == 0) return;
            block_offset += block.size();
        }

        // zero fill the rest
        memset(pointer, 0, length);
    }

    void bluereader::grabcf(int64 offset, cfloat* samples, int64 length) {
        using namespace internal;
        check(pimpl.valid(), "need an opened file");
        const int64 sample_size = pimpl.value()->meta.itemsize;

        // this one reads directly into the samples vector
        if (pimpl.value()->meta.format == "CF") {
            grab(offset*sample_size, (char*)samples, length*sample_size);

            if (pimpl.value()->is_swapped) {
                float* elems = (float*)samples;
                for (int64 ii = 0; ii<2*length; ii++) {
                    byteswap4(elems + ii);
                }
            }
            return;
        }

        // the rest perform type conversions and use the scratch buffer
        pimpl.value()->scratch.resize(length * sample_size);
        grab(offset*sample_size, (char*)pimpl.value()->scratch.data(), length*sample_size);

        if (pimpl.value()->meta.format == "SB") {
            int8_t* ptr = (int8_t*)pimpl.value()->scratch.data();
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl.value()->meta.format == "SI") {
            int16_t* ptr = (int16_t*)pimpl.value()->scratch.data();
            if (pimpl.value()->is_swapped) {
                for (int64 ii = 0; ii<length; ii++) {
                    byteswap2(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl.value()->meta.format == "SL") {
            int32_t* ptr = (int32_t*)pimpl.value()->scratch.data();
            if (pimpl.value()->is_swapped) {
                for (int64 ii = 0; ii<length; ii++) {
                    byteswap4(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl.value()->meta.format == "SF") {
            float* ptr = (float*)pimpl.value()->scratch.data();
            if (pimpl.value()->is_swapped) {
                for (int64 ii = 0; ii<length; ii++) {
                    byteswap4(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl.value()->meta.format == "SD") {
            double* ptr = (double*)pimpl.value()->scratch.data();
            if (pimpl.value()->is_swapped) {
                for (int64 ii = 0; ii<length; ii++) {
                    byteswap8(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl.value()->meta.format == "CB") {
            int8_t* ptr = (int8_t*)pimpl.value()->scratch.data();
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[2*ii + 0], ptr[2*ii + 1]);
            }

        } else if (pimpl.value()->meta.format == "CI") {
            int16_t* ptr = (int16_t*)pimpl.value()->scratch.data();
            if (pimpl.value()->is_swapped) {
                for (int64 ii = 0; ii<2*length; ii++) {
                    byteswap2(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[2*ii + 0], ptr[2*ii + 1]);
            }

        } else if (pimpl.value()->meta.format == "CL") {
            int32_t* ptr = (int32_t*)pimpl.value()->scratch.data();
            if (pimpl.value()->is_swapped) {
                for (int64 ii = 0; ii<2*length; ii++) {
                    byteswap4(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[2*ii + 0], ptr[2*ii + 1]);
            }

        } else if (pimpl.value()->meta.format == "CD") {
            double* ptr = (double*)pimpl.value()->scratch.data();
            if (pimpl.value()->is_swapped) {
                for (int64 ii = 0; ii<2*length; ii++) {
                    byteswap8(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[2*ii + 0], ptr[2*ii + 1]);
            }

        } else {
            check(false, "unsuported conversion '%s' to 'CF'", pimpl.value()->meta.format.data());
        }
    }

    const void* bluereader::mmap() {
        check(pimpl.valid(), "need an opened file");
        return pimpl.value()->data_offset + (char*)pimpl.value()->file.mmap();
    }

    inline int64 bluereader::byte_offset(int64 sample) {
        check(pimpl.valid(), "need an opened file");
        return pimpl.value()->data_offset + sample*pimpl.value()->meta.itemsize;
    }

    //}}}
    //{{{ bluewriter
    struct bluewriter {
        inline ~bluewriter();
        //inline bluewriter() = default;
        //inline bluewriter(const bluewriter&) = default;
        inline bluewriter(const string& path);

        //inline bluewriter& operator =(const bluewriter&) = default;

        inline bluemeta* operator ->();
        inline const bluemeta* operator ->() const;
        inline bluemeta& operator *();
        inline const bluemeta& operator *() const;

        inline void write(const void* ptr, int64 len);

        private:
            inline void setup();

            struct implementation {
                internal::rawfile file;
                bluemeta meta;
                int64 bytes_written;
                int64 total_length;
            };
            shared<implementation*> pimpl;
    };

    bluewriter::~bluewriter() {
        if (pimpl.valid()) check(
            pimpl.value()->bytes_written == pimpl.value()->total_length,
            "need to write the total number of bytes (%lld/%lld)",
            pimpl.value()->bytes_written, pimpl.value()->total_length
        );
    }

    bluewriter::bluewriter(const string& path) {
        using namespace internal;

        int fd = path == "-" ? dup(1) : open(
            path.data(), O_WRONLY | O_TRUNC | O_CREAT, 0666
        );
        check(fd >= 0, "opening '%s' for writing", path.data());
        rawfile file(fd);

        shared<implementation*> tmp(new implementation());
        swap(pimpl, tmp);
        swap(pimpl.value()->file, file);
        pimpl.value()->meta.type     = 1000;
        pimpl.value()->meta.format   = "CF";
        pimpl.value()->meta.xcount   = 0;
        pimpl.value()->meta.ycount   = 1;
        pimpl.value()->meta.xstart   = 0.0;
        pimpl.value()->meta.ystart   = 0.0;
        pimpl.value()->meta.xdelta   = 1.0;
        pimpl.value()->meta.ydelta   = 1.0;
        pimpl.value()->meta.xunits   = 0;
        pimpl.value()->meta.yunits   = 0;
        // sentinel to indicate that we need
        // to write the header and keywords
        pimpl.value()->bytes_written = -1;
    }

    bluemeta* bluewriter::operator ->() {
        check(pimpl.valid(), "need an opened file");
        return &pimpl.value()->meta;
    }

    const bluemeta* bluewriter::operator ->() const {
        check(pimpl.valid(), "need an opened file");
        return &pimpl.value()->meta;
    }

    bluemeta& bluewriter::operator *() {
        check(pimpl.valid(), "need an opened file");
        return pimpl.value()->meta;
    }

    const bluemeta& bluewriter::operator *() const {
        check(pimpl.valid(), "need an opened file");
        return pimpl.value()->meta;
    }

    void bluewriter::setup() {
        using namespace internal;
        xmheader hdr = init_header(pimpl.value()->meta);

        check(pimpl.value()->file.write(&hdr, sizeof(xmheader)), "writing header");
        write_kwds(&pimpl.value()->file, pimpl.value()->meta.kwds);
        /*
        if (padding_bytes) {
            static char scratch[512];
            check(pimpl.value()->file.write(scratch, padding_bytes), "writing padding");
        }
        */

        pimpl.value()->bytes_written = 0;
        pimpl.value()->total_length = (uint64_t)hdr.data_size;
    }

    void bluewriter::write(const void* ptr, int64 len) {
        check(pimpl.valid(), "need an opened file");

        if (pimpl.value()->bytes_written == -1) setup();

        check(
            pimpl.value()->bytes_written + len <= pimpl.value()->total_length,
            "can't write %lld bytes of data %lld/%lld", len,
            pimpl.value()->bytes_written, pimpl.value()->total_length
        );
        check(pimpl.value()->file.write(ptr, len), "writing data");
        pimpl.value()->bytes_written += len;
    }

    //}}}
    //{{{ dump2000 dump1000
    static inline void dump1000(
        const string& path, const string& format, const void* ptr, timecode time,
        double xstart, double xdelta, int xunits, int64 xcount
    ) {
        using namespace internal;
        int64 bytesize = xmbytesize(format);
        bluewriter output(path);
        output->time = time;
        output->type = 1000;
        output->format = format;
        output->xstart = xstart;
        output->xdelta = xdelta;
        output->xunits = xunits;
        output->xcount = xcount;
        output.write(ptr, xcount*bytesize);
    }

    static inline void dump2000(
        const string& path, const string& format, const void* ptr, timecode time,
        double xstart, double xdelta, int xunits, int64 xcount,
        double ystart, double ydelta, int yunits, int64 ycount
    ) {
        using namespace internal;
        int64 bytesize = xmbytesize(format);
        bluewriter output(path);
        output->time = time;
        output->type = 2000;
        output->format = format;
        output->xstart = xstart;
        output->xdelta = xdelta;
        output->xunits = xunits;
        output->xcount = xcount;
        output->ystart = ystart;
        output->ydelta = ydelta;
        output->yunits = yunits;
        output->ycount = ycount;
        output.write(ptr, xcount*ycount*bytesize);
    }

    //}}}
    //{{{ bluedetach
    static inline void bluedetach(
        const string& datapath, int64 byteoffset,
        const string& bluepath, const bluemeta& meta
    ) {
        string detfile = bluepath + ".det";
        string tmpfile = bluepath + ".tmp";

        using namespace internal;
        xmheader hdr = init_header(meta);
        hdr.detached = 1;
        hdr.data_start = byteoffset;

        int fd = open(tmpfile.data(), O_WRONLY | O_TRUNC | O_CREAT, 0660);
        check(fd >= 0, "opening '%s' for writing", tmpfile.data());
        rawfile file(fd);
        check(file.write(&hdr, sizeof(xmheader)), "writing header");
        write_kwds(&file, meta.kwds);

        unlink(detfile.data()); // delete if it exists
        check(symlink(datapath.data(), detfile.data()) == 0, "symlink '%s'", detfile.data());
    }
    //}}}

}

#endif // XM_BLUEFILES_H_
