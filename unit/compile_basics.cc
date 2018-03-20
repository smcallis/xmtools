#include <xm/basics.h>

int main() {
    using namespace xm;

    check(1 == 1, "sanity");
    ::free(alloc<char>());

    int a = 1;
    int b = 2;
    swap(a, b);
    check(min(0, 1) == 0, "sanity");
    check(max(0, 1) == 1, "sanity");
    check(min(0, 1, 2) == 0, "sanity");
    check(max(0, 1, 2) == 2, "sanity");
    check(sqr(1) == 1, "sanity");

    return 0;
}
