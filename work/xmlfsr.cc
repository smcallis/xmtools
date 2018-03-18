#include "xmtools.h"
using namespace xm;

int main(int argc, char* argv[]) {

    cmdline args(argc, argv, "Generate data using a Linear Feedback Shift Register");

    args.done();

    shiftreg lfsr;
    for (int64 ii = 0; ii<20; ii++) {
        printf("%016lx\n", lfsr.val());
        lfsr.shr();
    }


    return 0;
}
