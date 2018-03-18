#ifndef XM_CARTESIAN_H_
#define XM_CARTESIAN_H_ 1

namespace xm {

    //{{{ cartesian
    struct cartesian {
        double x, y, z;
    };

    static inline cartesian operator +(cartesian aa, cartesian bb) {
        return (cartesian){ aa.x + bb.x, aa.y + bb.y, aa.z + bb.z };
    }

    static inline cartesian operator -(cartesian aa, cartesian bb) {
        return (cartesian){ aa.x - bb.x, aa.y - bb.y, aa.z - bb.z };
    }

    static inline cartesian operator *(double aa, cartesian bb) {
        return (cartesian){ aa*bb.x, aa*bb.y, aa*bb.z };
    }

    static inline cartesian operator *(cartesian aa, double bb) {
        return (cartesian){ aa.x*bb, aa.y*bb, aa.z*bb };
    }

    static inline cartesian operator /(cartesian aa, double bb) {
        return aa * (1.0/bb);
    }

    static inline cartesian operator +=(cartesian& aa, cartesian bb) {
        return aa = aa + bb;
    }

    static inline cartesian operator -=(cartesian& aa, cartesian bb) {
        return aa = aa - bb;
    }

    static inline cartesian operator *=(cartesian& aa, double bb) {
        return aa = aa * bb;
    }

    static inline cartesian operator /=(cartesian& aa, double bb) {
        return aa = aa / bb;
    }

    static inline cartesian operator -(cartesian aa) {
        return (cartesian){ -aa.x, -aa.y, -aa.z };
    }

    static inline double len(cartesian aa) {
        return hypot(aa.x, hypot(aa.y, aa.z));
    }

    static inline double fastlen(cartesian aa) {
        // This one is less numerically stable, and it will have problems
        // if the squares underflow to zero or overflow to infinity, but
        // it is a little faster than the safe len(...) function above.
        return sqrt(aa.x*aa.x + aa.y*aa.y + aa.z*aa.z);
    }

    static inline double dist(cartesian aa, cartesian bb) {
        return len(aa - bb);
    }

    static inline cartesian unit(cartesian aa) {
        return (1.0/len(aa))*aa;
    }

    static inline double dot(cartesian aa, cartesian bb) {
        return aa.x*bb.x + aa.y*bb.y + aa.z*bb.z;
    }

    static inline cartesian cross(cartesian aa, cartesian bb) {
        return (cartesian){
            aa.y*bb.z - aa.z*bb.y,
            aa.z*bb.x - aa.x*bb.z,
            aa.x*bb.y - aa.y*bb.x
        };
    }

    /*
    static inline std::ostream& operator <<(std::ostream& os, const cartesian& xyz) {
        os << "cartesian(x: " << xyz.x << ", y: " << xyz.y << ", z: " << xyz.z << ")";
        return os;
    }
    */

    //}}}
    //{{{ transform
    struct transform {
        // Transform Matrix:
        //
        //   u.x   u.y   u.z   t.x
        //   v.x   v.y   v.z   t.y
        //   w.x   w.y   w.z   t.z
        //    0     0     0     1
        //
        cartesian u, v, w, t;

        cartesian operator ()(cartesian aa) const {
            return t + (cartesian){
                dot(u, aa), dot(v, aa), dot(w, aa)
            };
        }
    };

    static inline transform translate(cartesian tt) {
        return (transform){
            (cartesian){ 1, 0, 0 },
            (cartesian){ 0, 1, 0 },
            (cartesian){ 0, 0, 1 },
            tt
        };
    }

    static inline transform operator *(transform aa, transform bb) {
        return (transform){
            (cartesian){
                aa.u.x*bb.u.x + aa.u.y*bb.v.x + aa.u.z*bb.w.x,
                aa.u.x*bb.u.y + aa.u.y*bb.v.y + aa.u.z*bb.w.y,
                aa.u.x*bb.u.z + aa.u.y*bb.v.z + aa.u.z*bb.w.z
            },
            (cartesian){
                aa.v.x*bb.u.x + aa.v.y*bb.v.x + aa.v.z*bb.w.x,
                aa.v.x*bb.u.y + aa.v.y*bb.v.y + aa.v.z*bb.w.y,
                aa.v.x*bb.u.z + aa.v.y*bb.v.z + aa.v.z*bb.w.z
            },
            (cartesian){
                aa.w.x*bb.u.x + aa.w.y*bb.v.x + aa.w.z*bb.w.x,
                aa.w.x*bb.u.y + aa.w.y*bb.v.y + aa.w.z*bb.w.y,
                aa.w.x*bb.u.z + aa.w.y*bb.v.z + aa.w.z*bb.w.z
            },
            (cartesian){
                aa.u.x*bb.t.x + aa.u.y*bb.t.y + aa.u.z*bb.t.z + aa.t.x,
                aa.v.x*bb.t.x + aa.v.y*bb.t.y + aa.v.z*bb.t.z + aa.t.y,
                aa.w.x*bb.t.x + aa.w.y*bb.t.y + aa.w.z*bb.t.z + aa.t.z
            }
        };
    }


    //}}}

}

#endif // XM_CARTESIAN_H_

