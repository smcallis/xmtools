#ifndef XM_DTED_H_
#define XM_DTED_H_ 1

#include <stdint.h>
#include <string.h>

#include "rawfile.h"

namespace xm {

    //{{{ dtedtile
    namespace internal {
        static int16_t dtedsignmag(uint16_t xx) {
            xx = be16toh(xx);
            int16_t signbit = xx&0x8000;
            int16_t magbits = xx&0x7FFF;
            if (signbit) magbits *= -1;
            return magbits;
        }

        static int16_t dtedgetbits(const char* data, int64 off, int64 len) {
            int64 byte = off/8;
            int64 bit = off%8;
            int32_t result = 0;
            memcpy(&result, data + byte, sizeof(int32_t));
            result = be32toh(result);
            result <<= bit;
            result >>= 32 - len;
            return result;
        }
    }

    struct dtedtile {
        inline dtedtile(const string& path);

        //inline ~dtedtile() = default;
        inline dtedtile() : lats(0), lons(0) {}
        //inline dtedtile(const dtedtile&) = default;
        //inline dtedtile& operator =(const dtedtile&) = default;

        inline double lookup(double fractlat, double fractlon);
        inline bool isvalid();

        private:
            // data is stored column major, starting from the lower
            // left through each latitude, then increasing longitude.
            int64 lats, lons;
            vector<int16_t> data;
    };

    dtedtile::dtedtile(const string& path) {
        using namespace internal;

        // An empty path is used to indicate the absence of a tile.
        // This is used by dtedcache to indicate it should use MSL lookup.
        if (path == "") return;

        int fd = open(path.data(), O_RDONLY);
        check(fd >= 0, "opening '%s' for reading", path.data());
        rawfile file(fd);
        const char* ptr = (const char*)file.mmap();

        if (memcmp(ptr, "UHL", 3) == 0) {
            // uncompressed data
            char temp[5] = {};
            memcpy(temp, ptr + 47, 4);
            lons = strtol(temp, 0, 10);
            memcpy(temp, ptr + 51, 4);
            lats = strtol(temp, 0, 10);
            data.resize(lats*lons);
            for (int64 ii = 0; ii<lons; ii++) {
                const int64 skip = 80 + 648 + 2700 + 8;
                const int64 offset = skip + ii*(12 + lats*sizeof(int16_t));
                memcpy(&data[ii*lats], ptr + offset, lats*sizeof(int16_t));
                for (int64 jj = 0; jj<lats; jj++) {
                    data[ii*lats + jj] = dtedsignmag(data[ii*lats + jj]);
                }
            }
            return;
        }

        if (memcmp(ptr, "DTC", 3) == 0) {
            // compressed data
            char temp[5] = {};
            memcpy(temp, ptr + 51, 4);
            lons = strtol(temp, 0, 10);
            memcpy(temp, ptr + 55, 4);
            lats = strtol(temp, 0, 10);
            data.resize(lats*lons);

            const uint32_t* offset = (const uint32_t*)(ptr + 80 + 648 + 23);

            const int64 blockstart = 80 + 648 + 23 + 400;
            for (int64 ii = 0; ii<10; ii++) {
                for (int64 jj = 0; jj<10; jj++) {

                    const char* blockptr = ptr + offset[ii*10 + jj] + blockstart + 4;
                    const int64 blocklats = lats / 10 + (ii == 9 ? lats%10 : 0);
                    const int64 blocklons = lons / 10 + (jj == 9 ? lons%10 : 0);

                    int8_t nchunks = *(int8_t*)blockptr;
                    blockptr += 1;

                    for (int64 kk = 0; kk<blocklons; kk++) {
                        int16_t* line = &data[jj*lats*(lons/10) + ii*(lats/10) + kk*lats];

                        int64 total = 0;

                        for (int64 nn = 0; nn<nchunks; nn++) {
                            int64 samples = blocklats/nchunks + (nn == nchunks-1 ? blocklats%nchunks : 0);
                            uint8_t nbits = *(uint8_t*)(blockptr);
                            blockptr += 1;
                            check(nbits == 0xFF || nbits <= 16, "in bounds");

                            if (nbits == 0xFF) {
                                memcpy(line, blockptr, samples*2);
                                blockptr += samples*2;
                            } else {
                                int16_t accum = *(int16_t*)(blockptr);
                                blockptr += 2;
                                int64 bitoff = 0;
                                for (int64 ss = 0; ss<samples; ss++) {
                                    line[ss] = accum;
                                    if (ss != samples-1 && nbits > 0) {
                                        accum += dtedgetbits(blockptr, bitoff, nbits);
                                        bitoff += nbits;
                                    }
                                }
                                blockptr += (bitoff + 7)/8;
                            }
                            line += samples;
                            total += samples;
                        }

                        check(total == blocklats, "sanity %lld %lld", total, blocklats);
                    }
                }
            }

            return;
        }

        check(false, "unrecognized DTED file format");
    }

    double dtedtile::lookup(double fractlat, double fractlon) {
        check(fractlat >= 0.0, "fract lat in bounds: %lf", fractlat);
        check(fractlon >= 0.0, "fract lon in bounds: %lf", fractlon);
        check(fractlat <  1.0, "fract lat in bounds: %lf", fractlat);
        check(fractlon <  1.0, "fract lon in bounds: %lf", fractlon);

        double floatrow = (lats - 1)*fractlat;
        double floatcol = (lons - 1)*fractlon;
        int64 wholerow = floor(floatrow);
        int64 wholecol = floor(floatcol);
        double fractrow = floatrow - wholerow;
        double fractcol = floatcol - wholecol;

        //check(wholerow >= 0, "in bounds wholerow %lld < 0", wholerow);
        //check(wholecol >= 0, "in bounds wholecol %lld < 0", wholecol);
        //check(wholerow < lats - 1, "in bounds wholerow %lld < %lld", wholerow, lats - 1);
        //check(wholecol < lons - 1, "in bounds wholecol %lld < %lld", wholecol, lons - 1);

        double bl = data[(wholerow + 0) + lats*(wholecol + 0)];
        double br = data[(wholerow + 0) + lats*(wholecol + 1)];
        double tl = data[(wholerow + 1) + lats*(wholecol + 0)];
        double tr = data[(wholerow + 1) + lats*(wholecol + 1)];

        return (
            bl*(1.0 - fractrow)*(1.0 - fractcol) +
            br*(1.0 - fractrow)*(0.0 + fractcol) +
            tl*(0.0 + fractrow)*(1.0 - fractcol) +
            tr*(0.0 + fractrow)*(0.0 + fractcol)
        );
    }

    bool dtedtile::isvalid() {
        return data.size() > 0;
    }

    //}}}
    //{{{ dtedcache
    namespace internal {
        struct latlon {
            int32_t lat;
            int32_t lon;
            bool operator ==(const latlon& other) const {
                return lat == other.lat && lon == other.lon;
            }
        };

        static inline size_t hash(const latlon& ll) {
            return xm::hash(ll.lat) ^ 5*xm::hash(ll.lon);
        }
    }

    struct dtedcache {
        //inline ~dtedcache() = default;
        //inline dtedcache(const dtedcache&) = default;
        //inline dtedcache& operator =(const dtedcache&) = default;
        inline dtedcache(int64 maxtiles=64);

        inline double lookup(double lat, double lon);

        private:
            inline string trypath(int32_t lat, int32_t lon);
            int64 maxsize;
            int64 counter;
            struct countdted {
                int64 count;
                dtedtile tile;
            };
            dict<internal::latlon, countdted> cache;

            inline double getmsl(double lat, double lon);

            // MSL data is stored row-major starting from
            // 0-lon at the north pole, moving southward per row
            enum { MSL_LATS = 180*4 + 1, MSL_LONS = 360*4 + 1 };
            vector<float> msl;
    };

    dtedcache::dtedcache(int64 maxtiles) : maxsize(maxtiles), counter(0) {
        using namespace internal;

        // This should be: /full/path/to/WW15MGH.BIN
        char* dtedmsl = getenv("DTED_MSL");
        if (!dtedmsl) return;

        int fd = open(dtedmsl, O_RDONLY);
        check(fd >= 0, "opening '%s' for reading", dtedmsl);
        rawfile file(fd);
        check(
            file.size() >= int64((MSL_LATS*MSL_LONS + 6)*sizeof(float)),
            "expected size for MSL"
        );
        const float* ptr = (const float*)file.mmap();
        msl.resize(MSL_LATS*MSL_LONS + 6);
        memcpy(msl.data(), ptr, (MSL_LATS*MSL_LONS + 6)*sizeof(float));

#if __BYTE_ORDER == __BIG_ENDIAN
        // XXX: reimplement this without C++11
        for (auto ii = msl.begin(); ii != msl.end(); ++ii) {
            byteswap4(&*ii);
        }
#endif
        check(msl[0] == -90, "expected 0");
        check(msl[1] == +90, "expected 1");
        check(msl[2] ==   0, "expected 2");
        check(msl[3] == 360, "expected 3");
        check(msl[4] == .25, "expected 4");
        check(msl[5] == .25, "expected 5");
    }

    string dtedcache::trypath(int32_t lat, int32_t lon) {
        const char* dteddir = getenv("DTED_DIR");
        check(dteddir != 0, "DTED_DIR must be set");

        char ns = 'n';
        char ew = 'e';
        if (lat < 0) {
            ns = 's';
            lat *= -1;
        }
        if (lon < 0) {
            ew = 'w';
            lon *= -1;
        }

        list<string> paths;
        paths.append(format("%s/%c%03d/%c%02d.cdt2", dteddir, ew, lon, ns, lat));
        paths.append(format("%s/%c%03d/%c%02d.cdt1", dteddir, ew, lon, ns, lat));
        paths.append(format("%s/%c%03d/%c%02d.dt0",  dteddir, ew, lon, ns, lat));

        for (int64 ii = 0; ii<paths.size(); ii++) {
            const string& path = paths[ii];
            struct stat buf;
            if (stat(path.data(), &buf) == 0) {
                return path;
            }
        }

        return "";
    }

    inline double dtedcache::lookup(double lat, double lon) {
        using namespace internal;

        int32_t wholelat = (int32_t)floor(lat);
        int32_t wholelon = (int32_t)floor(lon);
        double fractlat = lat - wholelat;
        double fractlon = lon - wholelon;
        latlon ll = { wholelat, wholelon };

        if (!cache.haskey(ll)) {
            if ((int64)cache.size() >= maxsize) {
                // the oldest will have the lowest count
                latlon oldest = (latlon){ -1, -1 };
                int64 lowest = INT64_MAX;
                for (int64 ii = 0; ii<cache.size(); ii++) {
                    if (cache.val(ii).count < lowest) {
                        oldest = cache.key(ii);
                        lowest = cache.val(ii).count;
                    }
                }
                cache.remove(oldest);
            }
            cache[ll] = (countdted){ counter, dtedtile(trypath(wholelat, wholelon)) };
        }

        countdted& dt = cache[ll];
        dt.count = ++counter;
        if (dt.tile.isvalid()) {
            return dt.tile.lookup(fractlat, fractlon);
        }

        return getmsl(lat, lon);
    }

    double dtedcache::getmsl(double lat, double lon) {
        if (msl.size() == 0) return 0.0;
        if (lat >= +90) return msl[0];
        if (lat <= -90) return msl[MSL_LATS*MSL_LONS - 1];

        double rowcoord = 90.0 - lat;
        double colcoord = fmod(lon + 360.0, 360.0);
        double rowfloat = rowcoord*((MSL_LATS - 1)/180.0);
        double colfloat = colcoord*((MSL_LONS - 1)/360.0);
        int64 rowwhole = floor(rowfloat);
        int64 colwhole = floor(colfloat);
        check(rowwhole >= 0, "lat in bounds: %lf >= 0", lat);
        check(colwhole >= 0, "lon in bounds: %lf >= 0", lon);
        check(rowwhole < MSL_LATS - 1, "lat in bounds: %lf (%lld)", lat, rowwhole);
        check(colwhole < MSL_LONS - 1, "lon in bounds: %lf (%lld)", lon, colwhole);
        double rowfract = rowfloat - rowwhole;
        double colfract = colfloat - colwhole;

        float tl = msl[(rowwhole + 0)*MSL_LONS + (colwhole + 0) + 6];
        float tr = msl[(rowwhole + 0)*MSL_LONS + (colwhole + 1) + 6];
        float bl = msl[(rowwhole + 1)*MSL_LONS + (colwhole + 0) + 6];
        float br = msl[(rowwhole + 1)*MSL_LONS + (colwhole + 1) + 6];

        return (
            tl*(1.0 - rowfract)*(1.0 - colfract) +
            tr*(1.0 - rowfract)*(0.0 + colfract) +
            bl*(0.0 + rowfract)*(1.0 - colfract) +
            br*(0.0 + rowfract)*(0.0 + colfract)
        );
    }


    //}}}


}

#endif // XM_DTED_H_

