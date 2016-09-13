#include "xmtools.h"
#include "xmkwds.h"
using namespace xm;

int main(int argc, char* argv[]) {
    using namespace internal;

    cmdline args(
        argc, argv,
        "Prints a JSON representation of the header from a BLUE file.\n"
        "May optionally print all of the extended header keywords too."
    );

    bool   kwds   = args.getswitch("kwds", "include the keywords");
    str inpath = args.getinput("input.tmp", "input BLUE file");
    args.done();

    bluereader input(inpath);

    printf("{\n");
    printf("  \"time\"   : \"%s\",\n",   format(input->time, 12).data());;
    printf("  \"type\"   : %04d,\n",     input->type);
    printf("  \"format\" : \"%.2s\",\n", input->format.data());
    printf("  \"tc+xs\"  : \"%s\",\n",   format(input->time + input->xstart, 12).data());
    printf("  \"xstart\" : %.18lf,\n",   input->xstart);
    printf("  \"xdelta\" : %.18lf,\n",   input->xdelta);
    printf("  \"xrate\"  : %.18lf,\n", 1/input->xdelta);

    if (input->type/1000 == 1) {
        int64 bytesize = xmbytesize(input->format);
        printf("  \"xspan\"  : %.18lf,\n", input->xdelta*input->xcount);
        printf("  \"xcount\" : %lld,\n", (long long)input->xcount);
        printf("  \"xunits\" : \"%s\",\n", xmunits(input->xunits).data());
        printf("  \"xbytes\" : %lld%s\n", bytesize, kwds ? "," : "");
    }

    if (input->type/1000 == 2) {
        int64 bytesize = xmbytesize(input->format);
        check(bytesize != 0, "known sample format");
        printf("  \"xspan\"  : %.18lf,\n", input->xdelta*input->xcount);
        printf("  \"xcount\" : %lld,\n", (long long)input->xcount);
        printf("  \"xunits\" : \"%s\",\n", xmunits(input->xunits).data());
        printf("  \"xbytes\" : %lld,\n", bytesize);
        printf("  \"tc+ys\"  : \"%s\",\n", format(input->time + input->ystart, 12).data());
        printf("  \"ystart\" : %.18lf,\n", input->ystart);
        printf("  \"ydelta\" : %.18lf,\n", input->ydelta);
        printf("  \"yrate\"  : %.18lf,\n", 1/input->ydelta);
        printf("  \"yspan\"  : %.18lf,\n", input->ydelta*input->ycount);
        printf("  \"ycount\" : %lld,\n", (long long)input->ycount);
        printf("  \"ybytes\" : %lld,\n", bytesize*input->xcount);
        printf("  \"yunits\" : \"%s\"%s\n", xmunits(input->yunits).data(), kwds ? "," : "");
    }

    if (input->type/1000 == 3 || input->type/1000 == 5) {
        printf("  \"xspan\"  : %.18lf,\n", input->xdelta*input->xcount);
        printf("  \"xcount\" : %lld,\n", (long long)input->xcount);
        printf("  \"xbytes\" : %d,\n", input->itemsize);
        printf("  \"xunits\" : \"%s\",\n", xmunits(input->xunits).data());

        printf("  \"fields\": [\n");
        for (int64 ii = 0; ii<(int64)input->fields.size(); ii++) {
            printf(
                "    { \"name\": \"%.4s\", \"format\": \"%.2s\", \"offset\": %d }",
                input->fields[ii].name.data(), 
                input->fields[ii].format.data(),
                input->fields[ii].offset
            );
            if (ii != (int64)input->fields.size() - 1) printf(",");
            printf("\n");
        }
        printf("  ]%s\n", kwds ? "," : "");
    }

    if (kwds) {
        printf("  \"kwds\": [\n");
        for (int64 ii = 0; ii<(int64)input->kwds.storage.size(); ii++) {
            printkwd(
                input->kwds.storage[ii], "    ", 
                ii == (int64)input->kwds.storage.size() - 1
            );
        }
        printf("  ]\n");
    }

    printf("}\n");

    return 0;
}

