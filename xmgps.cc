#include "xmtools.h"
using namespace xm;

int main(int argc, char* argv[]) {

    cmdline args(argc, argv, "make GPS C/A code");

    int64 prn   = args.getint64("prn", 1, "PRN 1-37");
    int64 count = args.getint64("count", 1023, "how many chips to make");
    str outpath = args.getoutput("output.tmp", "output blue file");

    args.done();

    bluewriter output(outpath);
    output->xdelta = 1.0/(1023*1000);
    output->xcount = count;
    
    vec<cfloat> data(256);
    gpsgold gold(prn);
    while (count > 0) {
        int64 amount = (count <= 256) ? count : 256;

        for (int64 ii = 0; ii<amount; ii++) {
            data[ii] = gold.chip();
            gold.next();
        }
        output.write(data.data(), amount*sizeof(cfloat));
        count -= amount;
    }

    return 0;
}
