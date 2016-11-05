#include "xmtools.h"
using namespace xm;

int main(int argc, char* argv[]) {

    cmdline args(argc, argv, "make a white noise file");

    const timecode deftime = timecode::parse("2000-01-01T12:00:00");
    const timecode tstart  = args.gettimecode("tstart", deftime, "starting time");
    const double tspan     = args.getdouble("tspan", 1.0, "time stpan");
    const double rate      = args.getdouble("rate", 1e6, "sample rate");
    const int64 seed       = args.getint64("seed", 0, "seed for random number generator");
    const string outpath   = args.getoutput("output.tmp", "output file");

    args.done();

    check(tspan > 0, "must have positive tspan");
    check(rate > 0, "must have sample rate");
    int64 samples = llrint(tspan*rate);

    prng random(seed);

    bluewriter output(outpath);
    output->time = tstart;
    output->xdelta = 1/rate;
    output->xcount = samples;

    vec<cfloat> data(1024);

    while (samples > 0) {
        int64 amount = min(1024, samples);
        for (int64 ii = 0; ii<amount; ii++) {
            data[ii] = random.cxnormal();
        }
        output.write(data.data(), amount*sizeof(cfloat));
        samples -= amount;
    }

    return 0;
}
