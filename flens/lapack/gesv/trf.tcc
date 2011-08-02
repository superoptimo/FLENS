/*
 *   Copyright (c) 2011, Iris Haecker
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

/*
 *   Copyright (c) 1992-2007 The University of Tennessee.  All rights reserved.
 *   
 *   $COPYRIGHT$
 *   
 *   Additional copyrights may follow
 *   
 *   $HEADER$
 *   
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are
 *   met:
 *   
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 *     
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer listed
 *     in this license in the documentation and/or other materials
 *     provided with the distribution.
 *     
 *   - Neither the name of the copyright holders nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
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

#ifndef FLENS_LAPACK_GESV_TRF_TCC
#define FLENS_LAPACK_GESV_TRF_TCC 1

#include <algorithm>
#include <flens/blas/blas.h>

namespace cxxlapack {

//-- getf2 ---------------------------------------------------------------------
template <typename MA, typename VP>
typename GeMatrix<MA>::IndexType
trf(GeMatrix<MA> &A, DenseVector<VP> &iPiv);
{
    typedef typename GeMatrix<MA>::IndexType IndexType;
    typedef typename GeMatrix<MA>::ElementType  T;

    typedef Range<IndexType>    Range;
    const Underscore<IndexType> _;

    const IndexType m = A.numRows();
    const IndexType n = A.numCols();

    IndexType info = 0;

//
//  Quick return if possible
//
    if ((m==0) || (n==0)) {
        return info;
    }
//
//  Determine the block size for this environment.
//
    IndexType bs = 2; // TODO: laenv( 1, 'DGETRF', ' ', M, N, -1, -1 );
    if ((bs<=1) || (bs>=std::min(m,n))) {
//
//      Use unblocked code.
//
        info = tf2(A, iPiv);
    } else {
//
//      Use blocked code.
//
        for (IndexType j=1; j<=std::min(m,n); j+=bs) {
            IndexType jb = std::min(std::min(m,n)-j+1, bs);

            const Range colsDone        = _(1,j-1);
            const Range cols            = _(j,j+jb-1);
            const Range colsTrailing    = _(j+jb,n);

            const Range rows            = _(j,m);

//
//          Factor diagonal and subdiagonal blocks and test for exact
//          singularity.
//
            IndexType _info = tf2(A(rows,cols), iPiv(rows));
//
//          Adjust INFO and the pivot indices.
//
            if ((info==0) && (_info>0)) {
                info = _info + j - 1;
            }
            for (IndexType i=j; i<min(m,j+jb); ++i) {
                iPiv[i] += j;
            }
//
//          Apply interchanges to columns 1:J-1.
//
            laswp(order, j, A, ldA, j, j+jb-1, iPiv, IndexType(1));

            if (j+jb<n) {
//
//              Apply interchanges to columns J+JB:N.
//
                laswp(order, n-j-jb+1, A+(j+jb)*ldA, ldA,
                             j, j+jb-1, iPiv, IndexType(1));
//
//              Compute block row of U.
//
                trsm(order, Left, Lower, NoTrans, Unit,
                     jb, n-j-jb,
                     MA(1),
                     A + j*(ldA+1), ldA,
                     A + j+(j+jb)*ldA, ldA);

                if (j+jb<m) {
//
//                  Update trailing submatrix.
//
                    gemm(order, NoTrans, NoTrans,
                         m-j-jb, n-j-jb, jb,
                         MA(-1),
                         A+j*(ldA+1)+jb, ldA,
                         A+j+(j+jb)*ldA, ldA,
                         MA(1),
                         A+(j+jb)*(ldA+1), ldA);
                }
            }
        }
    }
    return info;
}

} // namespace cxxlapack

#endif // FLENS_LAPACK_GESV_TRF_TCC