#include "xmtools.h"
using namespace xm;

int main(int argc, char* argv[]) {
    cmdline args(argc, argv, "cut out a piece out of a file");
    timecode tstart = args.gettimecode("tstart", (timecode){-1,-1}, "starting timecode, default is the beginning");
    double tspan    = args.getdouble("tspan", -1, "amount of time to keep, default is whole file");
    bool copykwds   = args.getswitch("copykwds", "copy the input keywords to the output file");
    string inpath   = args.getinput("input.tmp", "input file");
    string outpath  = args.getoutput("output.tmp", "output file");
    args.done();

    bluereader input(inpath);
    bluewriter output(outpath);

    output->type     = input->type;
    output->time     = input->time;
    output->format   = input->format;
    output->xdelta   = input->xdelta;
    output->xunits   = input->xunits;
    output->ydelta   = input->ydelta;
    output->yunits   = input->yunits;
    output->fields   = input->fields;
    output->itemsize = input->itemsize;

    if (copykwds) {
        check(input.kwds_ready(), "keywords must be ready");
        output->kwds = input->kwds;
    }

    int64 byte_offset = 0;
    int64 total_bytes = 0;

    switch (input->type/1000) {

        case 1: case 3: case 5: {
            check(input->xdelta != 0, "can't have zero xdelta");
            if (tstart.whole == -1 && tstart.fract == -1) {
                tstart = input->time + input->xstart;
            }
            if (tspan < 0) {
                tspan = input->xcount*input->xdelta;
            }

            int64 sample_offset = llrint(
                (tstart - (input->time + input->xstart))/input->xdelta
            );
            int64 sample_count = llrint(tspan/input->xdelta);

            output->xcount = sample_count;
            output->xstart = input->xstart + sample_offset*input->xdelta;

            byte_offset = input->itemsize*sample_offset;
            total_bytes = input->itemsize*sample_count;
        } break;

        case 2: {
            check(input->ydelta != 0, "can't have zero ydelta");
            if (tstart.whole == -1 && tstart.fract == -1) {
                tstart = input->time + input->ystart;
            }
            if (tspan < 0) {
                tspan = input->ycount*input->ydelta;
            }

            int64 row_offset = llrint(
                (tstart - (input->time + input->ystart))/input->ydelta
            );
            int64 row_count = llrint(tspan/input->ydelta);

            output->xcount = input->xcount;
            output->ycount = row_count;
            output->ystart = input->ystart + row_offset*input->ydelta;

            byte_offset = input->itemsize*input->xcount*row_offset;
            total_bytes = input->itemsize*input->xcount*row_count;
        } break;

        default: check(false, "unsupported file type %d", input->type);
    }

    check(total_bytes >= 0, "can't handle negative cut size");

    const int64 maxblock = 65536;
    vector<uint8_t> buffer(maxblock);
    while (total_bytes > 0) {
        int64 amount = total_bytes;
        if (amount > maxblock) amount = maxblock;
        input.grab(byte_offset, buffer.data(), amount);
        output.write(buffer.data(), amount);
        byte_offset += amount;
        total_bytes -= amount;
    }

    return 0;
}

