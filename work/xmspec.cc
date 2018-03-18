#include "xmtools.h"
#include "kissfft.h"
using namespace xm;

int main(int argc, char* argv[]) {

    cmdline args(argc, argv, "make a spectra");

    args.done();

    /*

    const int64 size = 16384;
    vector<cfloat> data(size, 0);
    data[1] = 1;

    //kissfft<float> fft(size);
    //vec<cfloat> temp(size);
    //fft.exec(temp.data(), data.data());
    kisssse plan(size);
    plan.fft(data.data());

    FILE* gnuplot = popen("gnuplot -persist", "w");
    fprintf(gnuplot, "set grid\nplot '-' w l title 'real', '-' w l title 'imag'\n");

    for (int64 ii = 0; ii<size; ii++) {
        fprintf(gnuplot, "%le\n", data[ii].re);
    }
    fprintf(gnuplot, "e\n");

    for (int64 ii = 0; ii<size; ii++) {
        fprintf(gnuplot, "%le\n", data[ii].im);
    }
    fprintf(gnuplot, "e\n");

    fflush(gnuplot);


    */

    return 0;
}
