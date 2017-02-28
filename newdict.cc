#include "xmtools.h"

int main() {
    using namespace xm;
    dict<int32_t, int64_t> dd;
    //dd.reserve(180);
    dd.debug();
    dd.test();
    getchar();
    printf("inserting\n");
    for (int ii = 0; ii<180; ii++) {
        dd.insert(ii*ii, (ii + 1)*1001);
        dd.debug();
        dd.test();
        getchar();
        for (int jj = 0; jj<=ii; jj++) {
            check(dd[jj*jj] == (jj + 1)*1001, "matches %d %d", jj*jj, (jj + 1)*1001);
        }
    }
    list<int32_t> keys = dd.keys();
    list<int32_t> vals = dd.vals();
    for (int ii = 0; ii<180; ii++) {
        check(keys[ii] == ii*ii, "keys correct");
        check(vals[ii] == (ii + 1)*1001, "vals correct");
    }

    printf("removing\n");
    for (int ii = 0; ii<180; ii++) {
        dd.remove(ii*ii);
        dd.shrink();
        dd.debug();
        dd.test();
        getchar();
        for (int jj = ii+1; jj<180; jj++) {
            check(dd[jj*jj] == (jj + 1)*1001, "matches %d %d", jj*jj, (jj + 1)*1001);
        }
    }

    return 0;
}

