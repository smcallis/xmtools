#include "xmtools.h"

int main() {
    using namespace xm;

    fprintf(stderr, "here we go\n");
    dict<int64> one;
    for (int64 ii = 0; ii<5000000; ii++) {
        one.insert(ii);
    }

    fprintf(stderr, "and then this\n");
    dict<int64> two;
    for (int64 ii = 0; ii<one.size(); ii++) {
        two.insert(one.key(ii));
    }
    fprintf(stderr, "all done\n");

    return 0;
}

