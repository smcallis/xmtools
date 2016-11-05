#include "xmtools.h"
using namespace xm;

int main(int argc, char* argv[]) {

    cmdline args(argc, argv, "make a continuous wave tone");

    const timecode deftime = timecode::parse("2000-01-01T12:00:00");
    const timecode tstart  = args.gettimecode("tstart", deftime, "starting time");
    const double tspan     = args.getdouble("tspan", 1.0, "time stpan");
    const double rate      = args.getdouble("rate", 1e6, "sample rate");
    const double freq      = args.getdouble("freq", 0, "frequency of the tone");
    const double phase     = args.getdouble("phase", 0, "phase in cycles at first sample");
    const string outpath   = args.getoutput("output.tmp", "output file");

    args.done();

    check(tspan > 0, "must have positive tspan");
    check(rate > 0, "must have sample rate");
    const int64 samples = llrint(tspan*rate);

    bluewriter output(outpath);
    output->time = tstart;
    output->xdelta = 1/rate;
    output->xcount = samples;

    blocktuner bt(freq/rate, phase);
    vec<cfloat> data(1024);

    int64 offset = 0;
    while (offset < samples) {
        int64 amount = min(1024, samples - offset);
        for (int64 ii = 0; ii<amount; ii++) {
            data[ii] = 1;
        }
        bt.apply(data.data(), offset, amount);
        output.write(data.data(), amount*sizeof(cfloat));
        offset += amount;
    }

    return 0;
}
