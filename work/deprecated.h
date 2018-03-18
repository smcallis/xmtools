namespace xm {

    //{{{ Deprecated Functions and Types 
    namespace deprecated {
        //{{{ qrsolve
        //
        // Solve A*X = B for X using QR decomposition
        //
        //    double A[rows * cols]
        //    double B[rows * count]
        //
        // Returns false is A is singular
        //
        // A will be modified during the decomposition
        //
        // If successful, the result will be placed into the top
        // 'cols' rows of B.  B will also be modified on failure
        //
        static inline bool qrsolve (
            double* A, int64 rows, int64 cols,
            double* B, int64 count
        ) {
            check(rows >= cols, "rows %lld must be >= cols %lld", rows, cols);

            for (int64 jj = 0; jj<cols; jj++) {
                double rr = 0;
                for (int64 ii = jj; ii<rows; ii++) {
                    rr = hypot(rr, A[ii*cols + jj]);
                }
                // check if A is singular
                if (rr == 0) return false;

                // for numerical stability
                if (A[jj*cols + jj] > 0) rr = -rr;

                // normalize the Householder vector
                double invrr = 1.0/rr;
                for (int64 ii = jj; ii<rows; ii++) {
                    A[ii*cols + jj] *= invrr;
                }
                double pp = A[jj*cols + jj] -= 1;
                double invpp = 1.0/pp;

                // apply the Householder to the rest of A
                for (int64 kk = jj + 1; kk<cols; kk++) {
                    double sum = 0;
                    for (int64 ii = jj; ii<rows; ii++) {
                        sum += A[ii*cols + jj]*A[ii*cols + kk];
                    }
                    sum *= invpp;
                    for (int64 ii = jj; ii<rows; ii++) {
                        A[ii*cols + kk] += sum*A[ii*cols + jj];
                    }
                }

                // apply the Householder to all of B
                for (int64 kk = 0; kk<count; kk++) {
                    double sum = 0;
                    for (int64 ii = jj; ii<rows; ii++) {
                        sum += A[ii*cols + jj]*B[ii*count + kk];
                    }
                    sum *= invpp;
                    for (int64 ii = jj; ii<rows; ii++) {
                        B[ii*count + kk] += sum*A[ii*cols + jj];
                    }
                }

                A[jj*cols + jj] = rr;
            }

            // back substitute R into B
            for (int64 jj = cols - 1; jj >= 0; jj--) {
                // divide by diagonal element
                double dd = A[jj*cols + jj];
                double invdd = 1.0/dd;
                for (int64 kk = 0; kk<count; kk++) {
                    B[jj*count + kk] *= invdd;
                }

                // subtract from other rows
                for (int64 ii = jj - 1; ii >= 0; ii--) {
                    double ss = A[ii*cols + jj];
                    for (int64 kk = 0; kk<count; kk++) {
                        B[ii*count + kk] -= ss*B[jj*count + kk];
                    }
                }
            }

            return true;
        }

        // Note for the next several wrappers: we intentionally take A and B by value
        // so that copies will be made.  The worker routines will modify these copies.
        static inline matrix<double> qrsolve(matrix<double> A, matrix<double> B) {
            check(A.rows() >= A.cols(), "can't be underdetermined");
            check(A.rows() == B.rows(), "need compatible sizes");
            check(
                qrsolve(A.data(), A.rows(), A.cols(), B.data(), B.cols()),
                "matrix must not be singular"
            );
            matrix<double> X(A.cols(), B.cols());
            for (int64 ii = 0; ii<X.rows(); ii++) {
                for (int64 jj = 0; jj<X.cols(); jj++) {
                    X(ii, jj) = B(ii, jj);
                }
            }
            return X;
        }

        static inline vector<double> qrsolve(matrix<double> A, vector<double> b) {
            check(A.rows() >= A.cols(), "can't be underdetermined");
            check(A.rows() == b.size(), "need compatible sizes");
            check(
                qrsolve(A.data(), A.rows(), A.cols(), b.data(), 1),
                "matrix must not be singular"
            );
            vector<double> x(A.cols());
            for (int64 ii = 0; ii<x.size(); ii++) {
                x[ii] = b[ii];
            }
            return x;
        }
        //}}}
        //{{{ ldsolve
        //
        // Factor A using LDL^t decomposition.
        //
        // The matrix must be symmetric, but this is not checked.  The decomp is
        // placed in the lower half of the A matrix.  This decomp is similar to a
        // Cholesky decomp, but it does not require the matrix to be positive
        // definite, only symmetric.  This function always succeeds, but the decomp
        // is only unique when the matrix is non-singular.
        //
        static inline void ldfactor(double* A, int64 size) {
            for (int64 jj = 0; jj < size; jj++) {
                // Solve for Diagonal
                double ajj = A[jj*size + jj];
                for (int64 kk = 0; kk < jj; kk++) {
                    double dkk = A[kk*size + kk];
                    double ljk = A[jj*size + kk];
                    ajj -= ljk*(ljk)*dkk;
                }
                A[jj*size + jj] = ajj;
                if (ajj == 0) ajj = 1;

                // Solve for Lowers
                for (int64 ii = jj + 1; ii < size; ii++) {
                    double aij = A[ii*size + jj];
                    for (int64 kk = 0; kk < jj; kk++) {
                        double lik = A[ii*size + kk];
                        double ljk = A[jj*size + kk];
                        double dkk = A[kk*size + kk];
                        aij -= lik*(ljk)*dkk;
                    }
                    A[ii*size + jj] = aij/ajj;
                }
            }
        }

        //
        // Check if the LD decomposition is positive definite.
        //
        static inline bool ldposdef(const double* LD, int64 size) {
            for (int64 ii = 0; ii<size; ii++) {
                if (LD[ii*size + ii] <= 0) return false;
            }
            return true;
        }

        //
        // Use the LDL^t decomposition in A to solve A*X = B.  X = A\B.
        //
        //     double LD[size * size]
        //     double B[size * count]
        //
        // This function fails when the decomposition is singular (zero on diagonal)
        //
        static inline bool lddivide(const double* LD, int64 size, double* B, int64 count) {
            for (int64 ii = 0; ii<size; ii++) {
                if (LD[ii*size + ii] == 0) return false;
            }
            // Invert the Lower
            for (int64 jj = 0; jj < size - 1; jj++) {
                for (int64 ii = jj + 1; ii < size; ii++) {
                    for (int64 kk = 0; kk<count; kk++) {
                        B[ii*count + kk] -= LD[ii*size + jj]*B[jj*count + kk];
                    }
                }
            }
            // Invert the Diagonal
            for (int64 ii = 0; ii < size; ii++) {
                for (int64 kk = 0; kk<count; kk++) {
                    B[ii*count + kk] /= LD[ii*size + ii];
                }
            }
            // Invert the Lower Transpose
            for (int64 ii = size - 1; ii > 0; ii--) {
                for (int64 jj = 0; jj < ii; jj++) {
                    for (int64 kk = 0; kk<count; kk++) {
                        B[jj*count + kk] -= LD[ii*size + jj]*B[ii*count + kk];
                    }
                }
            }
            return true;
        }

        //
        // Solve A*X = B for X using LDL^t decomposition
        //
        //     double A[size * size]
        //     double B[size * count]
        //
        // Modifies A, places the result in B.  Returns false if A is singular.
        //
        static inline bool ldsolve(double* A, int64 size, double* B, int64 count) {
            ldfactor(A, size);
            return lddivide(A, size, B, count);
        }

        static inline matrix<double> ldsolve(matrix<double> A, matrix<double> B) {
            check(A.rows() == A.cols(), "must be a square matrix");
            check(A.rows() == B.rows(), "need compatible sizes");
            check(
                ldsolve(A.data(), A.rows(), B.data(), B.cols()),
                "matrix must not be singular"
            );
            matrix<double> X(A.cols(), B.cols());
            for (int64 ii = 0; ii<X.rows(); ii++) {
                for (int64 jj = 0; jj<X.cols(); jj++) {
                    X(ii, jj) = B(ii, jj);
                }
            }
            return X;
        }

        static inline vector<double> ldsolve(matrix<double> A, vector<double> b) {
            check(A.rows() == A.cols(), "must be a square matrix");
            check(A.rows() == b.size(), "need compatible sizes");
            check(
                ldsolve(A.data(), A.rows(), b.data(), 1),
                "matrix must not be singular"
            );
            vector<double> x(A.cols());
            for (int64 ii = 0; ii<x.size(); ii++) {
                x[ii] = b[ii];
            }
            return x;
        }

        //}}}
    //{{{ SVD 2x2

    template<class type>
    struct svd2x2t {
        type L[2][2];
        double S[2];
        type R[2][2];
    };

    typedef svd2x2t<double> rsvd2x2;
    typedef svd2x2t<cdouble> csvd2x2;

    namespace internal {
        static inline rsvd2x2 diagonalize (double ff, double gg, double hh) {
            // Diagonalize a 2x2 real valued lower bi-diagonal matrix.
            //
            // We can always factor an arbitrary 2x2 real or complex matrix
            // into this special case by using Householder (or Givens)
            // rotations as shown in the functions below.
            //
            // This algorithm is loosely taken from an article titled,
            // "Accuracy of two SVD algorithms for 2x2 triangular matrices"
            // published in Applied Mathematic and Computation in 2009.
            //
            // We want to find two rotation matrices such that:
            //
            //    [ff  0] = [Lc -Ls] [S0 0] [Rc -Rs]
            //    [gg hh]   [Ls  Lc] [0 S1] [Rs  Rc]
            //
            //    Where Lc = cos(L), Ls = sin(L), Rc = cos(R), Rs = sin(R)
            //
            // Transposing to invert and rearranging:
            //
            //    [ Lc Ls] [ff  0] [ Rc Rs] = [S0 0]
            //    [-Ls Lc] [gg hh] [-Rs Rc]   [0 S1]
            //
            // Expanding the left:
            //
            //    [( Lc*ff + Ls*gg) Ls*hh] [ Rc Rs] = [S0 0]
            //    [(-Ls*ff + Lc*gg) Lc*hh] [-Rs Rc]   [0 S1]
            //
            // Expanding and separating:
            //
            //    S0 =  Lc*Rc*ff + Ls*Rc*gg - Ls*Rs*hh
            //     0 =  Lc*Rs*ff + Ls*Rs*gg + Ls*Rc*hh
            //     0 = -Ls*Rc*ff + Lc*Rc*gg - Lc*Rs*hh
            //    S1 = -Ls*Rs*ff + Lc*Rs*gg + Lc*Rc*hh
            //
            ///////////////// First Method //////////////////////////
            //
            // Factoring the two zero equations for left cos and sin:
            //
            //    [Rs*ff  (Rs*gg + Rc*hh)] [Lc] = [0]
            //    [(Rc*gg - Rs*hh) -Rc*ff] [Ls]   [0]
            //
            // Since [Lc Ls] can't be zero, the determinant must be zero:
            //
            //    0 = -Rc*Rs*ff^2 - (Rc*Rs*gg^2 - Rs^2*gg*hh + Rc^2*gg*hh - Rc*Rs*hh^2)
            //      = Rc*Rs*ff^2 + Rc*Rs*gg^2 - Rs^2*gg*hh + Rc^2*gg*hh - Rc*Rs*hh^2
            //      = Rc*Rs*(ff^2 + gg^2 - hh^2) + (Rc^2 - Rs^2)*gg*hh
            //
            // We're going to use the following double-angle formulas:
            //
            //    sin(2x) = 2*cos(x)*sin(x)
            //    cos(2x) = cos(x) - sin(x)
            //
            // In:
            //
            //    0 = 2*Rs*CS*(ff^2 + gg^2 - hh^2) + 2*(Rc^2 - Rs^2)*gg*hh
            //      = sin(2R)*(ff^2 + gg^2 - hh^2) + 2*cos(2R)*gg*hh
            //
            // Rearranging, we get our first method which solves for the right angle:
            //
            //    sin(2R)*(ff^2 + gg^2 - hh^2) = -2*cos(2R)*gg*hh
            //    tan(2R) = (-2*gg*hh) / (ff^2 + gg^2 - hh^2)
            //
            // Now that we have a value for R, we can solve for L in two ways:
            //
            //    0 =  Lc*Rs*ff + Ls*Rs*gg + Ls*Rc*hh
            //    Ls*(Rs*gg + Rc*hh) = -Lc*(Rs*ff)
            //    tan(L) = -(Rs*ff) / (Rs*gg + Rc*hh)
            //
            // Or:
            //
            //    0 = -Ls*Rc*ff + Lc*Rc*gg - Lc*Rs*hh
            //    Ls*(Rc*ff) = Lc*(Rc*gg - Rs*hh)
            //    tan(L) = (Rc*gg - Rs*hh) / (Rc*ff)
            //
            ////////////////// Second Method ////////////////////////
            //
            // We can also factor those two equations for the right cos and sin:
            //
            //    0 =  Lc*Rs*ff + Ls*Rs*gg + Ls*Rc*hh
            //    0 = -Ls*Rc*ff + Lc*Rc*gg - Lc*Rs*hh
            //
            //    [Ls*hh   (Lc*ff + Ls*gg)] [Rc] = [0]
            //    [(-Ls*ff + Lc*gg) -Lc*hh] [Rs]   [0]
            //
            // Again, the determinant must be zero and using the double angle formulas:
            //
            //    0 = -Lc*Ls*hh^2 - (-Lc*Ls*ff^2 + Lc^2*ff*gg - Ls^2*ff*gg + Lc*Ls*gg^2)
            //      = Lc*Ls*hh^2 - Lc*Ls*ff^2 + Lc^2*ff*gg - Ls^2*ff*gg + Lc*Ls*gg^2
            //      = Lc*Ls*(-ff^2 + gg^2 + hh^2) + (Lc^2 - Ls^2)*ff*gg
            //      = sin(2*L)*(-ff^2 + gg^2 + hh^2) + cos(2L)*ff*gg
            //
            // Again rearranging, we get our second method for the left angle:
            //
            //    2*sin(2L)*(-ff^2 + gg^2 + hh^2) = -2*cos(2L)*ff*gg
            //    tan(2L) = (-2*ff*gg) / (-ff^2 + gg^2 + hh^2)
            //
            // This time we have a value for L, so we can solve for R in two ways:
            //
            //    0 =  Lc*Rs*ff + Ls*Rs*gg + Ls*Rc*hh
            //    Rs*(Lc*ff + Ls*gg) = -Rc*(Ls*hh)
            //    tan(R) = -Ls*hh / (Lc*ff + Ls*gg)
            //
            //    0 = -Ls*Rc*ff + Lc*Rc*gg - Lc*Rs*hh
            //    Rs*(Lc*hh) = Rc*(-Ls*ff + Lc*gg)
            //    tan(R) = (Lc*gg - Ls*ff) / Lc*hh
            //
            //////////////////////////////////////////////////////////
            //
            // So we have two methods - one which solves for L and one for R.
            // Each of those two methods has two sub-methods to solve for the
            // missing piece.  We choose which ones to use based on trying to
            // avoid catastrophic cancellation in the operands.
            //

            // Check if it's already diagonal
            if (gg == 0) return (rsvd2x2){
                {{ 1, 0 }, { 0, 1 }}, { ff, hh }, {{ 1, 0 }, { 0, 1 }}
            };

            if (ff == 0) { // shortcut
                double rr = hypot(gg, hh);
                double cc = hh/rr;
                double ss = gg/rr;
                return (rsvd2x2){
                    {{ 1, 0 }, { 0, 1 }}, { 0, rr }, {{ cc, -ss }, { ss, cc }}
                };
            }

            if (hh == 0) { // shortcut
                double rr = hypot(ff, gg);
                double cc = ff/rr;
                double ss = gg/rr;
                return (rsvd2x2){
                    {{ cc, -ss }, { ss, cc }}, { rr, 0 }, {{ 1, 0 }, { 0, 1 }}
                };
            }

            double f2 = ff*ff;
            double g2 = gg*gg;
            double h2 = hh*hh;

            double Lc, Ls, Rc, Rs;
            if (f2 > h2) {
                double R = .5*atan2(-2*gg*hh, f2 + g2 - h2);
                Rc = cos(R);
                Rs = sin(R);
                double L;
                if (fabs(Rs*gg + Rc*hh) > fabs(Rc*gg - Rs*hh)) {
                    L = atan2(-Rs*ff, Rs*gg + Rc*hh);
                } else {
                    L = atan2(Rc*gg - Rs*hh, Rc*ff);
                }
                Lc = cos(L);
                Ls = sin(L);
            } else {
                double L = .5*atan2(-2*ff*gg, h2 + g2 - f2);
                Lc = cos(L);
                Ls = sin(L);
                double R;
                if (fabs(Lc*ff + Ls*gg) > fabs(Lc*gg - Ls*ff)) {
                    R = atan2(-Ls*hh, Lc*ff + Ls*gg);
                } else {
                    R = atan2(Lc*gg - Ls*ff, Lc*hh);
                }
                Rc = cos(R);
                Rs = sin(R);
            }

            double S0 =  Lc*Rc*ff + Ls*Rc*gg - Ls*Rs*hh;
            double S1 = -Ls*Rs*ff + Lc*Rs*gg + Lc*Rc*hh;

            return (rsvd2x2) {
                {{ Lc, -Ls }, { Ls, Lc }}, { S0, S1 }, {{ Rc, -Rs }, { Rs, Rc }}
            };
        }
    }

    /* XXX
    template<class type>
    static inline std::ostream& operator <<(std::ostream& os, const svd2x2t<type>& s) {
        os << std::setprecision(5);
        os << "L: [[" << s.L[0][0] << ", " << s.L[0][1] << "][" << s.L[1][0] << ", " << s.L[1][1] << "]]" << std::endl;
        os << "S: [" << s.S[0] << "; " << s.S[1] << "]" << std::endl;
        os << "R: [[" << s.R[0][0] << ", " << s.R[0][1] << "][" << s.R[1][0] << ", " << s.R[1][1] << "]]" << std::endl;

        return os;
    }
    */

    static inline rsvd2x2 bsvd2x2 (double ff, double gg, double hh) {
        using namespace internal;

        // Calculate the SVD of a 2x2 real lower bidiagonal matrix
        // Permute and negate the bidiagonal result if needed
        rsvd2x2 svd = diagonalize(ff, gg, hh);
        if (svd.S[0] < 0) {
            svd.L[0][0] *= -1;
            svd.L[1][0] *= -1;
            svd.S[0] *= -1;
        }

        if (svd.S[1] < 0) {
            svd.L[0][1] *= -1;
            svd.L[1][1] *= -1;
            svd.S[1] *= -1;
        }

        if (svd.S[0] < svd.S[1]) {
            swap(svd.L[0][0], svd.L[0][1]);
            swap(svd.L[1][0], svd.L[1][1]);
            swap(svd.S[0], svd.S[1]);
            swap(svd.R[0][0], svd.R[1][0]);
            swap(svd.R[0][1], svd.R[1][1]);
        }

        return svd;
    }


    static inline rsvd2x2 svd2x2 (
        double tt, double uu,
        double vv, double ww
    ) {
        if (uu) {
            double ff = hypot(tt, uu);
            double cc = +tt/ff;
            double ss = -uu/ff;
            double gg = vv*cc - ww*ss;
            double hh = vv*ss + ww*cc;
            rsvd2x2 svd = bsvd2x2(ff, gg, hh);
            double r00 = +svd.R[0][0]*cc + svd.R[0][1]*ss;
            double r01 = -svd.R[0][0]*ss + svd.R[0][1]*cc;
            double r10 = +svd.R[1][0]*cc + svd.R[1][1]*ss;
            double r11 = -svd.R[1][0]*ss + svd.R[1][1]*cc;
            svd.R[0][0] = r00; svd.R[0][1] = r01;
            svd.R[1][0] = r10; svd.R[1][1] = r11;
            return svd;
        }
        // it was already bidiagonal
        return bsvd2x2(tt, vv, ww);
    }

    static inline csvd2x2 svd2x2 (
        cdouble tt, cdouble uu,
        cdouble vv, cdouble ww
    ) {
        double magt = hypot(tt.re, tt.im);
        double magu = hypot(uu.re, uu.im);
        double magx = hypot(magt, magu);
        double angt = atan2(tt.im, tt.re);
        double angu = atan2(uu.im, uu.re);
        double angx = atan2(magu, magt);
        double cosx = cos(angx);
        double cost = cos(angt);
        double cosu = cos(angu);
        double sinx = sin(angx);
        double sint = sin(angt);
        double sinu = sin(angu);

        cdouble R00 = +cosx*cdouble(cost, sint);
        cdouble R01 = +sinx*cdouble(cosu, sinu);
        cdouble R10 = -sinx*cdouble(cost, sint);
        cdouble R11 = +cosx*cdouble(cosu, sinu);

        cdouble C10 = vv*conj(R00) + ww*conj(R01);
        cdouble C11 = vv*conj(R10) + ww*conj(R11);

        double angl = atan2(C10.im, C10.re);
        cdouble zl = cdouble(cos(angl), sin(angl));
        cdouble L00 = cdouble(1, 0);
        cdouble L01 = cdouble(0, 0);
        cdouble L10 = cdouble(0, 0);
        cdouble L11 = zl;//cdouble(1, 0);
        C10 = C10*conj(zl);
        C11 = C11*conj(zl);

        double angr = atan2(C11.im, C11.re);
        cdouble zr = cdouble(cos(angr), sin(angr));
        R10 = R10*zr;
        R11 = R11*zr;

        double ff = magx;
        double gg = hypot(C10.re, C10.im);
        double hh = hypot(C11.re, C11.im);
        rsvd2x2 svd= bsvd2x2(ff, gg, hh);

        return (csvd2x2){
            {{svd.L[0][0]*L00 + svd.L[1][0]*L01, svd.L[0][1]*L00 + svd.L[1][1]*L01},
             {
                (svd.L[0][0])*L10 + (svd.L[1][0])*L11,
                (svd.L[0][1])*L10 + (svd.L[1][1])*L11
            }},
            { svd.S[0], svd.S[1] },
            {{
                (svd.R[0][0])*R00 + (svd.R[0][1])*R10,
                (svd.R[0][0])*R01 + (svd.R[0][1])*R11
            },{
                (svd.R[1][0])*R00 + (svd.R[1][1])*R10,
                (svd.R[1][0])*R01 + (svd.R[1][1])*R11
            }}
        };
    }
    
    //}}}
    //{{{ SIMD
 
#define define_simdtype(NAME, BASE, COUNT) \
    typedef BASE NAME __attribute__(       \
        (vector_size (COUNT*sizeof(BASE))) \
    )

    // 64 bit SIMD (MMX)
    define_simdtype( i8x8,    int8_t,  8);
    define_simdtype(i16x4,   int16_t,  4);
    define_simdtype(i32x2,   int32_t,  2);
    define_simdtype( u8x8,   uint8_t,  8);
    define_simdtype(u16x4,  uint16_t,  4);
    define_simdtype(u32x2,  uint32_t,  2);
    define_simdtype(f32x2,     float,  2);
 
    // 128 bit SIMD (SSE)
    define_simdtype(i8x16,    int8_t, 16);
    define_simdtype(i16x8,   int16_t,  8);
    define_simdtype(i32x4,   int32_t,  4);
    define_simdtype(i64x2,   int64_t,  2);
    define_simdtype(u8x16,   uint8_t, 16);
    define_simdtype(u16x8,  uint16_t,  8);
    define_simdtype(u32x4,  uint32_t,  4);
    define_simdtype(u64x2,  uint64_t,  2);
    define_simdtype(f32x4,     float,  4);
    define_simdtype(f64x2,    double,  2);

    // 256 bit SIMD (AVX)
    define_simdtype( i8x32,   int8_t, 32);
    define_simdtype(i16x16,  int16_t, 16);
    define_simdtype( i32x8,  int32_t,  8);
    define_simdtype( i64x4,  int64_t,  4);
    define_simdtype( u8x32,  uint8_t, 32);
    define_simdtype(u16x16, uint16_t, 16);
    define_simdtype( u32x8, uint32_t,  8);
    define_simdtype( u64x4, uint64_t,  4);
    define_simdtype( f32x8,    float,  8);
    define_simdtype( f64x4,   double,  4);

    // 512 bit SIMD (AVX-512)
    define_simdtype( i8x64,   int8_t, 64);
    define_simdtype(i16x32,  int16_t, 32);
    define_simdtype(i32x16,  int32_t, 16);
    define_simdtype( i64x8,  int64_t,  8);
    define_simdtype( u8x64,  uint8_t, 64);
    define_simdtype(u16x32, uint16_t, 32);
    define_simdtype(u32x16, uint32_t, 16);
    define_simdtype( u64x8, uint64_t,  8);
    define_simdtype(f32x16,    float, 16);
    define_simdtype( f64x8,   double,  8);

    // 1024 bit SIMD (Future??)
    define_simdtype(i8x128,   int8_t, 128);
    define_simdtype(i16x64,  int16_t,  64);
    define_simdtype(i32x32,  int32_t,  32);
    define_simdtype(i64x16,  int64_t,  16);
    define_simdtype(u8x128,  uint8_t, 128);
    define_simdtype(u16x64, uint16_t,  64);
    define_simdtype(u32x32, uint32_t,  32);
    define_simdtype(u64x16, uint64_t,  16);
    define_simdtype(f32x32,    float,  32);
    define_simdtype(f64x16,   double,  16);

#undef define_simdtype

    //}}}
    }
    //}}}

}
