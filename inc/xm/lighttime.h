#ifndef XM_LIGHTTIME_H_
#define XM_LIGHTTIME_H_ 1

namespace xm {

    //{{{ internal
    namespace internal {

        struct dopdopdot {
            double doppler;
            double doppdot;
        };

        static const double SPEED_OF_LIGHT = 299792458.0;
        static const double INVERSE_SPEED_OF_LIGHT = 3.3356409519815204e-09;

        /*
        static dopdopdot doppler_derivs(statevec src, statevec dst) {
            cartesian range = dst.pos - src.pos;
            cartesian rrate = dst.vel - src.vel;
            double mag = len(range);
            cartesian unit = range * INVERSE_SPEED_OF_LIGHT / mag;

            double vdotr = dot(rrate, range);
            cartesian t0 = range * vdotr / mag;
            cartesian t1 = mag * rrate;

            double scale = 1.0 / (mag * mag * SPEED_OF_LIGHT);
            cartesian udot = scale * (t1 - t0);

            double p0 = dot(src.acc, unit) + dot(src.vel, udot);
            double p1 = 1.0 - dot(dst.vel, unit);
            double p2 = dot(dst.acc, unit) + dot(dst.vel, udot);
            double p3 = 1.0 - dot(src.vel, unit);

            return dopdopdot { p1/p3, (p0*p1 - p2*p3) / (p3*p3) };
        }
        */

        static dopdopdot doppler_derivs(statevec src, statevec dst) {
            cartesian diffp = dst.pos - src.pos;
            cartesian diffv = dst.vel - src.vel;
            double magp = len(diffp);
            cartesian unitp = diffp / magp;
            cartesian usubp = unitp * INVERSE_SPEED_OF_LIGHT;
            double dstdop = (1.0 - dot(dst.vel, usubp));
            double srcdop = (1.0 - dot(src.vel, usubp));

            cartesian usubpdot = (
                diffv - unitp*dot(diffv, unitp)
            ) / (magp * SPEED_OF_LIGHT);

            /*
            printf("XM first: %.18le, second: %.18le\n",
                (dot(src.acc, usubp) + dot(src.vel, usubpdot))*dstdop,
                (dot(dst.acc, usubp) + dot(dst.vel, usubpdot))*srcdop
            );
            */
            double doppdot = (
                (dot(src.acc, usubp) + dot(src.vel, usubpdot))*dstdop -
                (dot(dst.acc, usubp) + dot(dst.vel, usubpdot))*srcdop
            ) / (srcdop * srcdop);

            return (dopdopdot){ dstdop / srcdop, doppdot };
        }

        static inline statevec lighttime_rotate(statevec src, double seconds) {
            const double eav = 7292115.8553e-11;   // WGS-84 Earth Angular Velocity at J2000
            const double rot = seconds * eav;

            // Rotate by Greenwich Hour Angle (gha radians) at
            // Earth Angular Velocity (eav radians per second)
            const double eavt = eav + eav;
            const double eav2 = eav * eav;

            double ss = sin(rot);
            double cc = cos(rot);

            statevec dst;

            double px = cc*src.pos.x - ss*src.pos.y;
            double py = ss*src.pos.x + cc*src.pos.y;
            dst.pos.x = px;
            dst.pos.y = py;
            dst.pos.z = src.pos.z;

            double vx = cc*src.vel.x - ss*src.vel.y;
            double vy = ss*src.vel.x + cc*src.vel.y;
            dst.vel.x = vx - eav*py;
            dst.vel.y = vy + eav*px;
            dst.vel.z = src.vel.z;

            double ax = cc*src.acc.x - ss*src.acc.y - eavt*vy - eav2*px;
            double ay = ss*src.acc.x + cc*src.acc.y + eavt*vx - eav2*py;
            dst.acc.x = ax;
            dst.acc.y = ay;
            dst.acc.z = src.acc.z;

            return dst;
        }
    }
    //}}}

    struct timefreq {
        timecode time;
        double freq;
        double chirp;
    };

    static inline timefreq lighttime_forward(
        timefreq tf, const ephemeris& src, const ephemeris& dst
    ) {
        using namespace internal;
        statevec src_ecf = src.lookup(tf.time);
        statevec src_eci = lighttime_rotate(src_ecf, 0);
        statevec dst_eci = statevec();
        //print("src ecf:", std::setprecision(18), src_ecf);

        double delay = 0;
        for (int ii = 0; ii<5; ii++) {
            statevec dst_ecf = dst.lookup(tf.time + delay);
            //print("dst ecf:", std::setprecision(18), dst_ecf);
            dst_eci = lighttime_rotate(dst_ecf, +delay);
            double dist = len(dst_eci.pos - src_eci.pos);
            delay = dist * INVERSE_SPEED_OF_LIGHT;
        }

        dopdopdot ddd = doppler_derivs(src_eci, dst_eci);
        //printf("xm fwd: doppler=%.18lf, doppler dot=%.18le\n", ddd.doppler, ddd.doppdot);
        //printf("xm fwd: freq: %.18lf, chirp: %.18lf\n", tf.freq, tf.chirp);

        return (timefreq){
            tf.time + delay,
            tf.freq*ddd.doppler,
            tf.chirp*ddd.doppler + tf.freq*ddd.doppdot
        };
    }

    static inline timefreq lighttime_reverse(
        const ephemeris& src, const ephemeris& dst, timefreq tf
    ) {
        using namespace internal;
        statevec dst_ecf = dst.lookup(tf.time);
        //print("dst ecf:", std::setprecision(18), dst_ecf);
        statevec dst_eci = lighttime_rotate(dst_ecf, 0);
        statevec src_eci = statevec();

        double delay = 0;
        for (int ii = 0; ii<5; ii++) {
            statevec src_ecf = src.lookup(tf.time - delay);
            //print("src ecf:", std::setprecision(18), src_ecf);
            src_eci = lighttime_rotate(src_ecf, -delay);
            double dist = len(dst_eci.pos - src_eci.pos);
            delay = dist * INVERSE_SPEED_OF_LIGHT;
        }

        dopdopdot ddd = doppler_derivs(src_eci, dst_eci);
        double invdopp = 1.0 / ddd.doppler;
        //double src_freq = tf.freq / ddd.doppler;
        //printf("xm rev: doppler=%.18lf, doppler dot=%.18le\n", ddd.doppler, ddd.doppdot);
        //printf("xm rev: freq: %.18lf, chirp: %.18lf\n", tf.freq, tf.chirp);

        return (timefreq){
            tf.time - delay,
            tf.freq * invdopp,
            invdopp * (tf.chirp - invdopp * tf.freq * ddd.doppdot)
            //src_freq,
            //tf.chirp - src_freq*ddd.doppdot/ddd.doppler
        };
    }

}


#endif // XM_LIGHTTIME_H_

