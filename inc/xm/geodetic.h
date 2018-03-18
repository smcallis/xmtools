#ifndef XM_GEODETIC_H_
#define XM_GEODETIC_H_ 1

#include "cartesian.h"

namespace xm {

    //{{{ geodetic
    struct geodetic {
        double lat, lon, alt;
    };

    namespace internal {
        static inline double curve_pvert(double lat) {
            const double EARTH_SMAJOR  = 6378137.0;     // meters, semi-major axis
            const double EARTH_ECC_SQ  = .006694379990141316996137;
            return EARTH_SMAJOR/sqrt(1.0 - EARTH_ECC_SQ*pow(sin(lat),2.0));
        }
    }

    static inline cartesian lla_to_ecf(geodetic lla) {
        using namespace internal;
        const double RAD_PER_DEG   = .017453292519943295769237;
        const double EARTH_ECC_SQ  = .006694379990141316996137;
        lla.lat *= RAD_PER_DEG; lla.lon *= RAD_PER_DEG;

        double clat  = cos(lla.lat), slat = sin(lla.lat);
        double clon  = cos(lla.lon), slon = sin(lla.lon);

        double N     = curve_pvert(lla.lat);
        double G     = (1.0-EARTH_ECC_SQ)*N;

        return (cartesian){
            (N+lla.alt)*clat*clon,
            (N+lla.alt)*clat*slon,
            (G+lla.alt)*slat
        };
    }

    static inline geodetic ecf_to_lla(cartesian xyz) {
        using namespace internal;
        const double EARTH_SMINOR  = 6356752.314245179497564;  // meters, semi-minor axis
        const double EARTH_ECC_SQ  = .006694379990141316996137;
        double x = xyz.x, y = xyz.y, z = xyz.z;

        double p   = hypot(x,y); // Equatorial projection
        double lon = atan2(y,x); // Right ascension
        double lat = atan2(z,p); // Use geocentric latitude as initial guess
        double alt;

        int cnt = 0;
        double lat_old = lat - 10.0;
        double Rn;
        while ((fabs(lat - lat_old) > 1e-12) && (cnt++ < 10)) {
            lat_old = lat;

            Rn  = curve_pvert(lat);
            //alt = p/cosl(lat) - Rn;
            lat = atan2(z+Rn*EARTH_ECC_SQ*sin(lat), p);
        }

        // Near the poles geocentric and geodetic become the same
        if (fabs((fabs(lat) - M_PI/2.0)) < 1e-12) {
            alt = fabs(z) - EARTH_SMINOR;
        } else {
            alt = p/cos(lat) - curve_pvert(lat);
        }

        return (geodetic){ lat*180.0/M_PI, lon*180.0/M_PI, alt };
    }

    static inline transform ecf_to_enu(geodetic lla) {
        cartesian enu_center = lla_to_ecf(lla);

        double lon = M_PI/2.0 + M_PI*lla.lon/180.0; // rotate around z-axis first
        double lat = M_PI/2.0 - M_PI*lla.lat/180.0; // rotate around x-axis second

        transform lonrot = {
            {  cos(lon),  sin(lon),       0.0 },
            { -sin(lon),  cos(lon),       0.0 },
            {       0.0,       0.0,       1.0 },
            {       0.0,       0.0,       0.0 }
        };

        transform latrot = {
            {       1.0,        0.0,      0.0 },
            {       0.0,   cos(lat), sin(lat) },
            {       0.0,  -sin(lat), cos(lat) },
            {       0.0,        0.0,      0.0 }
        };

        return latrot * lonrot * translate(-enu_center);
    }

    static inline transform enu_to_ecf(geodetic lla) {
        cartesian enu_center = lla_to_ecf(lla);

        double lon = M_PI/2.0 + M_PI*lla.lon/180.0; // rotate around z-axis first
        double lat = M_PI/2.0 - M_PI*lla.lat/180.0; // rotate around x-axis second

        transform lonrot = {
            {  cos(lon),  -sin(lon),      0.0 },
            {  sin(lon),   cos(lon),      0.0 },
            {       0.0,        0.0,      1.0 },
            {       0.0,        0.0,      0.0 }
        };

        transform latrot = {
            {       1.0,        0.0,       0.0  },
            {       0.0,    cos(lat), -sin(lat) },
            {       0.0,    sin(lat),  cos(lat) },
            {       0.0,        0.0,       0.0  }
        };

        return translate(enu_center) * lonrot * latrot;
    }

    /* XXX: Verify or Delete this
    static inline cartesian lla_to_ecf_vel(geodetic pos, geodetic vel) {
        using namespace internal;
        const double RAD_PER_DEG   = .017453292519943295769237;
        const double EARTH_ECC_SQ  = .006694379990141316996137;
        pos.lat *= RAD_PER_DEG; pos.lon *= RAD_PER_DEG;
        vel.lat *= RAD_PER_DEG; vel.lon *= RAD_PER_DEG;

        double clat = cos(pos.lat); double slat = sin(pos.lat);
        double clon = cos(pos.lon); double slon = sin(pos.lon);
        double N    = curve_pvert(pos.lat);
        double Ndot = (N*EARTH_ECC_SQ*slat*clat / (1 - EARTH_ECC_SQ*slat*slat))*vel.lat;

        double xx = -(N + pos.alt)*clat*slon*vel.lon -
            (N + pos.alt)*slat*vel.lat*clon + (Ndot + vel.alt)*clat*clon;

        double yy = (N + pos.alt)*clat*clon*vel.lon -
            (N + pos.alt)*slat*vel.lat*slon + (Ndot + vel.alt)*clat*slon;

        double zz = (N*(1 - EARTH_ECC_SQ) + pos.alt)*clat*vel.lat +
            (Ndot*(1 - EARTH_ECC_SQ) + vel.alt)*slat;

        return (cartesian){xx, yy, zz};
    }
    */

    /* XXX: Verify or Delete this
    static inline cartesian lla_to_ecf_acc(geodetic pos, geodetic vel, geodetic acc) {
        using namespace internal;
        const double RAD_PER_DEG   = .017453292519943295769237;
        const double EARTH_ECC_SQ  = .006694379990141316996137;
        pos.lat *= RAD_PER_DEG; pos.lon *= RAD_PER_DEG;
        vel.lat *= RAD_PER_DEG; vel.lon *= RAD_PER_DEG;

        double clat  = cos(pos.lat); double slat = sin(pos.lat);
        double clon  = cos(pos.lon); double slon = sin(pos.lon);
        double denom = 1/(1 - EARTH_ECC_SQ*slat*slat);
        double N     = curve_pvert(pos.lat);
        double Nh = N + pos.alt;
        double Ndot  = (N*EARTH_ECC_SQ*slat*clat*denom)*vel.lat;
        double Nddot = (N*EARTH_ECC_SQ*denom) *
            (
                3*EARTH_ECC_SQ*slat*slat*clat*clat*vel.lat*vel.lat*denom -
                slat*slat*vel.lat*vel.lat + slat*clat*acc.lat + clat*clat*vel.lat*vel.lat
            );

        double xx =
            -Nh*clat*slon*acc.lon - Nh*clat*clon*vel.lon*vel.lon
            +Nh*slat*vel.lat*slon*vel.lon - (Ndot+vel.alt)*clat*slon*vel.lon
            +Nh*slat*vel.lat*slon*vel.lon - Nh*slat*acc.lat*clon
            -Nh*clat*vel.lat*vel.lat*clon - (Ndot+vel.alt)*slat*vel.lat*clon
            -(Ndot+vel.alt)*clat*slon*vel.lon - (Ndot+vel.alt)*slat*vel.lat*clon
            +(Nddot+acc.alt)*clat*clon;

        double yy =
            +Nh*clat*clon*acc.lon - Nh*clat*slon*vel.lon*vel.lon
            -Nh*slat*vel.lat*clon*vel.lon + (Ndot+vel.alt)*clat*clon*vel.lon
            -Nh*slat*vel.lat*clon*vel.lon - Nh*slat*acc.lat*slon
            -Nh*clat*vel.lat*vel.lat*slon - (Ndot+vel.alt)*slat*vel.lat*slon
            +(Ndot+vel.alt)*clat*clon*vel.lon - (Ndot+vel.alt)*slat*vel.lat*slon
            +(Nddot+acc.alt)*clat*slon;

        double zz =
            +(N*(1-EARTH_ECC_SQ)+pos.alt)*clat*acc.lat
            -(N*(1-EARTH_ECC_SQ)+pos.alt)*slat*vel.lat*vel.lat
            +2*(Ndot*(1-EARTH_ECC_SQ)+vel.alt)*clat*vel.lat
            +(Nddot*(1-EARTH_ECC_SQ)+acc.alt)*slat;

        return (cartesian){xx, yy, zz};
    }
    */

}

#endif // XM_GEODETIC_H_

