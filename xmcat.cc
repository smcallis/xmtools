#include "xmtools.h"
using namespace xm;

int main(int argc, char* argv[]) {
    cmdline args(argc, argv, "cut out a piece out of a file");

    bool ignore     = args.getswitch("override", "ignore mismatched timecodes");
    bool remove     = args.getswitch("delete", "delete input files after concatenation");
    bool copykwds   = args.getswitch("copykwds", "copy keywords from the first input");
    bool getstdin   = args.getswitch("stdin", "take file names from stdin");
    str outpath     = args.getoutput("output.tmp", "output file");
    list<str> inpaths = args.getinputs("input.tmp", "input file");
    args.done();

    if (getstdin) {
        static char inpath[8192];
        while (fgets(inpath, 8192, stdin)) {
            inpaths.append(strip(inpath));
        }
    }

    check(inpaths.size() >= 1, "need at least one input file");

    bluemeta meta;
    timecode expect;
    str format;
    int64 total = 0;
    {
        bluereader input(inpaths[0]);
        // XXX: Add support for type 3000 and 5000 files
        check(
            input->type/1000 == 1 ||
            input->type/1000 == 2,
            "expect type 1000 or 2000 files"
        );
        if (copykwds) check(input.kwds_ready(), "keywords must be ready");
        meta = *input;
        expect = input->time + (
            input->type/1000 == 1 ?
            input->xstart : input->ystart
        ) + (
            input->type/1000 == 1 ?
            input->xcount*input->xdelta :
            input->ycount*input->ydelta
        );
        total += input->type/1000 == 1 ? input->xcount : input->ycount;
    }

    for (int64 ii = 1; ii<inpaths.size(); ii++) {
        bluereader input(inpaths[ii]);
        check(input->type/1000 == meta.type/1000, "matching types");
        check(input->format == meta.format, "matching types");
        if (meta.type/1000 == 1) {
            total += input->xcount;
        } else {
            total += input->ycount;
            check(meta.xcount == input->xcount, "matching xcount");
        }
        if (!ignore) {
            timecode intime = input->time + (
                input->type/1000 == 1 ? input->xstart : input->ystart
            );
            check(fabs(intime - expect) < 1e-9, "match within a nanosecond %.18le", fabs(intime - expect));
            expect = intime + (
                input->type/1000 == 1 ?
                input->xcount*input->xdelta :
                input->ycount*input->ydelta
            );
            check(input->xdelta == meta.xdelta, "matching xdelta");
            check(input->ydelta == meta.ydelta, "matching ydelta");
        }
    }

    bluewriter output(outpath);

    output->type     = meta.type;
    output->time     = meta.time;
    output->format   = meta.format;
    if (meta.type/1000 == 1) {
        output->xcount = total;
        output->ycount = 1;
    } else {
        output->xcount = meta.xcount;
        output->ycount = total;
    }
    output->xstart   = meta.xstart;
    output->ystart   = meta.ystart;
    output->xdelta   = meta.xdelta;
    output->ydelta   = meta.ydelta;
    output->xunits   = meta.xunits;
    output->yunits   = meta.yunits;
    output->itemsize = meta.itemsize;
    output->kwds     = meta.kwds;

    vec<uint8_t> scratch(8192);
    for (int64 ii = 0; ii<inpaths.size(); ii++) {
        bluereader input(inpaths[ii]);
        int64 length = input->xcount*input->ycount*input->itemsize;
        int64 offset = 0;
        while (length > 0) {
            int64 amount = length;
            if (amount > 8192) amount = 8192;
            input.grab(offset, scratch.data(), amount);
            length -= amount;
            offset += amount;
            output.write(scratch.data(), amount);
        }
        if (remove) unlink(inpaths[ii].data());
    }

    return 0;
}

