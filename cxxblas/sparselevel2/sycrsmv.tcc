/*
 *   Copyright (c) 2007-2012, Michael Lehn
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CXXBLAS_SPARSELEVEL2_SYCRSMV_TCC
#define CXXBLAS_SPARSELEVEL2_SYCRSMV_TCC 1

#include <cxxblas/typedefs.h>

#define HAVE_CXXBLAS_SYCRSMV 1

namespace cxxblas {

template <typename IndexType, typename ALPHA, typename MA, typename VX,
          typename BETA, typename VY>
void
sycrsmv(StorageUpLo      upLo,
        IndexType        m,
        const ALPHA      &alpha,
        const MA         *A,
        const IndexType  *ia,
        const IndexType  *ja,
        const VX         *x,
        const BETA       &beta,
        VY               *y)
{
    if (beta==BETA(0)) {
        for (int i=1; i<=m; ++i) {
            y[i] = 0;
        }
    } else if (beta!=BETA(1)) {
        for (int i=1; i<=m; ++i) {
            y[i] *= beta;
        }
    }

    if (upLo==Upper) {
        for (int i=1; i<=m; ++i) {
            //ASSERT(ja[ia[i]]==i);
            for (int k=ia[i]; k<ia[i+1]; ++k) {
                y[i] += alpha*A[k]*x[ja[k]];
                if (ja[k]!=i) {
                    y[ja[k]] += alpha*A[k]*x[i];
                }
            }
        }
    } else {
        for (int i=1; i<=m; ++i) {
            for (int k=ia[i]; k<ia[i+1]-1; ++k) {
                y[i] += alpha*A[k]*x[ja[k]];
                y[ja[k]] += alpha*A[k]*x[i];
            }
            ASSERT(ja[ia[i+1]-1]==i);
            y[i] += alpha*A[ia[i+1]-1]*x[i];
        }
    }
}

} // namespace cxxblas

#endif // CXXBLAS_SPARSELEVEL2_SYCRSMV_TCC