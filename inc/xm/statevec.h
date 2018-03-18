#ifndef XM_STATEVEC_H_
#define XM_STATEVEC_H_ 1

namespace xm {

    //{{{ statevec
    struct statevec {
        cartesian pos, vel, acc;
    };

    static inline statevec operator +(statevec aa, statevec bb) {
    	return (statevec){ aa.pos + bb.pos, aa.vel + bb.vel, aa.acc + bb.acc};
    }

    /*
    static inline std::ostream& operator <<(std::ostream& os, const statevec& sv) {
        os << "statevec(";
        os << "pos: { " << sv.pos.x << ", " << sv.pos.y << ", " << sv.pos.z << " }, ";
        os << "vel: { " << sv.vel.x << ", " << sv.vel.y << ", " << sv.vel.z << " }, ";
        os << "acc: { " << sv.acc.x << ", " << sv.acc.y << ", " << sv.acc.z << " }";
        os << ")";
        return os;
    }
    */

    //}}}
    //{{{ timestate
    struct timestate {
        timecode tc;
        statevec sv;
    };

    static inline bool operator <(const timestate& aa, const timestate& bb) {
        return (aa.tc - bb.tc) < 0.0;
    }

    namespace internal {

        // Evaluates an interpolating polynomial:
        //
        //      P(t) = a + b*t + c*t^2 + d*t^3 + e*t^4 + f*t^5
        //
        //  Such that:
        //
        //      P(-1) = plo      P(+1) = phi
        //     P'(-1) = vlo     P'(+1) = vhi
        //     P"(-1) = alo     P"(+1) = ahi
        //
        static double hermite5(
            double plo, double vlo, double alo,
            double phi, double vhi, double ahi,
            double tt
        ) {
            plo *= .0625; vlo *= .0625; alo *= .0625;
            phi *= .0625; vhi *= .0625; ahi *= .0625;

            double aa =   8*phi +  8*plo -  5*vhi +  5*vlo + 1*ahi + 1*alo;
            double bb =  15*phi - 15*plo -  7*vhi -  7*vlo + 1*ahi - 1*alo;
            double cc =                     6*vhi -  6*vlo - 2*ahi - 2*alo;
            double dd = -10*phi + 10*plo + 10*vhi + 10*vlo - 2*ahi + 2*alo;
            double ee =                  -  1*vhi +  1*vlo + 1*ahi + 1*alo;
            double ff =   3*phi -  3*plo -  3*vhi -  3*vlo + 1*ahi - 1*alo;

            return aa + (bb + (cc + (dd + (ee + ff*tt)*tt)*tt)*tt)*tt;
        }

        // Evaluates an interpolating polynomial:
        //
        //      P(t) = a + b*t + c*t^2 + d*t^3
        //
        //  Such that:
        //
        //      P(-1) = plo      P(+1) = phi
        //     P'(-1) = vlo     P'(+1) = vhi
        //
        static double hermite3(
            double plo, double vlo,
            double phi, double vhi,
            double tt
        ) {
            plo *= .25; vlo *= .25;
            phi *= .25; vhi *= .25;

            double aa =  2*phi + 2*plo - 1*vhi + 1*vlo;
            double bb =  3*phi - 3*plo - 1*vhi - 1*vlo;
            double cc =                  1*vhi - 1*vlo;
            double dd = -1*phi + 1*plo + 1*vhi + 1*vlo;

            return aa + (bb + (cc + dd*tt)*tt)*tt;
        }

        // This is really just linear interpolation with t in [-1,+1]
        static double hermite1(double plo, double phi, double tt) {
            return (plo*(1.0 - tt) + phi*(1.0 + tt))*0.5;
        }

    }

    static inline statevec interp(
        const timestate& ts0, const timestate& ts1, const timecode& tc
    ) {
        using namespace internal;
        double range = ts1.tc - ts0.tc;
        double half = range*.5;
        double tt = ((tc - ts1.tc) + (tc - ts0.tc)) / range;

        return (statevec){
            (cartesian){
                hermite5(
                    ts0.sv.pos.x, ts0.sv.vel.x*half, ts0.sv.acc.x*half*half,
                    ts1.sv.pos.x, ts1.sv.vel.x*half, ts1.sv.acc.x*half*half, tt
                ),
                hermite5(
                    ts0.sv.pos.y, ts0.sv.vel.y*half, ts0.sv.acc.y*half*half,
                    ts1.sv.pos.y, ts1.sv.vel.y*half, ts1.sv.acc.y*half*half, tt
                ),
                hermite5(
                    ts0.sv.pos.z, ts0.sv.vel.z*half, ts0.sv.acc.z*half*half,
                    ts1.sv.pos.z, ts1.sv.vel.z*half, ts1.sv.acc.z*half*half, tt
                )
            },
            (cartesian){
                hermite3(
                    ts0.sv.vel.x, ts0.sv.acc.x*half,
                    ts1.sv.vel.x, ts1.sv.acc.x*half, tt
                ),
                hermite3(
                    ts0.sv.vel.y, ts0.sv.acc.y*half,
                    ts1.sv.vel.y, ts1.sv.acc.y*half, tt
                ),
                hermite3(
                    ts0.sv.vel.z, ts0.sv.acc.z*half,
                    ts1.sv.vel.z, ts1.sv.acc.z*half, tt
                )
            },
            (cartesian){
                hermite1(ts0.sv.acc.x, ts1.sv.acc.x, tt),
                hermite1(ts0.sv.acc.y, ts1.sv.acc.y, tt),
                hermite1(ts0.sv.acc.z, ts1.sv.acc.z, tt)
            }
        };
    }
    //}}}

}

#endif // XM_STATEVEC_H_

