#ifndef XM_EPHEMERIS_H_
#define XM_EPHEMERIS_H_ 1

namespace xm {

    //{{{ ephemeris

    struct ephemeris {

        //~ephemeris() = default;
        ephemeris() {}; // = default;
        //ephemeris(const ephemeris&) = default;
        inline ephemeris(const statevec&);
        inline ephemeris(const geodetic&);
        inline ephemeris(const string& path);
        //ephemeris& operator =(const ephemeris&) = default;

        inline statevec lookup(timecode tc) const;

        // timestates must be sorted
        vector<timestate> storage;
    };

    ephemeris::ephemeris(const statevec& sv) {
        storage.resize(1);
        storage[0] = (timestate){ timecode(), sv };
    }

    ephemeris::ephemeris(const geodetic& lla) {
        storage.resize(1);
        storage[0] = (timestate){
            timecode(), (statevec){
                lla_to_ecf(lla), cartesian(), cartesian()
            }
        };
    }

    ephemeris::ephemeris(const string& path) {
        using namespace internal;
        bluereader input(path);
        check(input->type/1000 == 5, "expected a Type 5000 file");

        //print(format(input->time, 12), input->xstart, input->xdelta);

        if (input->itemsize == 72) {
            check(input->fields.size() == 3, "expected POS VEL ACC");
            check(input->fields[0].format == "VD", "expected VD POS");
            check(input->fields[1].format == "VD", "expected VD VEL");
            check(input->fields[2].format == "VD", "expected VD ACC");
            check(memcmp(input->fields[0].name.data(), "POS", 3) == 0, "expected POS");
            check(memcmp(input->fields[1].name.data(), "VEL", 3) == 0, "expected VEL");
            check(memcmp(input->fields[2].name.data(), "ACC", 3) == 0, "expected ACC");
            check(input->xdelta > 0, "positive xdelta");

            storage.resize(input->xcount);
            for (int64 ii = 0; ii<input->xcount; ii++) {
                statevec sv;
                input.grab(ii*sizeof(statevec), &sv, sizeof(statevec));
                if (input.is_swapped()) {
                    byteswap8(&sv.pos.x); byteswap8(&sv.pos.y); byteswap8(&sv.pos.z);
                    byteswap8(&sv.vel.x); byteswap8(&sv.vel.y); byteswap8(&sv.vel.z);
                    byteswap8(&sv.acc.x); byteswap8(&sv.acc.y); byteswap8(&sv.acc.z);
                }
                timecode tc = input->time + (input->xstart + ii*input->xdelta);
                storage[ii] = (timestate){ tc, sv };
            }

            check(storage.size() >= 2, "need at least two statevectors");

            return;
        }

        if (input->itemsize == 80) {
            check(input->fields.size() == 4, "expected POS VEL ACC TIME");
            check(input->fields[0].format == "VD", "expected VD POS");
            check(input->fields[1].format == "VD", "expected VD VEL");
            check(input->fields[2].format == "VD", "expected VD ACC");
            check(input->fields[3].format == "SD", "expected SD TIME");
            check(memcmp(input->fields[0].name.data(), "POS", 3) == 0, "expected POS");
            check(memcmp(input->fields[1].name.data(), "VEL", 3) == 0, "expected VEL");
            check(memcmp(input->fields[2].name.data(), "ACC", 3) == 0, "expected ACC");
            check(input->fields[3].name == "TIME", "expected TIME");

            storage.resize(input->xcount);
            for (int64 ii = 0; ii<input->xcount; ii++) {
                struct record { statevec sv; double tc; } rec;
                input.grab(ii*sizeof(record), &rec, sizeof(record));
                if (input.is_swapped()) {
                    byteswap8(&rec.sv.pos.x); byteswap8(&rec.sv.pos.y); byteswap8(&rec.sv.pos.z);
                    byteswap8(&rec.sv.vel.x); byteswap8(&rec.sv.vel.y); byteswap8(&rec.sv.vel.z);
                    byteswap8(&rec.sv.acc.x); byteswap8(&rec.sv.acc.y); byteswap8(&rec.sv.acc.z);
                    byteswap8(&rec.tc);
                }
                // The double precision time field only has about microsecond precision,
                // so we subtract the file time, round it, and then re-add the file time.
                double dt = 1e-6*round(1e+6*(normalize((timecode){ 0, rec.tc }) - input->time));
                storage[ii] = (timestate){ input->time + dt, rec.sv };
                /*
                if (ii%99 == 0) print(format(input->time + dt));
                if (ii > 0 && storage[ii - 0].tc - storage[ii - 1].tc > 1.1*input->xdelta) {
                   print(">>>");
                   print("===", format(storage[ii-1].tc));
                   print("===", format(storage[ii-0].tc));
                   print("<<<");

                }
                */
            }

            check(storage.size() >= 2, "need at least two statevectors");
            // they should be sorted, but this promises they will be
            introsort(storage.data(), storage.size());

            return;
        }

        check(false, "unsupported ephemeris format");
    }

    statevec ephemeris::lookup(timecode tc) const {
        check(storage.size() > 0, "must use an initialized ephemeris");
        if (storage.size() == 1) return storage[0].sv;

        check(!(tc - storage[0].tc < 0), "timecode not before ephemeris");
        check(!(storage[storage.size() - 1].tc - tc < 0), "timecode not after epehemeris");

        // using secant search to find the correct interval
        int64 lo = 0;
        int64 hi = storage.size() - 1;

        int64 iteration = 0;
        while (lo < hi - 1) {
            // three out of four iterations use secant search and the
            // others use standard bisection to get log(n) worst case
            int64 mid = ++iteration%4 ? (
                lo + llrint(
                    (hi - lo)*(tc - storage[lo].tc) /
                    (storage[hi].tc - storage[lo].tc)
                )
            ) : (lo + hi)/2;
            if (mid <= lo) mid = lo + 1;
            if (mid >= hi) mid = hi - 1;

            if (storage[mid].tc < tc) { lo = mid; continue; }
            if (tc < storage[mid].tc) { hi = mid; continue; }
            return storage[mid].sv; // exact match
        }
        //check(!(tc < storage[lo].tc), "sanity");
        //check(!(storage[hi].tc < tc), "sanity");
        //print("final:", lo, hi, "iterations:", iteration);

        return interp(storage[lo], storage[hi], tc);
        //return storage[lo].sv; // XXX: debugging
    }

    //}}}
    //{{{ ephcache
    namespace internal {
        struct vehday {
            int64 veh;
            int64 day;
            bool operator ==(const vehday& other) const {
                return veh == other.veh && day == other.day;
            }
        };

        static inline size_t hash(const vehday& md) {
            return xm::hash(md.veh) ^ 5*xm::hash(md.day);
        }
    }

    static inline string ephpath(int64 veh, datetime dt) {
        const char* ephdir = getenv("EPHEMERIS_DIR");
        check(ephdir != 0, "EPHEMERIS_DIR must be set");

        list<string> paths;
        // We put the "u" versions first because they are supposed to be "updated" (newer and better).
        paths.append(format("%s/sv%d_%04d%02d%02du.prm", ephdir, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/sv%d_%04d%02d%02du.tmp", ephdir, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/sv%d_%04d%02d%02d.prm", ephdir, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/sv%d_%04d%02d%02d.tmp", ephdir, (int)veh, dt.year, dt.month, dt.day));
        // These are alternative paths - they make more sense, but are not as common, so we put them last
        paths.append(format("%s/%04d/%02d/sv%d_%04d%02d%02du.prm", ephdir, dt.year, dt.month, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/%04d/%02d/sv%d_%04d%02d%02du.tmp", ephdir, dt.year, dt.month, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/%04d/%02d/sv%d_%04d%02d%02d.prm", ephdir, dt.year, dt.month, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/%04d/%02d/sv%d_%04d%02d%02d.tmp", ephdir, dt.year, dt.month, (int)veh, dt.year, dt.month, dt.day));

        for (int64 ii = 0; ii<paths.size(); ii++) {
            const string& path = paths[ii];
            struct stat buf;
            if (stat(path.data(), &buf) == 0) return path;
        }

        check(false, "unable to find ephemeris for %d on %04d-%02d-%02d", (int)veh, dt.year, dt.month, dt.day);
        return "";
    }



    struct ephcache {

        //inline ~ephcache() = default;
        //inline ephcache(const ephcache&) = default;
        //inline ephcache& operator =(const ephcache&) = default;

        inline ephcache(int64 maxsize=64) : maxsize(maxsize), counter(0) {};

        inline ephemeris& geteph(int64 veh, timecode tc);
        inline statevec getsv(int64 veh, timecode tc);

        private:

            int64 maxsize;
            int64 counter;
            struct counteph {
                int64 count;
                ephemeris eph;
            };

            dict<internal::vehday, counteph> cache;
    };

    ephemeris& ephcache::geteph(int64 veh, timecode tc) {
        using namespace internal;
        tc = normalize(tc);
        vehday md = (vehday){ veh, tc.whole/86400 };
        if (!cache.haskey(md)) {
            if ((int64)cache.size() >= maxsize) {
                // the oldest one will have the lowest count
                vehday oldest = (vehday){-1,-1};
                int64 lowest = INT64_MAX;
                for (int64 ii = 0; ii<cache.size(); ii++) {
                    if (cache.val(ii).count < lowest) {
                        oldest = cache.key(ii);
                        lowest = cache.val(ii).count;
                    }
                }
                cache.remove(oldest);
            }
            datetime dt = getdate(tc);

            cache[md] = (counteph){ counter, ephemeris(ephpath(veh, dt)) };
        }

        counteph& ce = cache[md];
        ce.count = ++counter;
        return ce.eph;
    }

    statevec ephcache::getsv(int64 veh, timecode tc) {
        return geteph(veh, tc).lookup(tc);
    }

    //}}}

}

#endif // XM_EPHEMERIS_H_
