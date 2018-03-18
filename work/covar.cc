#include "xmtools.h"

namespace xm {
    //{{{ covarmin covarmax

    namespace internal {
        struct candidate {
            double score, *data;
        };

        static inline bool operator <(
            const candidate& aa, const candidate& bb
        ) {
            return aa.score < bb.score;
        }

        template<class cost, int sign>
        double covaropt(
            cost func, double* center, double* steps,
            int64 dim, int64 its, uint64_t seed
        ) {
            prng rand(seed);

            const int64 children = ::llrint(4 + 3*::log(dim));
            const int64 parents = children/2;

            internal::candidate pop[parents + children];
            double storage[dim*(parents + children)];
            for (int64 ii = 0; ii<parents + children; ii++) {
                pop[ii].data = storage + ii*dim;
            }
            
            double mean[dim], chol[dim*dim], scale=1;
            for (int64 ii = 0; ii<dim; ii++) {
                mean[ii] = pop[0].data[ii] = 0;
                for (int64 jj = 0; jj<dim; jj++) {
                    chol[ii*dim + jj] = (ii == jj);
                }
            }
            pop[0].score = func(center);
            if (isinf(pop[0].score) == -1 || isnan(pop[0].score)) {
                return pop[0].score;
            }

            int64 previous = 1;
            while (its > 0) {

                // create new offspring, retaining our one best from before
                int64 improved = 0;
                for (int64 pp = previous; pp<parents + children; pp++) {
                    double probe[dim], unit[dim], *data = pop[pp].data;
                    for (;;) {
                        for (int64 ii = 0; ii<dim; ii++) {
                            data[ii] = mean[ii];
                            unit[ii] = rand.normal();
                            for (int64 jj = 0; jj<=ii; jj++) {
                                data[ii] += scale*chol[ii*dim + jj]*unit[jj];
                            }
                            probe[ii] = center[ii] + steps[ii]*data[ii];
                        }
                        double score = sign*func(probe);
                        if (isinf(score) == +1) continue;
                        if (isinf(score) == - 1 || isnan(score)) {
                            for (int64 ii = 0; ii<dim; ii++) {
                                center[ii] = probe[ii];
                            }
                            return score;
                        }
                        pop[pp].score = score;
                        break;
                    }
                    improved += pop[pp].score < pop[0].score;
                }

                introsort(pop, parents + children);

                // averaging our new covariance with the old
                const double learnrate = .2;
                const double oldweight = ::sqrt(1 - learnrate);
                const double newweight = ::sqrt(learnrate)/scale;

                // shrink the old covariance
                for (int64 ii = 0; ii<dim; ii++) {
                    for (int64 jj = 0; jj<=ii; jj++) {
                        chol[ii*dim + jj] *= oldweight;
                    }
                }
                for (int64 ii = 0; ii<dim; ii++) {
                    for (int64 jj = 0;jj<dim; jj++) {
                        fprintf(stderr, "\t%+.12lf", chol[ii*dim + jj]);
                    }
                    fprintf(stderr, "\n");
                }

                // add in the new contributions
                for (int64 pp = 0; pp<parents; pp++) {
                    double vec[dim];
                    for (int64 ii = 0; ii<dim; ii++) {
                        vec[ii] = (pop[pp].data[ii] - mean[ii])*newweight;
                    }
                    cholupdate(chol, +1, vec, dim);
                }
                cholunitize(chol, dim);

                /*
                if (improved == 0) {
                    // average back towards a sphere
                    const double aa = ::sqrt(.99);
                    const double bb = ::sqrt(.01);
                    for (int64 ii = 0; ii<dim; ii++) {
                        for (int64 jj = 0; jj<=ii; jj++) {
                            chol[ii*dim + jj] *= aa;
                        }
                        chol[ii*dim + ii] += bb;
                    }
                }
                */

                // trying to maintain the one-fifth rule
                scale = min(1, scale*::exp(
                    (improved - .2*(parents + children - previous))/3)
                );
                if (scale < 1e-50) scale = 1; // reset condition

                // update the new mean
                double meanscale = 1.0/parents;
                for (int64 ii = 0; ii<dim; ii++) {
                    mean[ii] = 0;
                }
                for (int64 pp = 0; pp<parents; pp++) {
                    for (int64 ii = 0; ii<dim; ii++) {
                        mean[ii] += meanscale*pop[pp].data[ii];
                    }
                }

                its -= parents + children - previous;
                previous = parents;
            }

            // center[ii] = our best answer
            for (int64 ii = 0; ii<dim; ii++) {
                center[ii] += steps[ii]*pop[0].data[ii];
            }
            
            // steps[ii] = bounding ellipsoid of the covariance
            for (int64 ii = 0; ii<dim; ii++) {
                double norm = 0;
                for (int64 jj = 0; jj<=ii; jj++) {
                    norm = ::hypot(norm, chol[ii*dim + jj]);
                }
                steps[ii] = scale*norm;
            }

            return pop[0].score;
        }
    }

    template<class cost>
    double covarmin(
        cost func, double* center, double* steps,
        int64 dim, int64 its, uint64_t seed=0
    ) {
        return internal::covaropt<cost, +1>(
            func, center, steps, dim, its, seed
        );
    }

    template<class cost>
    double covarmax(
        cost func, double* center, double* steps,
        int64 dim, int64 its, uint64_t seed=0
    ) {
        return internal::covaropt<cost, -1>(
            func, center, steps, dim, its, seed
        );
    }


    //}}}
}



template<int dim>
double cigar(double* args) {
    using namespace xm;
    double sum = sqr(args[0]);
    for (int ii = 1; ii<dim; ii++) {
        sum += 1e3*sqr(args[ii]);
    }

    if (1) {
        for (int ii = 0; ii<dim; ii++) {
            fprintf(stderr, "%.18lf\t", args[ii]);
        }
        fprintf(stderr, ": %.18lf\n", sum);

        static int64 count;
        static double costs[10000];
        costs[count++] = sum;

        static FILE* gnuplot;
        if (!gnuplot) {
            gnuplot = popen("gnuplot -persist", "w");
            fprintf(gnuplot, "set xrange [0:5000]\n");
            fprintf(gnuplot, "set yrange [-15:15]\n");
            fprintf(gnuplot, "set grid\n");
        }
        fprintf(gnuplot, "plot '-' with lines\n");
        for (int64 ii = 0; ii<count; ii++) {
            fprintf(gnuplot, "%lld %.18le\n", ii, ::log10(costs[ii]));
        }
        fprintf(gnuplot, "e\n");
        fflush(gnuplot);
    }

    return sum;
}

int main() {
    using namespace xm;

    double center[4] = { 100, 100, 100, 100 };
    double steps[4] = { 100, 100, 100, 100 };
    covarmin(cigar<4>, center, steps, 4, 10000);

    //double center[2] = { 0, 0 };
    //double steps[2] = { 10, 10 };
    //covarmin(sphere, center, steps, 2, 10000);

    return 0;
}

