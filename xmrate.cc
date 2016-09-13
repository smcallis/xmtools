#include "xmtools.h"
using namespace xm;

int main(int argc, char* argv[]) {

    cmdline args(argc, argv, "resample data to a new rate");
    const timecode deftime = { 0, nan("sentinel") };
    timecode tstart = args.gettimecode("tstart", deftime, "starting time (default beginning)");
    double tspan    = args.getdouble("tspan", -1, "time span after start (default all)");
    double outrate  = args.getdouble("rate", "output sample rate");
    double percent  = args.getdouble("percent", 80, "percentage of bandwidth to preserve");
    int64 window    = args.getint64("firwin", 2, "FIR window for resampling");
    bool copykwds   = args.getswitch("copykwds", "copy the input keywords to the output file");
    str inpath      = args.getinput("input.tmp", "input blue file");
    str outpath     = args.getinput("output.tmp", "output blue file");
    args.done();

    check(outrate > 0, "need positive sample rate");

    bluereader input(inpath);
    check(input->type/1000 == 1, "must be Type 1000 file");
    if (isnan(tstart.fract)) {
        tstart = input->time + input->xstart;
    }
    if (tspan < 0) {
        timecode tstop = (
            input->time + input->xstart +
            input->xcount*input->xdelta
        );
        tspan = tstop - tstart;
    }
    check(tspan >= 0, "can't have negative tspan");

    const double inrate = 1/input->xdelta;

    double dwidth = min(percent * .01 * outrate / inrate, 1);
    int64 taps = (int64)ceil(8 * apodize(window) / dwidth);
    if (taps%2) taps += 1;
    polyphase pp(window, dwidth, taps);

    const int64 samples = llrint(outrate*tspan);

    bluewriter output(outpath);
    const double xdelta = 1/outrate;
    const timecode tbegin = input->time + input->xstart;
    output->time = input->time;
    output->xstart = tstart - input->time;
    output->xdelta = xdelta;
    output->xcount = samples;
    output->xunits = input->xunits;

    if (copykwds) {
        check(input.kwds_ready(), "keywords must be ready");
        output->kwds = input->kwds;
    }

    vec<cfloat> data(1024);
    vec<cfloat> grab;
    int64 offset = 0;
    while (offset < samples) {
        int64 amount = min(1024, samples - offset);

        double want_lo = (tstart + xdelta*offset - tbegin)*inrate;
        double want_hi = (tstart + xdelta*(offset + amount) - tbegin)*inrate;
        int64 grab_lo = (int64)floor(want_lo - taps/2);
        int64 grab_hi = (int64) ceil(want_hi + taps/2);
        int64 grab_len = grab_hi - grab_lo;
        if (grab.size() < grab_len) grab.resize(2*grab_len);

        input.grabcf(grab_lo, grab.data(), grab_len);
        pp.resample(
            data.data(), amount, grab.data(), 
            want_lo - grab_lo, want_hi - grab_lo
        );

        output.write(data.data(), amount*sizeof(cfloat));

        offset += amount;
    }

    return 0;
}
