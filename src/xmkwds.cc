#include "xmtools.h"
#include "xm/printkwd.h"
using namespace xm;

int main(int argc, char* argv[]) {
    cmdline args(
        argc, argv,
        "Prints a JSON representation of the keywords from a BLUE file.\n"
    );

    bool debug    = args.getswitch("debug", "binary debug of the keywords");
    string inpath = args.getinput("input.tmp", "input BLUE file");
    args.done();

    bluereader input(inpath);

    printf("[\n");
    if (debug) {
        for (int64 ii = 0; ii<(int64)input->kwds.storage.size(); ii++) {
            const bluekeyword& kwd = input->kwds.storage[ii];
            printf("  [\"%s\", \"%c\", \"", kwd.name.data(), kwd.code);

            for (int64 jj = 0; jj<(int64)kwd.bytes.size(); jj++) {
                printf("\\%02x", (unsigned char)kwd.bytes[jj]);
            }
            printf("\"%s\n", ii == (int64)input->kwds.storage.size() - 1 ? "" : ",");
        }
    } else {
        for (int64 ii = 0; ii<(int64)input->kwds.storage.size(); ii++) {
            using namespace internal;
            printkwd(
                input->kwds.storage[ii], "    ", 
                ii == (int64)input->kwds.storage.size() - 1
            );
        }
    }
    printf("]\n");

    return 0;
}

