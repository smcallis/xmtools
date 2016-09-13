#include "xmtools.h"

static void test0000() {
    xm::check(true, "check true works");
}

static void test0001() {
    // Does vec<float> promote to vec<cfloat> correctly?
    xm::vec<float> v(100);
    v[1] = 3;
    xm::cfloat z(1, 3);
    xm::vec<xm::cfloat> w = z*v + v;
    xm::check(w.size() == 100, "size");
    xm::check(w[1].re == 6, "real part");
    xm::check(w[1].im == 9, "imag part");
}

static void test0002() {
    // Does mat<float> promote to mat<cfloat> correctly?
    xm::mat<float> m(100, 100);
    m(1, 1) = 3;
    xm::cfloat z(1, 3);
    xm::mat<xm::cfloat> w = z*m + m;
    xm::check(w.size() == 100*100, "size");
    xm::check(w(1, 1).re == 6, "real part");
    xm::check(w(1, 1).im == 9, "imag part");
}

static void test0003() {
    // testing tuples

    int8_t aa;
    int16_t bb;
    int32_t cc;
    int64_t dd;
    uint8_t ee;
    uint16_t ff;
    uint32_t gg;
    uint64_t hh;

    xm::multiref(aa, bb, cc, dd, ee, ff, gg, hh) = xm::multival(1, 2, 3, 4, 5, 6, 7, 8);
    xm::multiref(aa, bb, cc, dd, ee, ff, gg    ) = xm::multival(1, 2, 3, 4, 5, 6, 7   );
    xm::multiref(aa, bb, cc, dd, ee, ff        ) = xm::multival(1, 2, 3, 4, 5, 6      );
    xm::multiref(aa, bb, cc, dd, ee            ) = xm::multival(1, 2, 3, 4, 5         );
    xm::multiref(aa, bb, cc, dd                ) = xm::multival(1, 2, 3, 4            );
    xm::multiref(aa, bb, cc                    ) = xm::multival(1, 2, 3               );
    xm::multiref(aa, bb                        ) = xm::multival(1, 2                  );
    xm::multiref(aa                            ) = xm::multival(1                     );
    xm::multiref(                              ) = xm::multival(                      );

    xm::check(aa == 1, "aa");
    xm::check(bb == 2, "bb");
    xm::check(cc == 3, "cc");
    xm::check(dd == 4, "dd");
    xm::check(ee == 5, "ee");
    xm::check(ff == 6, "ff");
    xm::check(gg == 7, "gg");
    xm::check(hh == 8, "hh");

    xm::dict<xm::tuple<int64, int64>, int> xx;
    xx[xm::multival(10, 20)] = 30;
    xx[xm::multival(30, 40)] = 90;
    xm::check(xx[xm::multival(10, 20)] == 30, "10,20");
    xm::check(xx[xm::multival(30, 40)] == 90, "30,40");

    xm::check(xm::multiref(aa, bb, cc, dd, ee, ff, gg, hh) == xm::multival(1, 2, 3, 4, 5u, 6u, 7u, 8u), "8");
    xm::check(xm::multiref(aa, bb, cc, dd, ee, ff, gg    ) == xm::multival(1, 2, 3, 4, 5u, 6u, 7u    ), "7");
    xm::check(xm::multiref(aa, bb, cc, dd, ee, ff        ) == xm::multival(1, 2, 3, 4, 5u, 6u        ), "6");
    xm::check(xm::multiref(aa, bb, cc, dd, ee            ) == xm::multival(1, 2, 3, 4, 5u            ), "5");
    xm::check(xm::multiref(aa, bb, cc, dd                ) == xm::multival(1, 2, 3, 4                ), "4");
    xm::check(xm::multiref(aa, bb, cc                    ) == xm::multival(1, 2, 3                   ), "3");
    xm::check(xm::multiref(aa, bb                        ) == xm::multival(1, 2                      ), "2");
    xm::check(xm::multiref(aa                            ) == xm::multival(1                         ), "1");
    xm::check(xm::multiref(                              ) == xm::multival(                          ), "0");
 
}




int main() {
    test0000();
    test0001();
    test0002();
    test0003();

    return 0;
}
