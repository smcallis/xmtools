#ifndef XM_TIMECODE_H_
#define XM_TIMECODE_H_ 1

#include <math.h>

#include "basics.h"
#include "string.h"

namespace xm {

    struct timecode {
        // J1950 Epoch
        int64 whole;
        double fract;

        static inline timecode parse(const string& ss);
    };

    // returns "seconds since midnight" before the timecode
    static inline double ssm(timecode tc) {
        return fmod(tc.whole%86400 + tc.fract, 86400.0);
    }

    struct datetime {
        int32_t year, month, day;
        int32_t hour, min;
        double secs;
    };

    static inline timecode normalize(timecode tc) {
        double lower = floor(tc.fract);
        tc.whole += (int64)lower;
        tc.fract -= lower;
        return tc;
    }

    static inline timecode gettime(datetime dt) {
        check(1950 <= dt.year  && dt.year  <  2100, "year in bounds: %d", dt.year);
        check(1    <= dt.month && dt.month <=   12, "month in bounds: %d", dt.month);
        check(0.0  <= dt.secs  && dt.secs  <= 60.0, "seconds in bounds: %lf", dt.secs);
        // XXX: could check day, hour, min as well

        static const int32_t moffset[] = {
            0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
        };

        int64 whole = (dt.year - 1950) * 365;  // add days in first
        whole += (dt.year - 1949)/4;     // leap days since epoch
        whole += moffset[dt.month - 1];  // days in current year

        if ((dt.year % 4 == 0) && (dt.month > 2)) {
            whole++;                  // possible current leap day
        }

        whole += dt.day-1;  whole *= 24; // day of the month, now at curent day
        whole += dt.hour;   whole *= 60; // hours
        whole += dt.min;    whole *= 60; // minutes
        return normalize((timecode){ whole, dt.secs});
    }

    static inline datetime getdate(timecode tc, int32_t places = -1) {
        tc = normalize(tc);
        if (places >= 0) {
            double scale = pow(10, places);
            tc.fract = round(tc.fract*scale)/scale;
            if (tc.fract == 1.0) {
                tc.whole += 1;
                tc.fract = 0.0;
            }
        }

        static const int32_t mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        int32_t sec  = tc.whole % 60; tc.whole /= 60;
        int32_t min  = tc.whole % 60; tc.whole /= 60;
        int32_t hour = tc.whole % 24; tc.whole /= 24;

        // tc.whole is now days since 1950, switch to 1948 to make math easier
        tc.whole += 365+366;
        int32_t year = 1948 + tc.whole/(365*4+1)*4; tc.whole %= (365*4+1);

        // handle remainder of years
        int32_t curleap=1;
        if (tc.whole > 365) {
            year += 1;            tc.whole -= 366;
            year += tc.whole/365; tc.whole %= 365;
            curleap=0;
        }

        size_t ii;
        for (ii=0; ii < 12; ii++) {
            int32_t days = mdays[ii] + (((ii == 1) && curleap) ? 1 : 0);
            if (tc.whole < days) {
                break;
            }
            tc.whole -= days;
        }

        int32_t day   = tc.whole+1;
        int32_t month = ii + 1;
        return (datetime){ year, month, day, hour, min, sec + tc.fract };
    }

    static inline timecode operator +(timecode tc, double dd) {
        return normalize((timecode){ tc.whole, tc.fract + dd});
    }

    static inline timecode operator +(double dd, timecode tc) {
        return normalize((timecode){ tc.whole, tc.fract + dd});
    }

    static inline timecode operator -(timecode tc, double dd) {
        return normalize((timecode){ tc.whole, tc.fract - dd});
    }

    static inline double operator -(timecode aa, timecode bb) {
        return (double)(aa.whole - bb.whole) + (aa.fract - bb.fract);
    }

    static inline timecode& operator +=(timecode& tc, double dd) {
        return tc = tc + dd;
    }

    static inline timecode& operator -=(timecode& tc, double dd) {
        return tc = tc - dd;
    }

    static inline bool operator <(timecode aa, timecode bb) {
        return (aa - bb) < 0;
    }

    static inline bool operator >(timecode aa, timecode bb) {
        return (aa - bb) > 0;
    }

    static inline bool operator <=(timecode aa, timecode bb) {
        return (aa - bb) <= 0;
    }

    static inline bool operator >=(timecode aa, timecode bb) {
        return (aa - bb) >= 0;
    }

    timecode timecode::parse(const string& ss) {
        datetime dt;
        int res = sscanf(
            // XXX: This will silently ignore any timezone at the end.
            // It should work with Z for UTC or none (defaulting to UTC).
            ss.data(), " %u%*[/:-]%u%*[/:-]%u%*[/:T -]%u:%u:%lf ",
            &dt.year, &dt.month, &dt.day,
            &dt.hour, &dt.min, &dt.secs
        );
        check(res == 6, "expected valid timecode string '%s'", ss.data());

        return gettime(dt);
    }

    static inline string format(timecode tc, int32_t places=12) {
        datetime dt = getdate(tc, places);
        if (places <= 0) {
            return xm::format(
                "%04d-%02d-%02dT%02d:%02d:%02.0f",
                dt.year, dt.month, dt.day, dt.hour, dt.min, dt.secs
            );
        }
        return xm::format(
            "%04d-%02d-%02dT%02d:%02d:%0*.*lf",
            dt.year, dt.month, dt.day, dt.hour, dt.min,
            places + 3, places, dt.secs
        );
    }
}

#endif // XM_TIMECODE_H_

