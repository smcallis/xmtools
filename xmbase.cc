#include "xmtools.h"
using namespace xm;

template<class type>
void baseband(bluereader& input, bluewriter& output, ssize_t offset, ssize_t length) {

}

int main(int argc, char* argv[]) {
    cmdline args(
        argc, argv, "convert real-valued data to complex\n"
        "tunes the data down by fs/4, filters using a halfband filter,\n"
        "and decimates by two.  Similar to a Hilbert transform."
    );
    double bandwidth = args.getdouble("bandwidth", 0, "pass band to preserve, default is 80%");
    timecode tstart  = args.gettimecode("tstart", timecode{0,0}, "starting timecode, default is the beginning");
    double tspan     = args.getdouble("tspan", 0, "amount of time to keep, default is whole file");
    string inpath    = args.getinput("input.tmp", "input real-valued data");
    string outpath   = args.getoutput("output.tmp", "output complex float");
    args.done();

    bluereader input(inpath);
    check(input->format[0] == 'S', "requires real-valued input");
    check(input->type/1000 == 1, "requires Type 1000 input");

    bluewriter output(outpath);
    output->time   = input->time;
    output->xstart = input->xstart;
    output->xdelta = input->xdelta*2;
    output->xcount = input->xcount/2;
    output->xunits = input->xunits;

    // XXX: use tstart and tspan
    int64_t offset = 0;
    int64_t length = input->xcount;
    double dwidth = .5;

    switch (input->format[1]) {
        case 'B': return baseband<int8_t>  (input, output, offset, length);
        case 'I': return baseband<int16_t> (input, output, offset, length);
        case 'L': return baseband<int32_t> (input, output, offset, length);
        case 'X': return baseband<int64_t> (input, output, offset, length);
        case 'F': return baseband<float>   (input, output, offset, length);
        case 'D': return baseband<double>  (input, output, offset, length);
        default: check(false, "unsupported input file format");
    }

    return 0;
}

