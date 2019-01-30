#include "inverse.h"

#pragma warning(disable: 4996)

static integer c__0 = 0;
static real c_b163 = 0.f;
static real c_b164 = 1.f;
static integer c__1 = 1;


const char *f__r_mode[2] = { "rb", "r" };
const char *f__w_mode[4] = { "wb", "w", "r+b", "r+" };

char L_buffer[10] = "L";
char U_buffer[10] = "U";
char CHEMV_buffer[10] = "CHEMV";
char CHETRF_buffer[10] = "CHETRF";
char CHETF2_buffer[10] = "CHETF2";
char CHETRI_buffer[10] = "CHETRI";
char GE_buffer[10] = "GE";
char TRF_buffer[10] = "TRF";
char TRD_buffer[10] = "TRD";
char BRD_buffer[10] = "BRD";
char HRD_buffer[10] = "HRD";
char QRF_buffer[10] = "QRF";
char QLF_buffer[10] = "QLF";
char RQF_buffer[10] = "RQF";
char LQF_buffer[10] = "LQF";
char TRI_buffer[10] = "TRI";
char SY_buffer[10] = "SY";
char HE_buffer[10] = "HE";
char PO_buffer[10] = "PO";
char TR_buffer[10] = "TR";
char QR_buffer[10] = "QR";
char RQ_buffer[10] = "RQ";
char OR_buffer[10] = "OR";
char LQ_buffer[10] = "LQ";
char HR_buffer[10] = "HR";
char QL_buffer[10] = "QL";
char BR_buffer[10] = "BR";
char EBZ_buffer[10] = "EBZ";
char LA_buffer[10] = "LA";
char GST_buffer[10] = "GST";
char GB_buffer[10] = "GB";
char PB_buffer[10] = "PB";
char UUM_buffer[10] = "UUM";
char ST_buffer[10] = "ST";
char N_buffer[10] = "N";
char T_buffer[10] = "T";
char C_buffer[10] = "C";
char UN_buffer[10] = "UN";
char CGEMV1_buffer[10] = "CGEMV ";
char complexdivisionbyzero_buffer[100] = "complex division by zero";
char CGEMM_buffer[10] = "CGEMM";
char CHER1_buffer[10] = "CHER  ";
char Notranspose_buffer[20] = "No transpose";
char Transpose_buffer[20] = "Transpose";
          

unit f__units[MXUNIT];	/*unit table*/

int chetrf_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, complex *work, integer *lwork, integer *info)
{
	/*  -- LAPACK routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	CHETRF computes the factorization of a complex Hermitian matrix A
	using the Bunch-Kaufman diagonal pivoting method.  The form of the
	factorization is

	A = U*D*U**H  or  A = L*D*L**H

	where U (or L) is a product of permutation and unit upper (lower)
	triangular matrices, and D is Hermitian and block diagonal with
	1-by-1 and 2-by-2 diagonal blocks.

	This is the blocked version of the algorithm, calling Level 3 BLAS.

	Arguments
	=========

	UPLO    (input) CHARACTER*1
	= 'U':  Upper triangle of A is stored;
	= 'L':  Lower triangle of A is stored.

	N       (input) INTEGER
	The order of the matrix A.  N >= 0.

	A       (input/output) COMPLEX array, dimension (LDA,N)
	On entry, the Hermitian matrix A.  If UPLO = 'U', the leading
	N-by-N upper triangular part of A contains the upper
	triangular part of the matrix A, and the strictly lower
	triangular part of A is not referenced.  If UPLO = 'L', the
	leading N-by-N lower triangular part of A contains the lower
	triangular part of the matrix A, and the strictly upper
	triangular part of A is not referenced.

	On exit, the block diagonal matrix D and the multipliers used
	to obtain the factor U or L (see below for further details).

	LDA     (input) INTEGER
	The leading dimension of the array A.  LDA >= max(1,N).

	IPIV    (output) INTEGER array, dimension (N)
	Details of the interchanges and the block structure of D.
	If IPIV(k) > 0, then rows and columns k and IPIV(k) were
	interchanged and D(k,k) is a 1-by-1 diagonal block.
	If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and
	columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k)
	is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) =
	IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were
	interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block.

	WORK    (workspace/output) COMPLEX array, dimension (MAX(1,LWORK))
	On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

	LWORK   (input) INTEGER
	The length of WORK.  LWORK >=1.  For best performance
	LWORK >= N*NB, where NB is the block size returned by ILAENV.

	INFO    (output) INTEGER
	= 0:  successful exit
	< 0:  if INFO = -i, the i-th argument had an illegal value
	> 0:  if INFO = i, D(i,i) is exactly zero.  The factorization
	has been completed, but the block diagonal matrix D is
	exactly singular, and division by zero will occur if it
	is used to solve a system of equations.

	Further Details
	===============

	If UPLO = 'U', then A = U*D*U', where
	U = P(n)*U(n)* ... *P(k)U(k)* ...,
	i.e., U is a product of terms P(k)*U(k), where k decreases from n to
	1 in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1
	and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as
	defined by IPIV(k), and U(k) is a unit upper triangular matrix, such
	that if the diagonal block D(k) is of order s (s = 1 or 2), then

	(   I    v    0   )   k-s
	U(k) =  (   0    I    0   )   s
	(   0    0    I   )   n-k
	k-s   s   n-k

	If s = 1, D(k) overwrites A(k,k), and v overwrites A(1:k-1,k).
	If s = 2, the upper triangle of D(k) overwrites A(k-1,k-1), A(k-1,k),
	and A(k,k), and v overwrites A(1:k-2,k-1:k).

	If UPLO = 'L', then A = L*D*L', where
	L = P(1)*L(1)* ... *P(k)*L(k)* ...,
	i.e., L is a product of terms P(k)*L(k), where k increases from 1 to
	n in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1
	and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as
	defined by IPIV(k), and L(k) is a unit lower triangular matrix, such
	that if the diagonal block D(k) is of order s (s = 1 or 2), then

	(   I    0     0   )  k-1
	L(k) =  (   0    I     0   )  s
	(   0    v     I   )  n-k-s+1
	k-1   s  n-k-s+1

	If s = 1, D(k) overwrites A(k,k), and v overwrites A(k+1:n,k).
	If s = 2, the lower triangle of D(k) overwrites A(k,k), A(k+1,k),
	and A(k+1,k+1), and v overwrites A(k+2:n,k:k+1).

	=====================================================================


	Test the input parameters.

	Parameter adjustments */
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__2 = 2;

	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2;
	/* Local variables */
	static integer j, k, kb, nb, iws;
	static integer nbmin, iinfo;
	static logical upper;
	static integer ldwork, lwkopt;
	static logical lquery;


	a_dim1 = *lda;
	a_offset = 1 + a_dim1;
	a -= a_offset;
	--ipiv;
	--work;

	/* Function Body */
	*info = 0;
	upper = lsame_(uplo, U_buffer);
	lquery = *lwork == -1;
	if (!upper && !lsame_(uplo, L_buffer)) {
		*info = -1;
	}
	else if (*n < 0) {
		*info = -2;
	}
	else if (*lda < max(1, *n)) {
		*info = -4;
	}
	else if (*lwork < 1 && !lquery) {
		*info = -7;
	}

	if (*info == 0) {

		/*        Determine the block size */		
		nb = ilaenv_(&c__1, CHETRF_buffer, uplo, n, &c_n1, &c_n1, &c_n1, (ftnlen)6,
			(ftnlen)1);
		lwkopt = *n * nb;
		work[1].r = (real)lwkopt, work[1].i = 0.f;
	}

	if (*info != 0) {
		i__1 = -(*info);
		xerbla_(CHETRF_buffer, &i__1);
		return 0;
	}
	else if (lquery) {
		return 0;
	}

	nbmin = 2;
	ldwork = *n;
	if (nb > 1 && nb < *n) {
		iws = ldwork * nb;
		if (*lwork < iws) {
			/* Computing MAX */
			i__1 = *lwork / ldwork;
			nb = max(i__1, 1);
			/* Computing MAX */
			i__1 = 2, i__2 = ilaenv_(&c__2, CHETRF_buffer, uplo, n, &c_n1, &c_n1, &
				c_n1, (ftnlen)6, (ftnlen)1);
			nbmin = max(i__1, i__2);
		}
	}
	else {
		iws = 1;
	}
	if (nb < nbmin) {
		nb = *n;
	}

	if (upper) {

		/*        Factorize A as U*D*U' using the upper triangle of A

		K is the main loop index, decreasing from N to 1 in steps of
		KB, where KB is the number of columns factorized by CLAHEF;
		KB is either NB or NB-1, or K for the last block */

		k = *n;
	L10:

		/*        If K < 1, exit from loop */

		if (k < 1) {
			goto L40;
		}

		if (k > nb) {

			/*           Factorize columns k-kb+1:k of A and use blocked code to
			update columns 1:k-kb */

			clahef_(uplo, &k, &nb, &kb, &a[a_offset], lda, &ipiv[1], &work[1],
				n, &iinfo);
		}
		else {

			/*           Use unblocked code to factorize columns 1:k of A */

			chetf2_(uplo, &k, &a[a_offset], lda, &ipiv[1], &iinfo);
			kb = k;
		}

		/*        Set INFO on the first occurrence of a zero pivot */

		if (*info == 0 && iinfo > 0) {
			*info = iinfo;
		}

		/*        Decrease K and return to the start of the main loop */

		k -= kb;
		goto L10;

	}
	else {

		/*        Factorize A as L*D*L' using the lower triangle of A

		K is the main loop index, increasing from 1 to N in steps of
		KB, where KB is the number of columns factorized by CLAHEF;
		KB is either NB or NB-1, or N-K+1 for the last block */

		k = 1;
	L20:

		/*        If K > N, exit from loop */

		if (k > *n) {
			goto L40;
		}

		if (k <= *n - nb) {

			/*           Factorize columns k:k+kb-1 of A and use blocked code to
			update columns k+kb:n */

			i__1 = *n - k + 1;
			clahef_(uplo, &i__1, &nb, &kb, &a[k + k * a_dim1], lda, &ipiv[k],
				&work[1], n, &iinfo);
		}
		else {

			/*           Use unblocked code to factorize columns k:n of A */

			i__1 = *n - k + 1;
			chetf2_(uplo, &i__1, &a[k + k * a_dim1], lda, &ipiv[k], &iinfo);
			kb = *n - k + 1;
		}

		/*        Set INFO on the first occurrence of a zero pivot */

		if (*info == 0 && iinfo > 0) {
			*info = iinfo + k - 1;
		}

		/*        Adjust IPIV */

		i__1 = k + kb - 1;
		for (j = k; j <= i__1; ++j) {
			if (ipiv[j] > 0) {
				ipiv[j] = ipiv[j] + k - 1;
			}
			else {
				ipiv[j] = ipiv[j] - k + 1;
			}
			/* L30: */
		}

		/*        Increase K and return to the start of the main loop */

		k += kb;
		goto L20;

	}

L40:
	work[1].r = (real)lwkopt, work[1].i = 0.f;
	return 0;

	/*     End of CHETRF */

} /* chetrf_ */


double c_abs(complex *z)
{
	return(f__cabs(z->r, z->i));
}

double f__cabs(double real, double imag)
{
	double temp;

	if (real < 0)
		real = -real;
	if (imag < 0)
		imag = -imag;
	if (imag > real) {
		temp = real;
		real = imag;
		imag = temp;
	}
	if ((real + imag) == real)
		return(real);

	temp = imag / real;
	temp = real*sqrt(1.0 + temp*temp);  /*overflow!!*/
	return(temp);
}

/* Complex */ void cdotc_(complex * ret_val, integer *n, complex *cx, integer
	*incx, complex *cy, integer *incy)
{
	/* System generated locals */
	integer i__1, i__2;
	complex q__1, q__2, q__3;
	/* Builtin functions */
	/* Local variables */
	static integer i__, ix, iy;
	static complex ctemp;
	/*  Purpose
	=======
	forms the dot product of two vectors, conjugating the first
	vector.
	Further Details
	===============
	jack dongarra, linpack,  3/11/78.
	modified 12/3/93, array(1) declarations changed to array(*)
	Parameter adjustments */
	--cy;
	--cx;
	/* Function Body */
	ctemp.r = 0.f, ctemp.i = 0.f;
	ret_val->r = 0.f, ret_val->i = 0.f;
	if (*n <= 0) {
		return;
	}
	if (*incx == 1 && *incy == 1) {
		goto L20;
	}
	/*        code for unequal increments or equal increments
	not equal to 1 */
	ix = 1;
	iy = 1;
	if (*incx < 0) {
		ix = (-(*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
		iy = (-(*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		r_cnjg(&q__3, &cx[ix]);
		i__2 = iy;
		q__2.r = q__3.r * cy[i__2].r - q__3.i * cy[i__2].i, q__2.i = q__3.r *
			cy[i__2].i + q__3.i * cy[i__2].r;
		q__1.r = ctemp.r + q__2.r, q__1.i = ctemp.i + q__2.i;
		ctemp.r = q__1.r, ctemp.i = q__1.i;
		ix += *incx;
		iy += *incy;
		/* L10: */
	}
	ret_val->r = ctemp.r, ret_val->i = ctemp.i;
	return;
	/*        code for both increments equal to 1 */
L20:
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		r_cnjg(&q__3, &cx[i__]);
		i__2 = i__;
		q__2.r = q__3.r * cy[i__2].r - q__3.i * cy[i__2].i, q__2.i = q__3.r *
			cy[i__2].i + q__3.i * cy[i__2].r;
		q__1.r = ctemp.r + q__2.r, q__1.i = ctemp.i + q__2.i;
		ctemp.r = q__1.r, ctemp.i = q__1.i;
		/* L30: */
	}
	ret_val->r = ctemp.r, ret_val->i = ctemp.i;
	return;
} /* cdotc_ */


/* Subroutine */ int chemv_(char *uplo, integer *n, complex *alpha, complex *
	a, integer *lda, complex *x, integer *incx, complex *beta, complex *y,
	integer *incy)
{
	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;
	real r__1;
	complex q__1, q__2, q__3, q__4;
	/* Builtin functions */
	/* Local variables */
	static integer i__, j, ix, iy, jx, jy, kx, ky, info;
	static complex temp1, temp2;

	/*  Purpose
	=======
	CHEMV  performs the matrix-vector  operation
	y := alpha*A*x + beta*y,
	where alpha and beta are scalars, x and y are n element vectors and
	A is an n by n hermitian matrix.
	Arguments
	==========
	UPLO   - CHARACTER*1.
	On entry, UPLO specifies whether the upper or lower
	triangular part of the array A is to be referenced as
	follows:
	UPLO = 'U' or 'u'   Only the upper triangular part of A
	is to be referenced.
	UPLO = 'L' or 'l'   Only the lower triangular part of A
	is to be referenced.
	Unchanged on exit.
	N      - INTEGER.
	On entry, N specifies the order of the matrix A.
	N must be at least zero.
	Unchanged on exit.
	ALPHA  - COMPLEX         .
	On entry, ALPHA specifies the scalar alpha.
	Unchanged on exit.
	A      - COMPLEX          array of DIMENSION ( LDA, n ).
	Before entry with  UPLO = 'U' or 'u', the leading n by n
	upper triangular part of the array A must contain the upper
	triangular part of the hermitian matrix and the strictly
	lower triangular part of A is not referenced.
	Before entry with UPLO = 'L' or 'l', the leading n by n
	lower triangular part of the array A must contain the lower
	triangular part of the hermitian matrix and the strictly
	upper triangular part of A is not referenced.
	Note that the imaginary parts of the diagonal elements need
	not be set and are assumed to be zero.
	Unchanged on exit.
	LDA    - INTEGER.
	On entry, LDA specifies the first dimension of A as declared
	in the calling (sub) program. LDA must be at least
	max( 1, n ).
	Unchanged on exit.
	X      - COMPLEX          array of dimension at least
	( 1 + ( n - 1 )*abs( INCX ) ).
	Before entry, the incremented array X must contain the n
	element vector x.
	Unchanged on exit.
	INCX   - INTEGER.
	On entry, INCX specifies the increment for the elements of
	X. INCX must not be zero.
	Unchanged on exit.
	BETA   - COMPLEX         .
	On entry, BETA specifies the scalar beta. When BETA is
	supplied as zero then Y need not be set on input.
	Unchanged on exit.
	Y      - COMPLEX          array of dimension at least
	( 1 + ( n - 1 )*abs( INCY ) ).
	Before entry, the incremented array Y must contain the n
	element vector y. On exit, Y is overwritten by the updated
	vector y.
	INCY   - INTEGER.
	On entry, INCY specifies the increment for the elements of
	Y. INCY must not be zero.
	Unchanged on exit.
	Level 2 Blas routine.
	-- Written on 22-October-1986.
	Jack Dongarra, Argonne National Lab.
	Jeremy Du Croz, Nag Central Office.
	Sven Hammarling, Nag Central Office.
	Richard Hanson, Sandia National Labs.
	Test the input parameters.
	Parameter adjustments */
	a_dim1 = *lda;
	a_offset = 1 + a_dim1;
	a -= a_offset;
	--x;
	--y;
	/* Function Body */
	info = 0;
	if (!lsame_(uplo, U_buffer) && !lsame_(uplo, L_buffer)) {
		info = 1;
	}
	else if (*n < 0) {
		info = 2;
	}
	else if (*lda < max(1, *n)) {
		info = 5;
	}
	else if (*incx == 0) {
		info = 7;
	}
	else if (*incy == 0) {
		info = 10;
	}
	if (info != 0) {
		xerbla_(CHEMV_buffer, &info);
		return 0;
	}
	/*     Quick return if possible. */
	if (*n == 0 || alpha->r == 0.f && alpha->i == 0.f && (beta->r == 1.f &&
		beta->i == 0.f)) {
		return 0;
	}
	/*     Set up the start points in  X  and  Y. */
	if (*incx > 0) {
		kx = 1;
	}
	else {
		kx = 1 - (*n - 1) * *incx;
	}
	if (*incy > 0) {
		ky = 1;
	}
	else {
		ky = 1 - (*n - 1) * *incy;
	}
	/*     Start the operations. In this version the elements of A are
	accessed sequentially with one pass through the triangular part
	of A.
	First form  y := beta*y. */
	if (beta->r != 1.f || beta->i != 0.f) {
		if (*incy == 1) {
			if (beta->r == 0.f && beta->i == 0.f) {
				i__1 = *n;
				for (i__ = 1; i__ <= i__1; ++i__) {
					i__2 = i__;
					y[i__2].r = 0.f, y[i__2].i = 0.f;
					/* L10: */
				}
			}
			else {
				i__1 = *n;
				for (i__ = 1; i__ <= i__1; ++i__) {
					i__2 = i__;
					i__3 = i__;
					q__1.r = beta->r * y[i__3].r - beta->i * y[i__3].i,
						q__1.i = beta->r * y[i__3].i + beta->i * y[i__3]
						.r;
					y[i__2].r = q__1.r, y[i__2].i = q__1.i;
					/* L20: */
				}
			}
		}
		else {
			iy = ky;
			if (beta->r == 0.f && beta->i == 0.f) {
				i__1 = *n;
				for (i__ = 1; i__ <= i__1; ++i__) {
					i__2 = iy;
					y[i__2].r = 0.f, y[i__2].i = 0.f;
					iy += *incy;
					/* L30: */
				}
			}
			else {
				i__1 = *n;
				for (i__ = 1; i__ <= i__1; ++i__) {
					i__2 = iy;
					i__3 = iy;
					q__1.r = beta->r * y[i__3].r - beta->i * y[i__3].i,
						q__1.i = beta->r * y[i__3].i + beta->i * y[i__3]
						.r;
					y[i__2].r = q__1.r, y[i__2].i = q__1.i;
					iy += *incy;
					/* L40: */
				}
			}
		}
	}
	if (alpha->r == 0.f && alpha->i == 0.f) {
		return 0;
	}
	if (lsame_(uplo, U_buffer)) {
		/*        Form  y  when A is stored in upper triangle. */
		if (*incx == 1 && *incy == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = j;
				q__1.r = alpha->r * x[i__2].r - alpha->i * x[i__2].i, q__1.i =
					alpha->r * x[i__2].i + alpha->i * x[i__2].r;
				temp1.r = q__1.r, temp1.i = q__1.i;
				temp2.r = 0.f, temp2.i = 0.f;
				i__2 = j - 1;
				for (i__ = 1; i__ <= i__2; ++i__) {
					i__3 = i__;
					i__4 = i__;
					i__5 = i__ + j * a_dim1;
					q__2.r = temp1.r * a[i__5].r - temp1.i * a[i__5].i,
						q__2.i = temp1.r * a[i__5].i + temp1.i * a[i__5]
						.r;
					q__1.r = y[i__4].r + q__2.r, q__1.i = y[i__4].i + q__2.i;
					y[i__3].r = q__1.r, y[i__3].i = q__1.i;
					r_cnjg(&q__3, &a[i__ + j * a_dim1]);
					i__3 = i__;
					q__2.r = q__3.r * x[i__3].r - q__3.i * x[i__3].i, q__2.i =
						q__3.r * x[i__3].i + q__3.i * x[i__3].r;
					q__1.r = temp2.r + q__2.r, q__1.i = temp2.i + q__2.i;
					temp2.r = q__1.r, temp2.i = q__1.i;
					/* L50: */
				}
				i__2 = j;
				i__3 = j;
				i__4 = j + j * a_dim1;
				r__1 = a[i__4].r;
				q__3.r = r__1 * temp1.r, q__3.i = r__1 * temp1.i;
				q__2.r = y[i__3].r + q__3.r, q__2.i = y[i__3].i + q__3.i;
				q__4.r = alpha->r * temp2.r - alpha->i * temp2.i, q__4.i =
					alpha->r * temp2.i + alpha->i * temp2.r;
				q__1.r = q__2.r + q__4.r, q__1.i = q__2.i + q__4.i;
				y[i__2].r = q__1.r, y[i__2].i = q__1.i;
				/* L60: */
			}
		}
		else {
			jx = kx;
			jy = ky;
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = jx;
				q__1.r = alpha->r * x[i__2].r - alpha->i * x[i__2].i, q__1.i =
					alpha->r * x[i__2].i + alpha->i * x[i__2].r;
				temp1.r = q__1.r, temp1.i = q__1.i;
				temp2.r = 0.f, temp2.i = 0.f;
				ix = kx;
				iy = ky;
				i__2 = j - 1;
				for (i__ = 1; i__ <= i__2; ++i__) {
					i__3 = iy;
					i__4 = iy;
					i__5 = i__ + j * a_dim1;
					q__2.r = temp1.r * a[i__5].r - temp1.i * a[i__5].i,
						q__2.i = temp1.r * a[i__5].i + temp1.i * a[i__5]
						.r;
					q__1.r = y[i__4].r + q__2.r, q__1.i = y[i__4].i + q__2.i;
					y[i__3].r = q__1.r, y[i__3].i = q__1.i;
					r_cnjg(&q__3, &a[i__ + j * a_dim1]);
					i__3 = ix;
					q__2.r = q__3.r * x[i__3].r - q__3.i * x[i__3].i, q__2.i =
						q__3.r * x[i__3].i + q__3.i * x[i__3].r;
					q__1.r = temp2.r + q__2.r, q__1.i = temp2.i + q__2.i;
					temp2.r = q__1.r, temp2.i = q__1.i;
					ix += *incx;
					iy += *incy;
					/* L70: */
				}
				i__2 = jy;
				i__3 = jy;
				i__4 = j + j * a_dim1;
				r__1 = a[i__4].r;
				q__3.r = r__1 * temp1.r, q__3.i = r__1 * temp1.i;
				q__2.r = y[i__3].r + q__3.r, q__2.i = y[i__3].i + q__3.i;
				q__4.r = alpha->r * temp2.r - alpha->i * temp2.i, q__4.i =
					alpha->r * temp2.i + alpha->i * temp2.r;
				q__1.r = q__2.r + q__4.r, q__1.i = q__2.i + q__4.i;
				y[i__2].r = q__1.r, y[i__2].i = q__1.i;
				jx += *incx;
				jy += *incy;
				/* L80: */
			}
		}
	}
	else {
		/*        Form  y  when A is stored in lower triangle. */
		if (*incx == 1 && *incy == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = j;
				q__1.r = alpha->r * x[i__2].r - alpha->i * x[i__2].i, q__1.i =
					alpha->r * x[i__2].i + alpha->i * x[i__2].r;
				temp1.r = q__1.r, temp1.i = q__1.i;
				temp2.r = 0.f, temp2.i = 0.f;
				i__2 = j;
				i__3 = j;
				i__4 = j + j * a_dim1;
				r__1 = a[i__4].r;
				q__2.r = r__1 * temp1.r, q__2.i = r__1 * temp1.i;
				q__1.r = y[i__3].r + q__2.r, q__1.i = y[i__3].i + q__2.i;
				y[i__2].r = q__1.r, y[i__2].i = q__1.i;
				i__2 = *n;
				for (i__ = j + 1; i__ <= i__2; ++i__) {
					i__3 = i__;
					i__4 = i__;
					i__5 = i__ + j * a_dim1;
					q__2.r = temp1.r * a[i__5].r - temp1.i * a[i__5].i,
						q__2.i = temp1.r * a[i__5].i + temp1.i * a[i__5]
						.r;
					q__1.r = y[i__4].r + q__2.r, q__1.i = y[i__4].i + q__2.i;
					y[i__3].r = q__1.r, y[i__3].i = q__1.i;
					r_cnjg(&q__3, &a[i__ + j * a_dim1]);
					i__3 = i__;
					q__2.r = q__3.r * x[i__3].r - q__3.i * x[i__3].i, q__2.i =
						q__3.r * x[i__3].i + q__3.i * x[i__3].r;
					q__1.r = temp2.r + q__2.r, q__1.i = temp2.i + q__2.i;
					temp2.r = q__1.r, temp2.i = q__1.i;
					/* L90: */
				}
				i__2 = j;
				i__3 = j;
				q__2.r = alpha->r * temp2.r - alpha->i * temp2.i, q__2.i =
					alpha->r * temp2.i + alpha->i * temp2.r;
				q__1.r = y[i__3].r + q__2.r, q__1.i = y[i__3].i + q__2.i;
				y[i__2].r = q__1.r, y[i__2].i = q__1.i;
				/* L100: */
			}
		}
		else {
			jx = kx;
			jy = ky;
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = jx;
				q__1.r = alpha->r * x[i__2].r - alpha->i * x[i__2].i, q__1.i =
					alpha->r * x[i__2].i + alpha->i * x[i__2].r;
				temp1.r = q__1.r, temp1.i = q__1.i;
				temp2.r = 0.f, temp2.i = 0.f;
				i__2 = jy;
				i__3 = jy;
				i__4 = j + j * a_dim1;
				r__1 = a[i__4].r;
				q__2.r = r__1 * temp1.r, q__2.i = r__1 * temp1.i;
				q__1.r = y[i__3].r + q__2.r, q__1.i = y[i__3].i + q__2.i;
				y[i__2].r = q__1.r, y[i__2].i = q__1.i;
				ix = jx;
				iy = jy;
				i__2 = *n;
				for (i__ = j + 1; i__ <= i__2; ++i__) {
					ix += *incx;
					iy += *incy;
					i__3 = iy;
					i__4 = iy;
					i__5 = i__ + j * a_dim1;
					q__2.r = temp1.r * a[i__5].r - temp1.i * a[i__5].i,
						q__2.i = temp1.r * a[i__5].i + temp1.i * a[i__5]
						.r;
					q__1.r = y[i__4].r + q__2.r, q__1.i = y[i__4].i + q__2.i;
					y[i__3].r = q__1.r, y[i__3].i = q__1.i;
					r_cnjg(&q__3, &a[i__ + j * a_dim1]);
					i__3 = ix;
					q__2.r = q__3.r * x[i__3].r - q__3.i * x[i__3].i, q__2.i =
						q__3.r * x[i__3].i + q__3.i * x[i__3].r;
					q__1.r = temp2.r + q__2.r, q__1.i = temp2.i + q__2.i;
					temp2.r = q__1.r, temp2.i = q__1.i;
					/* L110: */
				}
				i__2 = jy;
				i__3 = jy;
				q__2.r = alpha->r * temp2.r - alpha->i * temp2.i, q__2.i =
					alpha->r * temp2.i + alpha->i * temp2.r;
				q__1.r = y[i__3].r + q__2.r, q__1.i = y[i__3].i + q__2.i;
				y[i__2].r = q__1.r, y[i__2].i = q__1.i;
				jx += *incx;
				jy += *incy;
				/* L120: */
			}
		}
	}
	return 0;
	/*     End of CHEMV . */
} /* chemv_ */

/* Subroutine */ int chetri_(char *uplo, integer *n, complex *a, integer *lda,
	integer *ipiv, complex *work, integer *info)
{
	/*  -- LAPACK routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	CHETRI computes the inverse of a complex Hermitian indefinite matrix
	A using the factorization A = U*D*U**H or A = L*D*L**H computed by
	CHETRF.

	Arguments
	=========

	UPLO    (input) CHARACTER*1
	Specifies whether the details of the factorization are stored
	as an upper or lower triangular matrix.
	= 'U':  Upper triangular, form is A = U*D*U**H;
	= 'L':  Lower triangular, form is A = L*D*L**H.

	N       (input) INTEGER
	The order of the matrix A.  N >= 0.

	A       (input/output) COMPLEX array, dimension (LDA,N)
	On entry, the block diagonal matrix D and the multipliers
	used to obtain the factor U or L as computed by CHETRF.

	On exit, if INFO = 0, the (Hermitian) inverse of the original
	matrix.  If UPLO = 'U', the upper triangular part of the
	inverse is formed and the part of A below the diagonal is not
	referenced; if UPLO = 'L' the lower triangular part of the
	inverse is formed and the part of A above the diagonal is
	not referenced.

	LDA     (input) INTEGER
	The leading dimension of the array A.  LDA >= max(1,N).

	IPIV    (input) INTEGER array, dimension (N)
	Details of the interchanges and the block structure of D
	as determined by CHETRF.

	WORK    (workspace) COMPLEX array, dimension (N)

	INFO    (output) INTEGER
	= 0: successful exit
	< 0: if INFO = -i, the i-th argument had an illegal value
	> 0: if INFO = i, D(i,i) = 0; the matrix is singular and its
	inverse could not be computed.

	=====================================================================


	Test the input parameters.

	Parameter adjustments */
	/* Table of constant values */
	static complex c_b2 = { 0.f,0.f };
	static integer c__1 = 1;

	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2, i__3;
	real r__1;
	complex q__1, q__2;
	/* Builtin functions */
	/* Local variables */
	static real d__;
	static integer j, k;
	static real t, ak;
	static integer kp;
	static real akp1;
	static complex temp, akkp1;
	static integer kstep;
	static logical upper;


	a_dim1 = *lda;
	a_offset = 1 + a_dim1;
	a -= a_offset;
	--ipiv;
	--work;

	/* Function Body */
	*info = 0;
	upper = lsame_(uplo, U_buffer);
	if (!upper && !lsame_(uplo, L_buffer)) {
		*info = -1;
	}
	else if (*n < 0) {
		*info = -2;
	}
	else if (*lda < max(1, *n)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = -(*info);
		xerbla_(CHETRI_buffer, &i__1);
		return 0;
	}

	/*     Quick return if possible */

	if (*n == 0) {
		return 0;
	}

	/*     Check that the diagonal matrix D is nonsingular. */

	if (upper) {

		/*        Upper triangular storage: examine D from bottom to top */

		for (*info = *n; *info >= 1; --(*info)) {
			i__1 = *info + *info * a_dim1;
			if (ipiv[*info] > 0 && (a[i__1].r == 0.f && a[i__1].i == 0.f)) {
				return 0;
			}
			/* L10: */
		}
	}
	else {

		/*        Lower triangular storage: examine D from top to bottom. */

		i__1 = *n;
		for (*info = 1; *info <= i__1; ++(*info)) {
			i__2 = *info + *info * a_dim1;
			if (ipiv[*info] > 0 && (a[i__2].r == 0.f && a[i__2].i == 0.f)) {
				return 0;
			}
			/* L20: */
		}
	}
	*info = 0;

	if (upper) {

		/*        Compute inv(A) from the factorization A = U*D*U'.

		K is the main loop index, increasing from 1 to N in steps of
		1 or 2, depending on the size of the diagonal blocks. */

		k = 1;
	L30:

		/*        If K > N, exit from loop. */

		if (k > *n) {
			goto L50;
		}

		if (ipiv[k] > 0) {

			/*           1 x 1 diagonal block

			Invert the diagonal block. */

			i__1 = k + k * a_dim1;
			i__2 = k + k * a_dim1;
			r__1 = 1.f / a[i__2].r;
			a[i__1].r = r__1, a[i__1].i = 0.f;

			/*           Compute column K of the inverse. */

			if (k > 1) {
				i__1 = k - 1;
				ccopy_(&i__1, &a[k * a_dim1 + 1], &c__1, &work[1], &c__1);
				i__1 = k - 1;
				q__1.r = -1.f, q__1.i = -0.f;
				chemv_(uplo, &i__1, &q__1, &a[a_offset], lda, &work[1], &c__1,
					&c_b2, &a[k * a_dim1 + 1], &c__1);
				i__1 = k + k * a_dim1;
				i__2 = k + k * a_dim1;
				i__3 = k - 1;
				cdotc_(&q__2, &i__3, &work[1], &c__1, &a[k * a_dim1 + 1], &
					c__1);
				r__1 = q__2.r;
				q__1.r = a[i__2].r - r__1, q__1.i = a[i__2].i;
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
			}
			kstep = 1;
		}
		else {

			/*           2 x 2 diagonal block

			Invert the diagonal block. */

			t = c_abs(&a[k + (k + 1) * a_dim1]);
			i__1 = k + k * a_dim1;
			ak = a[i__1].r / t;
			i__1 = k + 1 + (k + 1) * a_dim1;
			akp1 = a[i__1].r / t;
			i__1 = k + (k + 1) * a_dim1;
			q__1.r = a[i__1].r / t, q__1.i = a[i__1].i / t;
			akkp1.r = q__1.r, akkp1.i = q__1.i;
			d__ = t * (ak * akp1 - 1.f);
			i__1 = k + k * a_dim1;
			r__1 = akp1 / d__;
			a[i__1].r = r__1, a[i__1].i = 0.f;
			i__1 = k + 1 + (k + 1) * a_dim1;
			r__1 = ak / d__;
			a[i__1].r = r__1, a[i__1].i = 0.f;
			i__1 = k + (k + 1) * a_dim1;
			q__2.r = -akkp1.r, q__2.i = -akkp1.i;
			q__1.r = q__2.r / d__, q__1.i = q__2.i / d__;
			a[i__1].r = q__1.r, a[i__1].i = q__1.i;

			/*           Compute columns K and K+1 of the inverse. */

			if (k > 1) {
				i__1 = k - 1;
				ccopy_(&i__1, &a[k * a_dim1 + 1], &c__1, &work[1], &c__1);
				i__1 = k - 1;
				q__1.r = -1.f, q__1.i = -0.f;
				chemv_(uplo, &i__1, &q__1, &a[a_offset], lda, &work[1], &c__1,
					&c_b2, &a[k * a_dim1 + 1], &c__1);
				i__1 = k + k * a_dim1;
				i__2 = k + k * a_dim1;
				i__3 = k - 1;
				cdotc_(&q__2, &i__3, &work[1], &c__1, &a[k * a_dim1 + 1], &
					c__1);
				r__1 = q__2.r;
				q__1.r = a[i__2].r - r__1, q__1.i = a[i__2].i;
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
				i__1 = k + (k + 1) * a_dim1;
				i__2 = k + (k + 1) * a_dim1;
				i__3 = k - 1;
				cdotc_(&q__2, &i__3, &a[k * a_dim1 + 1], &c__1, &a[(k + 1) *
					a_dim1 + 1], &c__1);
				q__1.r = a[i__2].r - q__2.r, q__1.i = a[i__2].i - q__2.i;
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
				i__1 = k - 1;
				ccopy_(&i__1, &a[(k + 1) * a_dim1 + 1], &c__1, &work[1], &
					c__1);
				i__1 = k - 1;
				q__1.r = -1.f, q__1.i = -0.f;
				chemv_(uplo, &i__1, &q__1, &a[a_offset], lda, &work[1], &c__1,
					&c_b2, &a[(k + 1) * a_dim1 + 1], &c__1);
				i__1 = k + 1 + (k + 1) * a_dim1;
				i__2 = k + 1 + (k + 1) * a_dim1;
				i__3 = k - 1;
				cdotc_(&q__2, &i__3, &work[1], &c__1, &a[(k + 1) * a_dim1 + 1]
					, &c__1);
				r__1 = q__2.r;
				q__1.r = a[i__2].r - r__1, q__1.i = a[i__2].i;
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
			}
			kstep = 2;
		}

		kp = (i__1 = ipiv[k], abs(i__1));
		if (kp != k) {

			/*           Interchange rows and columns K and KP in the leading
			submatrix A(1:k+1,1:k+1) */

			i__1 = kp - 1;
			cswap_(&i__1, &a[k * a_dim1 + 1], &c__1, &a[kp * a_dim1 + 1], &
				c__1);
			i__1 = k - 1;
			for (j = kp + 1; j <= i__1; ++j) {
				r_cnjg(&q__1, &a[j + k * a_dim1]);
				temp.r = q__1.r, temp.i = q__1.i;
				i__2 = j + k * a_dim1;
				r_cnjg(&q__1, &a[kp + j * a_dim1]);
				a[i__2].r = q__1.r, a[i__2].i = q__1.i;
				i__2 = kp + j * a_dim1;
				a[i__2].r = temp.r, a[i__2].i = temp.i;
				/* L40: */
			}
			i__1 = kp + k * a_dim1;
			r_cnjg(&q__1, &a[kp + k * a_dim1]);
			a[i__1].r = q__1.r, a[i__1].i = q__1.i;
			i__1 = k + k * a_dim1;
			temp.r = a[i__1].r, temp.i = a[i__1].i;
			i__1 = k + k * a_dim1;
			i__2 = kp + kp * a_dim1;
			a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
			i__1 = kp + kp * a_dim1;
			a[i__1].r = temp.r, a[i__1].i = temp.i;
			if (kstep == 2) {
				i__1 = k + (k + 1) * a_dim1;
				temp.r = a[i__1].r, temp.i = a[i__1].i;
				i__1 = k + (k + 1) * a_dim1;
				i__2 = kp + (k + 1) * a_dim1;
				a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
				i__1 = kp + (k + 1) * a_dim1;
				a[i__1].r = temp.r, a[i__1].i = temp.i;
			}
		}

		k += kstep;
		goto L30;
	L50:

		;
	}
	else {

		/*        Compute inv(A) from the factorization A = L*D*L'.

		K is the main loop index, increasing from 1 to N in steps of
		1 or 2, depending on the size of the diagonal blocks. */

		k = *n;
	L60:

		/*        If K < 1, exit from loop. */

		if (k < 1) {
			goto L80;
		}

		if (ipiv[k] > 0) {

			/*           1 x 1 diagonal block

			Invert the diagonal block. */

			i__1 = k + k * a_dim1;
			i__2 = k + k * a_dim1;
			r__1 = 1.f / a[i__2].r;
			a[i__1].r = r__1, a[i__1].i = 0.f;

			/*           Compute column K of the inverse. */

			if (k < *n) {
				i__1 = *n - k;
				ccopy_(&i__1, &a[k + 1 + k * a_dim1], &c__1, &work[1], &c__1);
				i__1 = *n - k;
				q__1.r = -1.f, q__1.i = -0.f;
				chemv_(uplo, &i__1, &q__1, &a[k + 1 + (k + 1) * a_dim1], lda,
					&work[1], &c__1, &c_b2, &a[k + 1 + k * a_dim1], &c__1);
				i__1 = k + k * a_dim1;
				i__2 = k + k * a_dim1;
				i__3 = *n - k;
				cdotc_(&q__2, &i__3, &work[1], &c__1, &a[k + 1 + k * a_dim1],
					&c__1);
				r__1 = q__2.r;
				q__1.r = a[i__2].r - r__1, q__1.i = a[i__2].i;
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
			}
			kstep = 1;
		}
		else {

			/*           2 x 2 diagonal block

			Invert the diagonal block. */

			t = c_abs(&a[k + (k - 1) * a_dim1]);
			i__1 = k - 1 + (k - 1) * a_dim1;
			ak = a[i__1].r / t;
			i__1 = k + k * a_dim1;
			akp1 = a[i__1].r / t;
			i__1 = k + (k - 1) * a_dim1;
			q__1.r = a[i__1].r / t, q__1.i = a[i__1].i / t;
			akkp1.r = q__1.r, akkp1.i = q__1.i;
			d__ = t * (ak * akp1 - 1.f);
			i__1 = k - 1 + (k - 1) * a_dim1;
			r__1 = akp1 / d__;
			a[i__1].r = r__1, a[i__1].i = 0.f;
			i__1 = k + k * a_dim1;
			r__1 = ak / d__;
			a[i__1].r = r__1, a[i__1].i = 0.f;
			i__1 = k + (k - 1) * a_dim1;
			q__2.r = -akkp1.r, q__2.i = -akkp1.i;
			q__1.r = q__2.r / d__, q__1.i = q__2.i / d__;
			a[i__1].r = q__1.r, a[i__1].i = q__1.i;

			/*           Compute columns K-1 and K of the inverse. */

			if (k < *n) {
				i__1 = *n - k;
				ccopy_(&i__1, &a[k + 1 + k * a_dim1], &c__1, &work[1], &c__1);
				i__1 = *n - k;
				q__1.r = -1.f, q__1.i = -0.f;
				chemv_(uplo, &i__1, &q__1, &a[k + 1 + (k + 1) * a_dim1], lda,
					&work[1], &c__1, &c_b2, &a[k + 1 + k * a_dim1], &c__1);
				i__1 = k + k * a_dim1;
				i__2 = k + k * a_dim1;
				i__3 = *n - k;
				cdotc_(&q__2, &i__3, &work[1], &c__1, &a[k + 1 + k * a_dim1],
					&c__1);
				r__1 = q__2.r;
				q__1.r = a[i__2].r - r__1, q__1.i = a[i__2].i;
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
				i__1 = k + (k - 1) * a_dim1;
				i__2 = k + (k - 1) * a_dim1;
				i__3 = *n - k;
				cdotc_(&q__2, &i__3, &a[k + 1 + k * a_dim1], &c__1, &a[k + 1
					+ (k - 1) * a_dim1], &c__1);
				q__1.r = a[i__2].r - q__2.r, q__1.i = a[i__2].i - q__2.i;
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
				i__1 = *n - k;
				ccopy_(&i__1, &a[k + 1 + (k - 1) * a_dim1], &c__1, &work[1], &
					c__1);
				i__1 = *n - k;
				q__1.r = -1.f, q__1.i = -0.f;
				chemv_(uplo, &i__1, &q__1, &a[k + 1 + (k + 1) * a_dim1], lda,
					&work[1], &c__1, &c_b2, &a[k + 1 + (k - 1) * a_dim1],
					&c__1);
				i__1 = k - 1 + (k - 1) * a_dim1;
				i__2 = k - 1 + (k - 1) * a_dim1;
				i__3 = *n - k;
				cdotc_(&q__2, &i__3, &work[1], &c__1, &a[k + 1 + (k - 1) *
					a_dim1], &c__1);
				r__1 = q__2.r;
				q__1.r = a[i__2].r - r__1, q__1.i = a[i__2].i;
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
			}
			kstep = 2;
		}

		kp = (i__1 = ipiv[k], abs(i__1));
		if (kp != k) {

			/*           Interchange rows and columns K and KP in the trailing
			submatrix A(k-1:n,k-1:n) */

			if (kp < *n) {
				i__1 = *n - kp;
				cswap_(&i__1, &a[kp + 1 + k * a_dim1], &c__1, &a[kp + 1 + kp *
					a_dim1], &c__1);
			}
			i__1 = kp - 1;
			for (j = k + 1; j <= i__1; ++j) {
				r_cnjg(&q__1, &a[j + k * a_dim1]);
				temp.r = q__1.r, temp.i = q__1.i;
				i__2 = j + k * a_dim1;
				r_cnjg(&q__1, &a[kp + j * a_dim1]);
				a[i__2].r = q__1.r, a[i__2].i = q__1.i;
				i__2 = kp + j * a_dim1;
				a[i__2].r = temp.r, a[i__2].i = temp.i;
				/* L70: */
			}
			i__1 = kp + k * a_dim1;
			r_cnjg(&q__1, &a[kp + k * a_dim1]);
			a[i__1].r = q__1.r, a[i__1].i = q__1.i;
			i__1 = k + k * a_dim1;
			temp.r = a[i__1].r, temp.i = a[i__1].i;
			i__1 = k + k * a_dim1;
			i__2 = kp + kp * a_dim1;
			a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
			i__1 = kp + kp * a_dim1;
			a[i__1].r = temp.r, a[i__1].i = temp.i;
			if (kstep == 2) {
				i__1 = k + (k - 1) * a_dim1;
				temp.r = a[i__1].r, temp.i = a[i__1].i;
				i__1 = k + (k - 1) * a_dim1;
				i__2 = kp + (k - 1) * a_dim1;
				a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
				i__1 = kp + (k - 1) * a_dim1;
				a[i__1].r = temp.r, a[i__1].i = temp.i;
			}
		}

		k -= kstep;
		goto L60;
	L80:
		;
	}

	return 0;

	/*     End of CHETRI */

} /* chetri_ */


logical lsame_(char *ca, char *cb)
{
	/*  -- LAPACK auxiliary routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	LSAME returns .TRUE. if CA is the same letter as CB regardless of
	case.

	Arguments
	=========

	CA      (input) CHARACTER*1
	CB      (input) CHARACTER*1
	CA and CB specify the single characters to be compared.

	=====================================================================


	Test if the characters are equal */
	/* System generated locals */
	logical ret_val;
	/* Local variables */
	static integer inta, intb, zcode;


	ret_val = *(unsigned char *)ca == *(unsigned char *)cb;
	if (ret_val) {
		return ret_val;
	}

	/*     Now test for equivalence if both characters are alphabetic. */

	zcode = 'Z';

	/*     Use 'Z' rather than 'A' so that ASCII can be detected on Prime
	machines, on which ICHAR returns a value with bit 8 set.
	ICHAR('A') on Prime machines returns 193 which is the same as
	ICHAR('A') on an EBCDIC machine. */

	inta = *(unsigned char *)ca;
	intb = *(unsigned char *)cb;

	if (zcode == 90 || zcode == 122) {

		/*        ASCII is assumed - ZCODE is the ASCII code of either lower or
		upper case 'Z'. */

		if (inta >= 97 && inta <= 122) {
			inta += -32;
		}
		if (intb >= 97 && intb <= 122) {
			intb += -32;
		}

	}
	else if (zcode == 233 || zcode == 169) {

		/*        EBCDIC is assumed - ZCODE is the EBCDIC code of either lower or
		upper case 'Z'. */

		if (inta >= 129 && inta <= 137 || inta >= 145 && inta <= 153 || inta
			>= 162 && inta <= 169) {
			inta += 64;
		}
		if (intb >= 129 && intb <= 137 || intb >= 145 && intb <= 153 || intb
			>= 162 && intb <= 169) {
			intb += 64;
		}

	}
	else if (zcode == 218 || zcode == 250) {

		/*        ASCII is assumed, on Prime machines - ZCODE is the ASCII code
		plus 128 of either lower or upper case 'Z'. */

		if (inta >= 225 && inta <= 250) {
			inta += -32;
		}
		if (intb >= 225 && intb <= 250) {
			intb += -32;
		}
	}
	ret_val = inta == intb;

	/*     RETURN

	End of LSAME */

	return ret_val;
} /* lsame_ */


double r_imag(complex *z)
{
	return(z->i);
}
void r_cnjg(complex *r, complex *z)
{
	real zi = z->i;
	r->r = z->r;
	r->i = -zi;
}


doublereal slapy2_(real *x, real *y)
{
	/*  -- LAPACK auxiliary routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	SLAPY2 returns sqrt(x**2+y**2), taking care not to cause unnecessary
	overflow.

	Arguments
	=========

	X       (input) REAL
	Y       (input) REAL
	X and Y specify the values x and y.

	===================================================================== */
	/* System generated locals */
	real ret_val, r__1;
	/* Builtin functions */
	double sqrt(doublereal);
	/* Local variables */
	static real w, z__, xabs, yabs;



	xabs = dabs(*x);
	yabs = dabs(*y);
	w = dmax(xabs, yabs);
	z__ = dmin(xabs, yabs);
	if (z__ == 0.f) {
		ret_val = w;
	}
	else {
		/* Computing 2nd power */
		r__1 = z__ / w;
		ret_val = w * sqrt(r__1 * r__1 + 1.f);
	}
	return ret_val;

	/*     End of SLAPY2 */

} /* slapy2_ */


logical slaisnan_(real *sin1, real *sin2)
{
	/*  -- LAPACK auxiliary routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	This routine is not for general use.  It exists solely to avoid
	over-optimization in SISNAN.

	SLAISNAN checks for NaNs by comparing its two arguments for
	inequality.  NaN is the only floating-point value where NaN != NaN
	returns .TRUE.  To check for NaNs, pass the same variable as both
	arguments.

	Strictly speaking, Fortran does not allow aliasing of function
	arguments. So a compiler must assume that the two arguments are
	not the same variable, and the test will not be optimized away.
	Interprocedural or whole-program optimization may delete this
	test.  The ISNAN functions will be replaced by the correct
	Fortran 03 intrinsic once the intrinsic is widely available.

	Arguments
	=========

	SIN1     (input) REAL
	SIN2     (input) REAL
	Two numbers to compare for inequality.

	===================================================================== */
	/* System generated locals */
	logical ret_val;


	ret_val = *sin1 != *sin2;
	return ret_val;
} /* slaisnan_ */
logical sisnan_(real *sin__)
{
	/*  -- LAPACK auxiliary routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	SISNAN returns .TRUE. if its argument is NaN, and .FALSE.
	otherwise.  To be replaced by the Fortran 2003 intrinsic in the
	future.

	Arguments
	=========

	SIN      (input) REAL
	Input to test for NaN.

	===================================================================== */
	/* System generated locals */
	logical ret_val;
	/* Local variables */

	ret_val = slaisnan_(sin__, sin__);
	return ret_val;
} /* sisnan_ */


/* Subroutine */ int chetf2_(char *uplo, integer *n, complex *a, integer *lda,
	integer *ipiv, integer *info)
{
	/*  -- LAPACK routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	CHETF2 computes the factorization of a complex Hermitian matrix A
	using the Bunch-Kaufman diagonal pivoting method:

	A = U*D*U'  or  A = L*D*L'

	where U (or L) is a product of permutation and unit upper (lower)
	triangular matrices, U' is the conjugate transpose of U, and D is
	Hermitian and block diagonal with 1-by-1 and 2-by-2 diagonal blocks.

	This is the unblocked version of the algorithm, calling Level 2 BLAS.

	Arguments
	=========

	UPLO    (input) CHARACTER*1
	Specifies whether the upper or lower triangular part of the
	Hermitian matrix A is stored:
	= 'U':  Upper triangular
	= 'L':  Lower triangular

	N       (input) INTEGER
	The order of the matrix A.  N >= 0.

	A       (input/output) COMPLEX array, dimension (LDA,N)
	On entry, the Hermitian matrix A.  If UPLO = 'U', the leading
	n-by-n upper triangular part of A contains the upper
	triangular part of the matrix A, and the strictly lower
	triangular part of A is not referenced.  If UPLO = 'L', the
	leading n-by-n lower triangular part of A contains the lower
	triangular part of the matrix A, and the strictly upper
	triangular part of A is not referenced.

	On exit, the block diagonal matrix D and the multipliers used
	to obtain the factor U or L (see below for further details).

	LDA     (input) INTEGER
	The leading dimension of the array A.  LDA >= max(1,N).

	IPIV    (output) INTEGER array, dimension (N)
	Details of the interchanges and the block structure of D.
	If IPIV(k) > 0, then rows and columns k and IPIV(k) were
	interchanged and D(k,k) is a 1-by-1 diagonal block.
	If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and
	columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k)
	is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) =
	IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were
	interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block.

	INFO    (output) INTEGER
	= 0: successful exit
	< 0: if INFO = -k, the k-th argument had an illegal value
	> 0: if INFO = k, D(k,k) is exactly zero.  The factorization
	has been completed, but the block diagonal matrix D is
	exactly singular, and division by zero will occur if it
	is used to solve a system of equations.

	Further Details
	===============

	09-29-06 - patch from
	Bobby Cheng, MathWorks

	Replace l.210 and l.392
	IF( MAX( ABSAKK, COLMAX ).EQ.ZERO ) THEN
	by
	IF( (MAX( ABSAKK, COLMAX ).EQ.ZERO) .OR. SISNAN(ABSAKK) ) THEN

	01-01-96 - Based on modifications by
	J. Lewis, Boeing Computer Services Company
	A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA

	If UPLO = 'U', then A = U*D*U', where
	U = P(n)*U(n)* ... *P(k)U(k)* ...,
	i.e., U is a product of terms P(k)*U(k), where k decreases from n to
	1 in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1
	and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as
	defined by IPIV(k), and U(k) is a unit upper triangular matrix, such
	that if the diagonal block D(k) is of order s (s = 1 or 2), then

	(   I    v    0   )   k-s
	U(k) =  (   0    I    0   )   s
	(   0    0    I   )   n-k
	k-s   s   n-k

	If s = 1, D(k) overwrites A(k,k), and v overwrites A(1:k-1,k).
	If s = 2, the upper triangle of D(k) overwrites A(k-1,k-1), A(k-1,k),
	and A(k,k), and v overwrites A(1:k-2,k-1:k).

	If UPLO = 'L', then A = L*D*L', where
	L = P(1)*L(1)* ... *P(k)*L(k)* ...,
	i.e., L is a product of terms P(k)*L(k), where k increases from 1 to
	n in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1
	and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as
	defined by IPIV(k), and L(k) is a unit lower triangular matrix, such
	that if the diagonal block D(k) is of order s (s = 1 or 2), then

	(   I    0     0   )  k-1
	L(k) =  (   0    I     0   )  s
	(   0    v     I   )  n-k-s+1
	k-1   s  n-k-s+1

	If s = 1, D(k) overwrites A(k,k), and v overwrites A(k+1:n,k).
	If s = 2, the lower triangle of D(k) overwrites A(k,k), A(k+1,k),
	and A(k+1,k+1), and v overwrites A(k+2:n,k:k+1).

	=====================================================================


	Test the input parameters.

	Parameter adjustments */
	/* Table of constant values */
	static integer c__1 = 1;

	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5, i__6;
	real r__1, r__2, r__3, r__4;
	complex q__1, q__2, q__3, q__4, q__5, q__6;
	/* Builtin functions */
	double sqrt(doublereal);
	/* Local variables */
	static real d__;
	static integer i__, j, k;
	static complex t;
	static real r1, d11;
	static complex d12;
	static real d22;
	static complex d21;
	static integer kk, kp;
	static complex wk;
	static real tt;
	static complex wkm1, wkp1;
	static integer imax, jmax;
	static real alpha;
	static integer kstep;
	static logical upper;
	static real absakk;
	static real colmax;
	static real rowmax;


	a_dim1 = *lda;
	a_offset = 1 + a_dim1;
	a -= a_offset;
	--ipiv;

	/* Function Body */
	*info = 0;
	upper = lsame_(uplo, U_buffer);
	if (!upper && !lsame_(uplo, L_buffer)) {
		*info = -1;
	}
	else if (*n < 0) {
		*info = -2;
	}
	else if (*lda < max(1, *n)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = -(*info);
		xerbla_(CHETF2_buffer, &i__1);
		return 0;
	}

	/*     Initialize ALPHA for use in choosing pivot block size. */

	alpha = (sqrt(17.f) + 1.f) / 8.f;

	if (upper) {

		/*        Factorize A as U*D*U' using the upper triangle of A

		K is the main loop index, decreasing from N to 1 in steps of
		1 or 2 */

		k = *n;
	L10:

		/*        If K < 1, exit from loop */

		if (k < 1) {
			goto L90;
		}
		kstep = 1;

		/*        Determine rows and columns to be interchanged and whether
		a 1-by-1 or 2-by-2 pivot block will be used */

		i__1 = k + k * a_dim1;
		absakk = (r__1 = a[i__1].r, dabs(r__1));

		/*        IMAX is the row-index of the largest off-diagonal element in
		column K, and COLMAX is its absolute value */

		if (k > 1) {
			i__1 = k - 1;
			imax = icamax_(&i__1, &a[k * a_dim1 + 1], &c__1);
			i__1 = imax + k * a_dim1;
			colmax = (r__1 = a[i__1].r, dabs(r__1)) + (r__2 = r_imag(&a[imax
				+ k * a_dim1]), dabs(r__2));
		}
		else {
			colmax = 0.f;
		}

		if (dmax(absakk, colmax) == 0.f || sisnan_(&absakk)) {

			/*           Column K is zero or contains a NaN: set INFO and continue */

			if (*info == 0) {
				*info = k;
			}
			kp = k;
			i__1 = k + k * a_dim1;
			i__2 = k + k * a_dim1;
			r__1 = a[i__2].r;
			a[i__1].r = r__1, a[i__1].i = 0.f;
		}
		else {
			if (absakk >= alpha * colmax) {

				/*              no interchange, use 1-by-1 pivot block */

				kp = k;
			}
			else {

				/*              JMAX is the column-index of the largest off-diagonal
				element in row IMAX, and ROWMAX is its absolute value */

				i__1 = k - imax;
				jmax = imax + icamax_(&i__1, &a[imax + (imax + 1) * a_dim1],
					lda);
				i__1 = imax + jmax * a_dim1;
				rowmax = (r__1 = a[i__1].r, dabs(r__1)) + (r__2 = r_imag(&a[
					imax + jmax * a_dim1]), dabs(r__2));
				if (imax > 1) {
					i__1 = imax - 1;
					jmax = icamax_(&i__1, &a[imax * a_dim1 + 1], &c__1);
					/* Computing MAX */
					i__1 = jmax + imax * a_dim1;
					r__3 = rowmax, r__4 = (r__1 = a[i__1].r, dabs(r__1)) + (
						r__2 = r_imag(&a[jmax + imax * a_dim1]), dabs(
							r__2));
					rowmax = dmax(r__3, r__4);
				}

				if (absakk >= alpha * colmax * (colmax / rowmax)) {

					/*                 no interchange, use 1-by-1 pivot block */

					kp = k;
				}
				else /* if(complicated condition) */ {
					i__1 = imax + imax * a_dim1;
					if ((r__1 = a[i__1].r, dabs(r__1)) >= alpha * rowmax) {

						/*                 interchange rows and columns K and IMAX, use 1-by-1
						pivot block */

						kp = imax;
					}
					else {

						/*                 interchange rows and columns K-1 and IMAX, use 2-by-2
						pivot block */

						kp = imax;
						kstep = 2;
					}
				}
			}

			kk = k - kstep + 1;
			if (kp != kk) {

				/*              Interchange rows and columns KK and KP in the leading
				submatrix A(1:k,1:k) */

				i__1 = kp - 1;
				cswap_(&i__1, &a[kk * a_dim1 + 1], &c__1, &a[kp * a_dim1 + 1],
					&c__1);
				i__1 = kk - 1;
				for (j = kp + 1; j <= i__1; ++j) {
					r_cnjg(&q__1, &a[j + kk * a_dim1]);
					t.r = q__1.r, t.i = q__1.i;
					i__2 = j + kk * a_dim1;
					r_cnjg(&q__1, &a[kp + j * a_dim1]);
					a[i__2].r = q__1.r, a[i__2].i = q__1.i;
					i__2 = kp + j * a_dim1;
					a[i__2].r = t.r, a[i__2].i = t.i;
					/* L20: */
				}
				i__1 = kp + kk * a_dim1;
				r_cnjg(&q__1, &a[kp + kk * a_dim1]);
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
				i__1 = kk + kk * a_dim1;
				r1 = a[i__1].r;
				i__1 = kk + kk * a_dim1;
				i__2 = kp + kp * a_dim1;
				r__1 = a[i__2].r;
				a[i__1].r = r__1, a[i__1].i = 0.f;
				i__1 = kp + kp * a_dim1;
				a[i__1].r = r1, a[i__1].i = 0.f;
				if (kstep == 2) {
					i__1 = k + k * a_dim1;
					i__2 = k + k * a_dim1;
					r__1 = a[i__2].r;
					a[i__1].r = r__1, a[i__1].i = 0.f;
					i__1 = k - 1 + k * a_dim1;
					t.r = a[i__1].r, t.i = a[i__1].i;
					i__1 = k - 1 + k * a_dim1;
					i__2 = kp + k * a_dim1;
					a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
					i__1 = kp + k * a_dim1;
					a[i__1].r = t.r, a[i__1].i = t.i;
				}
			}
			else {
				i__1 = k + k * a_dim1;
				i__2 = k + k * a_dim1;
				r__1 = a[i__2].r;
				a[i__1].r = r__1, a[i__1].i = 0.f;
				if (kstep == 2) {
					i__1 = k - 1 + (k - 1) * a_dim1;
					i__2 = k - 1 + (k - 1) * a_dim1;
					r__1 = a[i__2].r;
					a[i__1].r = r__1, a[i__1].i = 0.f;
				}
			}

			/*           Update the leading submatrix */

			if (kstep == 1) {

				/*              1-by-1 pivot block D(k): column k now holds

				W(k) = U(k)*D(k)

				where U(k) is the k-th column of U

				Perform a rank-1 update of A(1:k-1,1:k-1) as

				A := A - U(k)*D(k)*U(k)' = A - W(k)*1/D(k)*W(k)' */

				i__1 = k + k * a_dim1;
				r1 = 1.f / a[i__1].r;
				i__1 = k - 1;
				r__1 = -r1;
				cher_(uplo, &i__1, &r__1, &a[k * a_dim1 + 1], &c__1, &a[
					a_offset], lda);

				/*              Store U(k) in column k */

				i__1 = k - 1;
				csscal_(&i__1, &r1, &a[k * a_dim1 + 1], &c__1);
			}
			else {

				/*              2-by-2 pivot block D(k): columns k and k-1 now hold

				( W(k-1) W(k) ) = ( U(k-1) U(k) )*D(k)

				where U(k) and U(k-1) are the k-th and (k-1)-th columns
				of U

				Perform a rank-2 update of A(1:k-2,1:k-2) as

				A := A - ( U(k-1) U(k) )*D(k)*( U(k-1) U(k) )'
				= A - ( W(k-1) W(k) )*inv(D(k))*( W(k-1) W(k) )' */

				if (k > 2) {

					i__1 = k - 1 + k * a_dim1;
					r__1 = a[i__1].r;
					r__2 = r_imag(&a[k - 1 + k * a_dim1]);
					d__ = slapy2_(&r__1, &r__2);
					i__1 = k - 1 + (k - 1) * a_dim1;
					d22 = a[i__1].r / d__;
					i__1 = k + k * a_dim1;
					d11 = a[i__1].r / d__;
					tt = 1.f / (d11 * d22 - 1.f);
					i__1 = k - 1 + k * a_dim1;
					q__1.r = a[i__1].r / d__, q__1.i = a[i__1].i / d__;
					d12.r = q__1.r, d12.i = q__1.i;
					d__ = tt / d__;

					for (j = k - 2; j >= 1; --j) {
						i__1 = j + (k - 1) * a_dim1;
						q__3.r = d11 * a[i__1].r, q__3.i = d11 * a[i__1].i;
						r_cnjg(&q__5, &d12);
						i__2 = j + k * a_dim1;
						q__4.r = q__5.r * a[i__2].r - q__5.i * a[i__2].i,
							q__4.i = q__5.r * a[i__2].i + q__5.i * a[i__2]
							.r;
						q__2.r = q__3.r - q__4.r, q__2.i = q__3.i - q__4.i;
						q__1.r = d__ * q__2.r, q__1.i = d__ * q__2.i;
						wkm1.r = q__1.r, wkm1.i = q__1.i;
						i__1 = j + k * a_dim1;
						q__3.r = d22 * a[i__1].r, q__3.i = d22 * a[i__1].i;
						i__2 = j + (k - 1) * a_dim1;
						q__4.r = d12.r * a[i__2].r - d12.i * a[i__2].i,
							q__4.i = d12.r * a[i__2].i + d12.i * a[i__2]
							.r;
						q__2.r = q__3.r - q__4.r, q__2.i = q__3.i - q__4.i;
						q__1.r = d__ * q__2.r, q__1.i = d__ * q__2.i;
						wk.r = q__1.r, wk.i = q__1.i;
						for (i__ = j; i__ >= 1; --i__) {
							i__1 = i__ + j * a_dim1;
							i__2 = i__ + j * a_dim1;
							i__3 = i__ + k * a_dim1;
							r_cnjg(&q__4, &wk);
							q__3.r = a[i__3].r * q__4.r - a[i__3].i * q__4.i,
								q__3.i = a[i__3].r * q__4.i + a[i__3].i *
								q__4.r;
							q__2.r = a[i__2].r - q__3.r, q__2.i = a[i__2].i -
								q__3.i;
							i__4 = i__ + (k - 1) * a_dim1;
							r_cnjg(&q__6, &wkm1);
							q__5.r = a[i__4].r * q__6.r - a[i__4].i * q__6.i,
								q__5.i = a[i__4].r * q__6.i + a[i__4].i *
								q__6.r;
							q__1.r = q__2.r - q__5.r, q__1.i = q__2.i -
								q__5.i;
							a[i__1].r = q__1.r, a[i__1].i = q__1.i;
							/* L30: */
						}
						i__1 = j + k * a_dim1;
						a[i__1].r = wk.r, a[i__1].i = wk.i;
						i__1 = j + (k - 1) * a_dim1;
						a[i__1].r = wkm1.r, a[i__1].i = wkm1.i;
						i__1 = j + j * a_dim1;
						i__2 = j + j * a_dim1;
						r__1 = a[i__2].r;
						q__1.r = r__1, q__1.i = 0.f;
						a[i__1].r = q__1.r, a[i__1].i = q__1.i;
						/* L40: */
					}

				}

			}
		}

		/*        Store details of the interchanges in IPIV */

		if (kstep == 1) {
			ipiv[k] = kp;
		}
		else {
			ipiv[k] = -kp;
			ipiv[k - 1] = -kp;
		}

		/*        Decrease K and return to the start of the main loop */

		k -= kstep;
		goto L10;

	}
	else {

		/*        Factorize A as L*D*L' using the lower triangle of A

		K is the main loop index, increasing from 1 to N in steps of
		1 or 2 */

		k = 1;
	L50:

		/*        If K > N, exit from loop */

		if (k > *n) {
			goto L90;
		}
		kstep = 1;

		/*        Determine rows and columns to be interchanged and whether
		a 1-by-1 or 2-by-2 pivot block will be used */

		i__1 = k + k * a_dim1;
		absakk = (r__1 = a[i__1].r, dabs(r__1));

		/*        IMAX is the row-index of the largest off-diagonal element in
		column K, and COLMAX is its absolute value */

		if (k < *n) {
			i__1 = *n - k;
			imax = k + icamax_(&i__1, &a[k + 1 + k * a_dim1], &c__1);
			i__1 = imax + k * a_dim1;
			colmax = (r__1 = a[i__1].r, dabs(r__1)) + (r__2 = r_imag(&a[imax
				+ k * a_dim1]), dabs(r__2));
		}
		else {
			colmax = 0.f;
		}

		if (dmax(absakk, colmax) == 0.f || sisnan_(&absakk)) {

			/*           Column K is zero or contains a NaN: set INFO and continue */

			if (*info == 0) {
				*info = k;
			}
			kp = k;
			i__1 = k + k * a_dim1;
			i__2 = k + k * a_dim1;
			r__1 = a[i__2].r;
			a[i__1].r = r__1, a[i__1].i = 0.f;
		}
		else {
			if (absakk >= alpha * colmax) {

				/*              no interchange, use 1-by-1 pivot block */

				kp = k;
			}
			else {

				/*              JMAX is the column-index of the largest off-diagonal
				element in row IMAX, and ROWMAX is its absolute value */

				i__1 = imax - k;
				jmax = k - 1 + icamax_(&i__1, &a[imax + k * a_dim1], lda);
				i__1 = imax + jmax * a_dim1;
				rowmax = (r__1 = a[i__1].r, dabs(r__1)) + (r__2 = r_imag(&a[
					imax + jmax * a_dim1]), dabs(r__2));
				if (imax < *n) {
					i__1 = *n - imax;
					jmax = imax + icamax_(&i__1, &a[imax + 1 + imax * a_dim1],
						&c__1);
					/* Computing MAX */
					i__1 = jmax + imax * a_dim1;
					r__3 = rowmax, r__4 = (r__1 = a[i__1].r, dabs(r__1)) + (
						r__2 = r_imag(&a[jmax + imax * a_dim1]), dabs(
							r__2));
					rowmax = dmax(r__3, r__4);
				}

				if (absakk >= alpha * colmax * (colmax / rowmax)) {

					/*                 no interchange, use 1-by-1 pivot block */

					kp = k;
				}
				else /* if(complicated condition) */ {
					i__1 = imax + imax * a_dim1;
					if ((r__1 = a[i__1].r, dabs(r__1)) >= alpha * rowmax) {

						/*                 interchange rows and columns K and IMAX, use 1-by-1
						pivot block */

						kp = imax;
					}
					else {

						/*                 interchange rows and columns K+1 and IMAX, use 2-by-2
						pivot block */

						kp = imax;
						kstep = 2;
					}
				}
			}

			kk = k + kstep - 1;
			if (kp != kk) {

				/*              Interchange rows and columns KK and KP in the trailing
				submatrix A(k:n,k:n) */

				if (kp < *n) {
					i__1 = *n - kp;
					cswap_(&i__1, &a[kp + 1 + kk * a_dim1], &c__1, &a[kp + 1
						+ kp * a_dim1], &c__1);
				}
				i__1 = kp - 1;
				for (j = kk + 1; j <= i__1; ++j) {
					r_cnjg(&q__1, &a[j + kk * a_dim1]);
					t.r = q__1.r, t.i = q__1.i;
					i__2 = j + kk * a_dim1;
					r_cnjg(&q__1, &a[kp + j * a_dim1]);
					a[i__2].r = q__1.r, a[i__2].i = q__1.i;
					i__2 = kp + j * a_dim1;
					a[i__2].r = t.r, a[i__2].i = t.i;
					/* L60: */
				}
				i__1 = kp + kk * a_dim1;
				r_cnjg(&q__1, &a[kp + kk * a_dim1]);
				a[i__1].r = q__1.r, a[i__1].i = q__1.i;
				i__1 = kk + kk * a_dim1;
				r1 = a[i__1].r;
				i__1 = kk + kk * a_dim1;
				i__2 = kp + kp * a_dim1;
				r__1 = a[i__2].r;
				a[i__1].r = r__1, a[i__1].i = 0.f;
				i__1 = kp + kp * a_dim1;
				a[i__1].r = r1, a[i__1].i = 0.f;
				if (kstep == 2) {
					i__1 = k + k * a_dim1;
					i__2 = k + k * a_dim1;
					r__1 = a[i__2].r;
					a[i__1].r = r__1, a[i__1].i = 0.f;
					i__1 = k + 1 + k * a_dim1;
					t.r = a[i__1].r, t.i = a[i__1].i;
					i__1 = k + 1 + k * a_dim1;
					i__2 = kp + k * a_dim1;
					a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
					i__1 = kp + k * a_dim1;
					a[i__1].r = t.r, a[i__1].i = t.i;
				}
			}
			else {
				i__1 = k + k * a_dim1;
				i__2 = k + k * a_dim1;
				r__1 = a[i__2].r;
				a[i__1].r = r__1, a[i__1].i = 0.f;
				if (kstep == 2) {
					i__1 = k + 1 + (k + 1) * a_dim1;
					i__2 = k + 1 + (k + 1) * a_dim1;
					r__1 = a[i__2].r;
					a[i__1].r = r__1, a[i__1].i = 0.f;
				}
			}

			/*           Update the trailing submatrix */

			if (kstep == 1) {

				/*              1-by-1 pivot block D(k): column k now holds

				W(k) = L(k)*D(k)

				where L(k) is the k-th column of L */

				if (k < *n) {

					/*                 Perform a rank-1 update of A(k+1:n,k+1:n) as

					A := A - L(k)*D(k)*L(k)' = A - W(k)*(1/D(k))*W(k)' */

					i__1 = k + k * a_dim1;
					r1 = 1.f / a[i__1].r;
					i__1 = *n - k;
					r__1 = -r1;
					cher_(uplo, &i__1, &r__1, &a[k + 1 + k * a_dim1], &c__1, &
						a[k + 1 + (k + 1) * a_dim1], lda);

					/*                 Store L(k) in column K */

					i__1 = *n - k;
					csscal_(&i__1, &r1, &a[k + 1 + k * a_dim1], &c__1);
				}
			}
			else {

				/*              2-by-2 pivot block D(k) */

				if (k < *n - 1) {

					/*                 Perform a rank-2 update of A(k+2:n,k+2:n) as

					A := A - ( L(k) L(k+1) )*D(k)*( L(k) L(k+1) )'
					= A - ( W(k) W(k+1) )*inv(D(k))*( W(k) W(k+1) )'

					where L(k) and L(k+1) are the k-th and (k+1)-th
					columns of L */

					i__1 = k + 1 + k * a_dim1;
					r__1 = a[i__1].r;
					r__2 = r_imag(&a[k + 1 + k * a_dim1]);
					d__ = slapy2_(&r__1, &r__2);
					i__1 = k + 1 + (k + 1) * a_dim1;
					d11 = a[i__1].r / d__;
					i__1 = k + k * a_dim1;
					d22 = a[i__1].r / d__;
					tt = 1.f / (d11 * d22 - 1.f);
					i__1 = k + 1 + k * a_dim1;
					q__1.r = a[i__1].r / d__, q__1.i = a[i__1].i / d__;
					d21.r = q__1.r, d21.i = q__1.i;
					d__ = tt / d__;

					i__1 = *n;
					for (j = k + 2; j <= i__1; ++j) {
						i__2 = j + k * a_dim1;
						q__3.r = d11 * a[i__2].r, q__3.i = d11 * a[i__2].i;
						i__3 = j + (k + 1) * a_dim1;
						q__4.r = d21.r * a[i__3].r - d21.i * a[i__3].i,
							q__4.i = d21.r * a[i__3].i + d21.i * a[i__3]
							.r;
						q__2.r = q__3.r - q__4.r, q__2.i = q__3.i - q__4.i;
						q__1.r = d__ * q__2.r, q__1.i = d__ * q__2.i;
						wk.r = q__1.r, wk.i = q__1.i;
						i__2 = j + (k + 1) * a_dim1;
						q__3.r = d22 * a[i__2].r, q__3.i = d22 * a[i__2].i;
						r_cnjg(&q__5, &d21);
						i__3 = j + k * a_dim1;
						q__4.r = q__5.r * a[i__3].r - q__5.i * a[i__3].i,
							q__4.i = q__5.r * a[i__3].i + q__5.i * a[i__3]
							.r;
						q__2.r = q__3.r - q__4.r, q__2.i = q__3.i - q__4.i;
						q__1.r = d__ * q__2.r, q__1.i = d__ * q__2.i;
						wkp1.r = q__1.r, wkp1.i = q__1.i;
						i__2 = *n;
						for (i__ = j; i__ <= i__2; ++i__) {
							i__3 = i__ + j * a_dim1;
							i__4 = i__ + j * a_dim1;
							i__5 = i__ + k * a_dim1;
							r_cnjg(&q__4, &wk);
							q__3.r = a[i__5].r * q__4.r - a[i__5].i * q__4.i,
								q__3.i = a[i__5].r * q__4.i + a[i__5].i *
								q__4.r;
							q__2.r = a[i__4].r - q__3.r, q__2.i = a[i__4].i -
								q__3.i;
							i__6 = i__ + (k + 1) * a_dim1;
							r_cnjg(&q__6, &wkp1);
							q__5.r = a[i__6].r * q__6.r - a[i__6].i * q__6.i,
								q__5.i = a[i__6].r * q__6.i + a[i__6].i *
								q__6.r;
							q__1.r = q__2.r - q__5.r, q__1.i = q__2.i -
								q__5.i;
							a[i__3].r = q__1.r, a[i__3].i = q__1.i;
							/* L70: */
						}
						i__2 = j + k * a_dim1;
						a[i__2].r = wk.r, a[i__2].i = wk.i;
						i__2 = j + (k + 1) * a_dim1;
						a[i__2].r = wkp1.r, a[i__2].i = wkp1.i;
						i__2 = j + j * a_dim1;
						i__3 = j + j * a_dim1;
						r__1 = a[i__3].r;
						q__1.r = r__1, q__1.i = 0.f;
						a[i__2].r = q__1.r, a[i__2].i = q__1.i;
						/* L80: */
					}
				}
			}
		}

		/*        Store details of the interchanges in IPIV */

		if (kstep == 1) {
			ipiv[k] = kp;
		}
		else {
			ipiv[k] = -kp;
			ipiv[k + 1] = -kp;
		}

		/*        Increase K and return to the start of the main loop */

		k += kstep;
		goto L50;

	}

L90:
	return 0;

	/*     End of CHETF2 */

} /* chetf2_ */

#ifndef SIGIOT
#ifdef SIGABRT
#define SIGIOT SIGABRT
#endif
#endif

static int copy(FILE *from, register long len, FILE *to)
{
	int len1;
	char buf[BUFSIZ];

	while (fread(buf, len1 = len > BUFSIZ ? BUFSIZ : (int)len, 1, from)) {
		if (!fwrite(buf, len1, 1, to))
			return 1;
		if ((len -= len1) <= 0)
			break;
	}
	return 0;
}
int t_runc(alist *a)
{
	OFF_T loc, len;
	unit *b;
	int rc;
	FILE *bf;
#ifdef NO_TRUNCATE
	FILE *tf;
#endif
	FILE *f__cf;	/*current file*/

	b = &f__units[a->aunit];
	if (b->url)
		return(0);	/*don't truncate direct files*/
	loc = FTELL(bf = b->ufd);
	FSEEK(bf, (OFF_T)0, SEEK_END);
	len = FTELL(bf);
	if (loc >= len || b->useek == 0)
		return(0);
#ifdef NO_TRUNCATE
	if (b->ufnm == NULL)
		return 0;
	rc = 0;
	fclose(b->ufd);
	if (!loc) {
		if (!(bf = FOPEN(b->ufnm, f__w_mode[b->ufmt])))
			rc = 1;
		if (b->uwrt)
			b->uwrt = 1;
		goto done;
	}
	if ((!(bf = FOPEN(b->ufnm, f__r_mode[0])))||( !(tf = tmpfile()))) 
	{
#ifdef NON_UNIX_STDIO
		bad :
#endif
			rc = 1;
			goto done;
	}
	if (copy(bf, (long)loc, tf)) {
	bad1:
		rc = 1;
		goto done1;
	}
	if (!(bf = FREOPEN(b->ufnm, f__w_mode[0], bf)))
		goto bad1;
	rewind(tf);
	if (copy(tf, (long)loc, bf))
		goto bad1;
	b->uwrt = 1;
	b->urw = 2;
#ifdef NON_UNIX_STDIO
	if (b->ufmt) {
		fclose(bf);
		if (!(bf = FOPEN(b->ufnm, f__w_mode[3])))
			goto bad;
		FSEEK(bf, (OFF_T)0, SEEK_END);
		b->urw = 3;
	}
#endif
done1:
	fclose(tf);
done:
	f__cf = b->ufd = bf;
#else /* NO_TRUNCATE */
	if (b->urw & 2)
		fflush(b->ufd); /* necessary on some Linux systems */
#ifndef FTRUNCATE
#define FTRUNCATE ftruncate
#endif
	rc = FTRUNCATE(fileno(b->ufd), loc);
	/* The following FSEEK is unnecessary on some systems, */
	/* but should be harmless. */
	FSEEK(b->ufd, (OFF_T)0, SEEK_END);
#endif /* NO_TRUNCATE */
	//if (rc)
		//err(a->aerr, 111, "endfile");
	return 0;
}

integer f_clos(cllist *a)
{
	unit *b;

	if (a->cunit >= MXUNIT) return(0);
	b = &f__units[a->cunit];
	if (b->ufd == NULL)
		goto done;
	if (b->uscrtch == 1)
		goto Delete;
	if (!a->csta)
		goto Keep;
	switch (*a->csta) {
	default:
	Keep:
	case 'k':
	case 'K':
		if (b->uwrt == 1)
			t_runc((alist *)a);
		if (b->ufnm) {
			fclose(b->ufd);
			free(b->ufnm);
		}
		break;
	case 'd':
	case 'D':
	Delete:
		fclose(b->ufd);
		if (b->ufnm) {
			//unlink(b->ufnm); /*SYSDEP*/
			free(b->ufnm);
		}
	}
	b->ufd = NULL;
done:
	b->uend = 0;
	b->ufnm = NULL;
	return(0);
}

void f_exit(void)
{
	int i;
	static cllist xx;
	if (!xx.cerr) {
		xx.cerr = 1;
		xx.csta = NULL;
		for (i = 0; i<MXUNIT; i++)
		{
			xx.cunit = i;
			(void)f_clos(&xx);
		}
	}
}

void sig_die(register char *s, int kill)
{
	/* print error message, then clear buffers */
	fprintf(stderr, "%s\n", s);

	if (kill)
	{
		fflush(stderr);
		f_exit();
		fflush(stderr);
		/* now get a core */
#ifdef SIGIOT
		signal(SIGIOT, 0);
#endif
		abort();
	}
	else {
#ifdef NO_ONEXIT
		f_exit();
#endif
		exit(1);
	}
}

void c_div(complex *c, complex *a, complex *b)
{
	double ratio, den;
	double abr, abi, cr;

	if ((abr = b->r) < 0.)
		abr = -abr;
	if ((abi = b->i) < 0.)
		abi = -abi;
	if (abr <= abi)
	{
		if (abi == 0) {
			sig_die(complexdivisionbyzero_buffer, 1);
		}
		ratio = (double)b->r / b->i;
		den = b->i * (1 + ratio*ratio);
		cr = (a->r*ratio + a->i) / den;
		c->i = (a->i*ratio - a->r) / den;
	}

	else
	{
		ratio = (double)b->i / b->r;
		den = b->r * (1 + ratio*ratio);
		cr = (a->r + a->i*ratio) / den;
		c->i = (a->i - a->r*ratio) / den;
	}
	c->r = cr;
}


/* Subroutine */ int cgemm_(char *transa, char *transb, integer *m, integer *
	n, integer *k, complex *alpha, complex *a, integer *lda, complex *b,
	integer *ldb, complex *beta, complex *c__, integer *ldc)
{
	/* System generated locals */
	integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, i__1, i__2,
		i__3, i__4, i__5, i__6;
	complex q__1, q__2, q__3, q__4;
	/* Builtin functions */
	/* Local variables */
	static integer i__, j, l, info;
	static logical nota, notb;
	static complex temp;
	static logical conja, conjb;
	static integer ncola;
	static integer nrowa, nrowb;
	/*  Purpose
	=======
	CGEMM  performs one of the matrix-matrix operations
	C := alpha*op( A )*op( B ) + beta*C,
	where  op( X ) is one of
	op( X ) = X   or   op( X ) = X'   or   op( X ) = conjg( X' ),
	alpha and beta are scalars, and A, B and C are matrices, with op( A )
	an m by k matrix,  op( B )  a  k by n matrix and  C an m by n matrix.
	Arguments
	==========
	TRANSA - CHARACTER*1.
	On entry, TRANSA specifies the form of op( A ) to be used in
	the matrix multiplication as follows:
	TRANSA = 'N' or 'n',  op( A ) = A.
	TRANSA = 'T' or 't',  op( A ) = A'.
	TRANSA = 'C' or 'c',  op( A ) = conjg( A' ).
	Unchanged on exit.
	TRANSB - CHARACTER*1.
	On entry, TRANSB specifies the form of op( B ) to be used in
	the matrix multiplication as follows:
	TRANSB = 'N' or 'n',  op( B ) = B.
	TRANSB = 'T' or 't',  op( B ) = B'.
	TRANSB = 'C' or 'c',  op( B ) = conjg( B' ).
	Unchanged on exit.
	M      - INTEGER.
	On entry,  M  specifies  the number  of rows  of the  matrix
	op( A )  and of the  matrix  C.  M  must  be at least  zero.
	Unchanged on exit.
	N      - INTEGER.
	On entry,  N  specifies the number  of columns of the matrix
	op( B ) and the number of columns of the matrix C. N must be
	at least zero.
	Unchanged on exit.
	K      - INTEGER.
	On entry,  K  specifies  the number of columns of the matrix
	op( A ) and the number of rows of the matrix op( B ). K must
	be at least  zero.
	Unchanged on exit.
	ALPHA  - COMPLEX         .
	On entry, ALPHA specifies the scalar alpha.
	Unchanged on exit.
	A      - COMPLEX          array of DIMENSION ( LDA, ka ), where ka is
	k  when  TRANSA = 'N' or 'n',  and is  m  otherwise.
	Before entry with  TRANSA = 'N' or 'n',  the leading  m by k
	part of the array  A  must contain the matrix  A,  otherwise
	the leading  k by m  part of the array  A  must contain  the
	matrix A.
	Unchanged on exit.
	LDA    - INTEGER.
	On entry, LDA specifies the first dimension of A as declared
	in the calling (sub) program. When  TRANSA = 'N' or 'n' then
	LDA must be at least  max( 1, m ), otherwise  LDA must be at
	least  max( 1, k ).
	Unchanged on exit.
	B      - COMPLEX          array of DIMENSION ( LDB, kb ), where kb is
	n  when  TRANSB = 'N' or 'n',  and is  k  otherwise.
	Before entry with  TRANSB = 'N' or 'n',  the leading  k by n
	part of the array  B  must contain the matrix  B,  otherwise
	the leading  n by k  part of the array  B  must contain  the
	matrix B.
	Unchanged on exit.
	LDB    - INTEGER.
	On entry, LDB specifies the first dimension of B as declared
	in the calling (sub) program. When  TRANSB = 'N' or 'n' then
	LDB must be at least  max( 1, k ), otherwise  LDB must be at
	least  max( 1, n ).
	Unchanged on exit.
	BETA   - COMPLEX         .
	On entry,  BETA  specifies the scalar  beta.  When  BETA  is
	supplied as zero then C need not be set on input.
	Unchanged on exit.
	C      - COMPLEX          array of DIMENSION ( LDC, n ).
	Before entry, the leading  m by n  part of the array  C must
	contain the matrix  C,  except when  beta  is zero, in which
	case C need not be set on entry.
	On exit, the array  C  is overwritten by the  m by n  matrix
	( alpha*op( A )*op( B ) + beta*C ).
	LDC    - INTEGER.
	On entry, LDC specifies the first dimension of C as declared
	in  the  calling  (sub)  program.   LDC  must  be  at  least
	max( 1, m ).
	Unchanged on exit.
	Level 3 Blas routine.
	-- Written on 8-February-1989.
	Jack Dongarra, Argonne National Laboratory.
	Iain Duff, AERE Harwell.
	Jeremy Du Croz, Numerical Algorithms Group Ltd.
	Sven Hammarling, Numerical Algorithms Group Ltd.
	Set  NOTA  and  NOTB  as  true if  A  and  B  respectively are not
	conjugated or transposed, set  CONJA and CONJB  as true if  A  and
	B  respectively are to be  transposed but  not conjugated  and set
	NROWA, NCOLA and  NROWB  as the number of rows and  columns  of  A
	and the number of rows of  B  respectively.
	Parameter adjustments */
	a_dim1 = *lda;
	a_offset = 1 + a_dim1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1;
	b -= b_offset;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1;
	c__ -= c_offset;
	/* Function Body */
	nota = lsame_(transa, N_buffer);
	notb = lsame_(transb, N_buffer);
	conja = lsame_(transa, C_buffer);
	conjb = lsame_(transb, C_buffer);
	if (nota) {
		nrowa = *m;
		ncola = *k;
	}
	else {
		nrowa = *k;
		ncola = *m;
	}
	if (notb) {
		nrowb = *k;
	}
	else {
		nrowb = *n;
	}
	/*     Test the input parameters. */
	info = 0;
	if (!nota && !conja && !lsame_(transa, T_buffer)) {
		info = 1;
	}
	else if (!notb && !conjb && !lsame_(transb, T_buffer)) {
		info = 2;
	}
	else if (*m < 0) {
		info = 3;
	}
	else if (*n < 0) {
		info = 4;
	}
	else if (*k < 0) {
		info = 5;
	}
	else if (*lda < max(1, nrowa)) {
		info = 8;
	}
	else if (*ldb < max(1, nrowb)) {
		info = 10;
	}
	else if (*ldc < max(1, *m)) {
		info = 13;
	}
	if (info != 0) {
		xerbla_(CGEMM_buffer, &info);
		return 0;
	}
	/*     Quick return if possible. */
	if (*m == 0 || *n == 0 || (alpha->r == 0.f && alpha->i == 0.f || *k == 0)
		&& (beta->r == 1.f && beta->i == 0.f)) {
		return 0;
	}
	/*     And when  alpha.eq.zero. */
	if (alpha->r == 0.f && alpha->i == 0.f) {
		if (beta->r == 0.f && beta->i == 0.f) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					i__3 = i__ + j * c_dim1;
					c__[i__3].r = 0.f, c__[i__3].i = 0.f;
					/* L10: */
				}
				/* L20: */
			}
		}
		else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					i__3 = i__ + j * c_dim1;
					i__4 = i__ + j * c_dim1;
					q__1.r = beta->r * c__[i__4].r - beta->i * c__[i__4].i,
						q__1.i = beta->r * c__[i__4].i + beta->i * c__[
							i__4].r;
					c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					/* L30: */
				}
				/* L40: */
			}
		}
		return 0;
	}
	/*     Start the operations. */
	if (notb) {
		if (nota) {
			/*           Form  C := alpha*A*B + beta*C. */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (beta->r == 0.f && beta->i == 0.f) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * c_dim1;
						c__[i__3].r = 0.f, c__[i__3].i = 0.f;
						/* L50: */
					}
				}
				else if (beta->r != 1.f || beta->i != 0.f) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * c_dim1;
						i__4 = i__ + j * c_dim1;
						q__1.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__1.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
						/* L60: */
					}
				}
				i__2 = *k;
				for (l = 1; l <= i__2; ++l) {
					i__3 = l + j * b_dim1;
					if (b[i__3].r != 0.f || b[i__3].i != 0.f) {
						i__3 = l + j * b_dim1;
						q__1.r = alpha->r * b[i__3].r - alpha->i * b[i__3].i,
							q__1.i = alpha->r * b[i__3].i + alpha->i * b[
								i__3].r;
						temp.r = q__1.r, temp.i = q__1.i;
						i__3 = *m;
						for (i__ = 1; i__ <= i__3; ++i__) {
							i__4 = i__ + j * c_dim1;
							i__5 = i__ + j * c_dim1;
							i__6 = i__ + l * a_dim1;
							q__2.r = temp.r * a[i__6].r - temp.i * a[i__6].i,
								q__2.i = temp.r * a[i__6].i + temp.i * a[
									i__6].r;
							q__1.r = c__[i__5].r + q__2.r, q__1.i = c__[i__5]
								.i + q__2.i;
							c__[i__4].r = q__1.r, c__[i__4].i = q__1.i;
							/* L70: */
						}
					}
					/* L80: */
				}
				/* L90: */
			}
		}
		else if (conja) {
			/*           Form  C := alpha*conjg( A' )*B + beta*C. */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp.r = 0.f, temp.i = 0.f;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						r_cnjg(&q__3, &a[l + i__ * a_dim1]);
						i__4 = l + j * b_dim1;
						q__2.r = q__3.r * b[i__4].r - q__3.i * b[i__4].i,
							q__2.i = q__3.r * b[i__4].i + q__3.i * b[i__4]
							.r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						/* L100: */
					}
					if (beta->r == 0.f && beta->i == 0.f) {
						i__3 = i__ + j * c_dim1;
						q__1.r = alpha->r * temp.r - alpha->i * temp.i,
							q__1.i = alpha->r * temp.i + alpha->i *
							temp.r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					else {
						i__3 = i__ + j * c_dim1;
						q__2.r = alpha->r * temp.r - alpha->i * temp.i,
							q__2.i = alpha->r * temp.i + alpha->i *
							temp.r;
						i__4 = i__ + j * c_dim1;
						q__3.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__3.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					/* L110: */
				}
				/* L120: */
			}
		}
		else {
			/*           Form  C := alpha*A'*B + beta*C */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp.r = 0.f, temp.i = 0.f;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						i__4 = l + i__ * a_dim1;
						i__5 = l + j * b_dim1;
						q__2.r = a[i__4].r * b[i__5].r - a[i__4].i * b[i__5]
							.i, q__2.i = a[i__4].r * b[i__5].i + a[i__4]
							.i * b[i__5].r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						/* L130: */
					}
					if (beta->r == 0.f && beta->i == 0.f) {
						i__3 = i__ + j * c_dim1;
						q__1.r = alpha->r * temp.r - alpha->i * temp.i,
							q__1.i = alpha->r * temp.i + alpha->i *
							temp.r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					else {
						i__3 = i__ + j * c_dim1;
						q__2.r = alpha->r * temp.r - alpha->i * temp.i,
							q__2.i = alpha->r * temp.i + alpha->i *
							temp.r;
						i__4 = i__ + j * c_dim1;
						q__3.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__3.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					/* L140: */
				}
				/* L150: */
			}
		}
	}
	else if (nota) {
		if (conjb) {
			/*           Form  C := alpha*A*conjg( B' ) + beta*C. */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (beta->r == 0.f && beta->i == 0.f) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * c_dim1;
						c__[i__3].r = 0.f, c__[i__3].i = 0.f;
						/* L160: */
					}
				}
				else if (beta->r != 1.f || beta->i != 0.f) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * c_dim1;
						i__4 = i__ + j * c_dim1;
						q__1.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__1.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
						/* L170: */
					}
				}
				i__2 = *k;
				for (l = 1; l <= i__2; ++l) {
					i__3 = j + l * b_dim1;
					if (b[i__3].r != 0.f || b[i__3].i != 0.f) {
						r_cnjg(&q__2, &b[j + l * b_dim1]);
						q__1.r = alpha->r * q__2.r - alpha->i * q__2.i,
							q__1.i = alpha->r * q__2.i + alpha->i *
							q__2.r;
						temp.r = q__1.r, temp.i = q__1.i;
						i__3 = *m;
						for (i__ = 1; i__ <= i__3; ++i__) {
							i__4 = i__ + j * c_dim1;
							i__5 = i__ + j * c_dim1;
							i__6 = i__ + l * a_dim1;
							q__2.r = temp.r * a[i__6].r - temp.i * a[i__6].i,
								q__2.i = temp.r * a[i__6].i + temp.i * a[
									i__6].r;
							q__1.r = c__[i__5].r + q__2.r, q__1.i = c__[i__5]
								.i + q__2.i;
							c__[i__4].r = q__1.r, c__[i__4].i = q__1.i;
							/* L180: */
						}
					}
					/* L190: */
				}
				/* L200: */
			}
		}
		else {
			/*           Form  C := alpha*A*B'          + beta*C */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (beta->r == 0.f && beta->i == 0.f) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * c_dim1;
						c__[i__3].r = 0.f, c__[i__3].i = 0.f;
						/* L210: */
					}
				}
				else if (beta->r != 1.f || beta->i != 0.f) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * c_dim1;
						i__4 = i__ + j * c_dim1;
						q__1.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__1.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
						/* L220: */
					}
				}
				i__2 = *k;
				for (l = 1; l <= i__2; ++l) {
					i__3 = j + l * b_dim1;
					if (b[i__3].r != 0.f || b[i__3].i != 0.f) {
						i__3 = j + l * b_dim1;
						q__1.r = alpha->r * b[i__3].r - alpha->i * b[i__3].i,
							q__1.i = alpha->r * b[i__3].i + alpha->i * b[
								i__3].r;
						temp.r = q__1.r, temp.i = q__1.i;
						i__3 = *m;
						for (i__ = 1; i__ <= i__3; ++i__) {
							i__4 = i__ + j * c_dim1;
							i__5 = i__ + j * c_dim1;
							i__6 = i__ + l * a_dim1;
							q__2.r = temp.r * a[i__6].r - temp.i * a[i__6].i,
								q__2.i = temp.r * a[i__6].i + temp.i * a[
									i__6].r;
							q__1.r = c__[i__5].r + q__2.r, q__1.i = c__[i__5]
								.i + q__2.i;
							c__[i__4].r = q__1.r, c__[i__4].i = q__1.i;
							/* L230: */
						}
					}
					/* L240: */
				}
				/* L250: */
			}
		}
	}
	else if (conja) {
		if (conjb) {
			/*           Form  C := alpha*conjg( A' )*conjg( B' ) + beta*C. */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp.r = 0.f, temp.i = 0.f;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						r_cnjg(&q__3, &a[l + i__ * a_dim1]);
						r_cnjg(&q__4, &b[j + l * b_dim1]);
						q__2.r = q__3.r * q__4.r - q__3.i * q__4.i, q__2.i =
							q__3.r * q__4.i + q__3.i * q__4.r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						/* L260: */
					}
					if (beta->r == 0.f && beta->i == 0.f) {
						i__3 = i__ + j * c_dim1;
						q__1.r = alpha->r * temp.r - alpha->i * temp.i,
							q__1.i = alpha->r * temp.i + alpha->i *
							temp.r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					else {
						i__3 = i__ + j * c_dim1;
						q__2.r = alpha->r * temp.r - alpha->i * temp.i,
							q__2.i = alpha->r * temp.i + alpha->i *
							temp.r;
						i__4 = i__ + j * c_dim1;
						q__3.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__3.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					/* L270: */
				}
				/* L280: */
			}
		}
		else {
			/*           Form  C := alpha*conjg( A' )*B' + beta*C */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp.r = 0.f, temp.i = 0.f;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						r_cnjg(&q__3, &a[l + i__ * a_dim1]);
						i__4 = j + l * b_dim1;
						q__2.r = q__3.r * b[i__4].r - q__3.i * b[i__4].i,
							q__2.i = q__3.r * b[i__4].i + q__3.i * b[i__4]
							.r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						/* L290: */
					}
					if (beta->r == 0.f && beta->i == 0.f) {
						i__3 = i__ + j * c_dim1;
						q__1.r = alpha->r * temp.r - alpha->i * temp.i,
							q__1.i = alpha->r * temp.i + alpha->i *
							temp.r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					else {
						i__3 = i__ + j * c_dim1;
						q__2.r = alpha->r * temp.r - alpha->i * temp.i,
							q__2.i = alpha->r * temp.i + alpha->i *
							temp.r;
						i__4 = i__ + j * c_dim1;
						q__3.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__3.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					/* L300: */
				}
				/* L310: */
			}
		}
	}
	else {
		if (conjb) {
			/*           Form  C := alpha*A'*conjg( B' ) + beta*C */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp.r = 0.f, temp.i = 0.f;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						i__4 = l + i__ * a_dim1;
						r_cnjg(&q__3, &b[j + l * b_dim1]);
						q__2.r = a[i__4].r * q__3.r - a[i__4].i * q__3.i,
							q__2.i = a[i__4].r * q__3.i + a[i__4].i *
							q__3.r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						/* L320: */
					}
					if (beta->r == 0.f && beta->i == 0.f) {
						i__3 = i__ + j * c_dim1;
						q__1.r = alpha->r * temp.r - alpha->i * temp.i,
							q__1.i = alpha->r * temp.i + alpha->i *
							temp.r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					else {
						i__3 = i__ + j * c_dim1;
						q__2.r = alpha->r * temp.r - alpha->i * temp.i,
							q__2.i = alpha->r * temp.i + alpha->i *
							temp.r;
						i__4 = i__ + j * c_dim1;
						q__3.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__3.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					/* L330: */
				}
				/* L340: */
			}
		}
		else {
			/*           Form  C := alpha*A'*B' + beta*C */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp.r = 0.f, temp.i = 0.f;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						i__4 = l + i__ * a_dim1;
						i__5 = j + l * b_dim1;
						q__2.r = a[i__4].r * b[i__5].r - a[i__4].i * b[i__5]
							.i, q__2.i = a[i__4].r * b[i__5].i + a[i__4]
							.i * b[i__5].r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						/* L350: */
					}
					if (beta->r == 0.f && beta->i == 0.f) {
						i__3 = i__ + j * c_dim1;
						q__1.r = alpha->r * temp.r - alpha->i * temp.i,
							q__1.i = alpha->r * temp.i + alpha->i *
							temp.r;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					else {
						i__3 = i__ + j * c_dim1;
						q__2.r = alpha->r * temp.r - alpha->i * temp.i,
							q__2.i = alpha->r * temp.i + alpha->i *
							temp.r;
						i__4 = i__ + j * c_dim1;
						q__3.r = beta->r * c__[i__4].r - beta->i * c__[i__4]
							.i, q__3.i = beta->r * c__[i__4].i + beta->i *
							c__[i__4].r;
						q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
						c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
					}
					/* L360: */
				}
				/* L370: */
			}
		}
	}
	return 0;
	/*     End of CGEMM . */
} /* cgemm_ */


/* Subroutine */ int cgemv_(char *trans, integer *m, integer *n, complex *
	alpha, complex *a, integer *lda, complex *x, integer *incx, complex *
	beta, complex *y, integer *incy)
{
	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;
	complex q__1, q__2, q__3;
	/* Builtin functions */
	/* Local variables */
	static integer i__, j, ix, iy, jx, jy, kx, ky, info;
	static complex temp;
	static integer lenx, leny;
	static logical noconj;
	/*  Purpose
	=======
	CGEMV performs one of the matrix-vector operations
	y := alpha*A*x + beta*y,   or   y := alpha*A'*x + beta*y,   or
	y := alpha*conjg( A' )*x + beta*y,
	where alpha and beta are scalars, x and y are vectors and A is an
	m by n matrix.
	Arguments
	==========
	TRANS  - CHARACTER*1.
	On entry, TRANS specifies the operation to be performed as
	follows:
	TRANS = 'N' or 'n'   y := alpha*A*x + beta*y.
	TRANS = 'T' or 't'   y := alpha*A'*x + beta*y.
	TRANS = 'C' or 'c'   y := alpha*conjg( A' )*x + beta*y.
	Unchanged on exit.
	M      - INTEGER.
	On entry, M specifies the number of rows of the matrix A.
	M must be at least zero.
	Unchanged on exit.
	N      - INTEGER.
	On entry, N specifies the number of columns of the matrix A.
	N must be at least zero.
	Unchanged on exit.
	ALPHA  - COMPLEX         .
	On entry, ALPHA specifies the scalar alpha.
	Unchanged on exit.
	A      - COMPLEX          array of DIMENSION ( LDA, n ).
	Before entry, the leading m by n part of the array A must
	contain the matrix of coefficients.
	Unchanged on exit.
	LDA    - INTEGER.
	On entry, LDA specifies the first dimension of A as declared
	in the calling (sub) program. LDA must be at least
	max( 1, m ).
	Unchanged on exit.
	X      - COMPLEX          array of DIMENSION at least
	( 1 + ( n - 1 )*abs( INCX ) ) when TRANS = 'N' or 'n'
	and at least
	( 1 + ( m - 1 )*abs( INCX ) ) otherwise.
	Before entry, the incremented array X must contain the
	vector x.
	Unchanged on exit.
	INCX   - INTEGER.
	On entry, INCX specifies the increment for the elements of
	X. INCX must not be zero.
	Unchanged on exit.
	BETA   - COMPLEX         .
	On entry, BETA specifies the scalar beta. When BETA is
	supplied as zero then Y need not be set on input.
	Unchanged on exit.
	Y      - COMPLEX          array of DIMENSION at least
	( 1 + ( m - 1 )*abs( INCY ) ) when TRANS = 'N' or 'n'
	and at least
	( 1 + ( n - 1 )*abs( INCY ) ) otherwise.
	Before entry with BETA non-zero, the incremented array Y
	must contain the vector y. On exit, Y is overwritten by the
	updated vector y.
	INCY   - INTEGER.
	On entry, INCY specifies the increment for the elements of
	Y. INCY must not be zero.
	Unchanged on exit.
	Level 2 Blas routine.
	-- Written on 22-October-1986.
	Jack Dongarra, Argonne National Lab.
	Jeremy Du Croz, Nag Central Office.
	Sven Hammarling, Nag Central Office.
	Richard Hanson, Sandia National Labs.
	Test the input parameters.
	Parameter adjustments */
	a_dim1 = *lda;
	a_offset = 1 + a_dim1;
	a -= a_offset;
	--x;
	--y;
	/* Function Body */
	info = 0;
	if (!lsame_(trans, N_buffer) && !lsame_(trans, T_buffer) && !lsame_(trans, C_buffer)
		) {
		info = 1;
	}
	else if (*m < 0) {
		info = 2;
	}
	else if (*n < 0) {
		info = 3;
	}
	else if (*lda < max(1, *m)) {
		info = 6;
	}
	else if (*incx == 0) {
		info = 8;
	}
	else if (*incy == 0) {
		info = 11;
	}
	if (info != 0) {
		xerbla_(CGEMV1_buffer, &info);
		return 0;
	}
	/*     Quick return if possible. */
	if (*m == 0 || *n == 0 || alpha->r == 0.f && alpha->i == 0.f && (beta->r
		== 1.f && beta->i == 0.f)) {
		return 0;
	}
	noconj = lsame_(trans, T_buffer);
	/*     Set  LENX  and  LENY, the lengths of the vectors x and y, and set
	up the start points in  X  and  Y. */
	if (lsame_(trans, N_buffer)) {
		lenx = *n;
		leny = *m;
	}
	else {
		lenx = *m;
		leny = *n;
	}
	if (*incx > 0) {
		kx = 1;
	}
	else {
		kx = 1 - (lenx - 1) * *incx;
	}
	if (*incy > 0) {
		ky = 1;
	}
	else {
		ky = 1 - (leny - 1) * *incy;
	}
	/*     Start the operations. In this version the elements of A are
	accessed sequentially with one pass through A.
	First form  y := beta*y. */
	if (beta->r != 1.f || beta->i != 0.f) {
		if (*incy == 1) {
			if (beta->r == 0.f && beta->i == 0.f) {
				i__1 = leny;
				for (i__ = 1; i__ <= i__1; ++i__) {
					i__2 = i__;
					y[i__2].r = 0.f, y[i__2].i = 0.f;
					/* L10: */
				}
			}
			else {
				i__1 = leny;
				for (i__ = 1; i__ <= i__1; ++i__) {
					i__2 = i__;
					i__3 = i__;
					q__1.r = beta->r * y[i__3].r - beta->i * y[i__3].i,
						q__1.i = beta->r * y[i__3].i + beta->i * y[i__3]
						.r;
					y[i__2].r = q__1.r, y[i__2].i = q__1.i;
					/* L20: */
				}
			}
		}
		else {
			iy = ky;
			if (beta->r == 0.f && beta->i == 0.f) {
				i__1 = leny;
				for (i__ = 1; i__ <= i__1; ++i__) {
					i__2 = iy;
					y[i__2].r = 0.f, y[i__2].i = 0.f;
					iy += *incy;
					/* L30: */
				}
			}
			else {
				i__1 = leny;
				for (i__ = 1; i__ <= i__1; ++i__) {
					i__2 = iy;
					i__3 = iy;
					q__1.r = beta->r * y[i__3].r - beta->i * y[i__3].i,
						q__1.i = beta->r * y[i__3].i + beta->i * y[i__3]
						.r;
					y[i__2].r = q__1.r, y[i__2].i = q__1.i;
					iy += *incy;
					/* L40: */
				}
			}
		}
	}
	if (alpha->r == 0.f && alpha->i == 0.f) {
		return 0;
	}
	if (lsame_(trans, N_buffer)) {
		/*        Form  y := alpha*A*x + y. */
		jx = kx;
		if (*incy == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = jx;
				if (x[i__2].r != 0.f || x[i__2].i != 0.f) {
					i__2 = jx;
					q__1.r = alpha->r * x[i__2].r - alpha->i * x[i__2].i,
						q__1.i = alpha->r * x[i__2].i + alpha->i * x[i__2]
						.r;
					temp.r = q__1.r, temp.i = q__1.i;
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__;
						i__4 = i__;
						i__5 = i__ + j * a_dim1;
						q__2.r = temp.r * a[i__5].r - temp.i * a[i__5].i,
							q__2.i = temp.r * a[i__5].i + temp.i * a[i__5]
							.r;
						q__1.r = y[i__4].r + q__2.r, q__1.i = y[i__4].i +
							q__2.i;
						y[i__3].r = q__1.r, y[i__3].i = q__1.i;
						/* L50: */
					}
				}
				jx += *incx;
				/* L60: */
			}
		}
		else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = jx;
				if (x[i__2].r != 0.f || x[i__2].i != 0.f) {
					i__2 = jx;
					q__1.r = alpha->r * x[i__2].r - alpha->i * x[i__2].i,
						q__1.i = alpha->r * x[i__2].i + alpha->i * x[i__2]
						.r;
					temp.r = q__1.r, temp.i = q__1.i;
					iy = ky;
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = iy;
						i__4 = iy;
						i__5 = i__ + j * a_dim1;
						q__2.r = temp.r * a[i__5].r - temp.i * a[i__5].i,
							q__2.i = temp.r * a[i__5].i + temp.i * a[i__5]
							.r;
						q__1.r = y[i__4].r + q__2.r, q__1.i = y[i__4].i +
							q__2.i;
						y[i__3].r = q__1.r, y[i__3].i = q__1.i;
						iy += *incy;
						/* L70: */
					}
				}
				jx += *incx;
				/* L80: */
			}
		}
	}
	else {
		/*        Form  y := alpha*A'*x + y  or  y := alpha*conjg( A' )*x + y. */
		jy = ky;
		if (*incx == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				temp.r = 0.f, temp.i = 0.f;
				if (noconj) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * a_dim1;
						i__4 = i__;
						q__2.r = a[i__3].r * x[i__4].r - a[i__3].i * x[i__4]
							.i, q__2.i = a[i__3].r * x[i__4].i + a[i__3]
							.i * x[i__4].r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						/* L90: */
					}
				}
				else {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						r_cnjg(&q__3, &a[i__ + j * a_dim1]);
						i__3 = i__;
						q__2.r = q__3.r * x[i__3].r - q__3.i * x[i__3].i,
							q__2.i = q__3.r * x[i__3].i + q__3.i * x[i__3]
							.r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						/* L100: */
					}
				}
				i__2 = jy;
				i__3 = jy;
				q__2.r = alpha->r * temp.r - alpha->i * temp.i, q__2.i =
					alpha->r * temp.i + alpha->i * temp.r;
				q__1.r = y[i__3].r + q__2.r, q__1.i = y[i__3].i + q__2.i;
				y[i__2].r = q__1.r, y[i__2].i = q__1.i;
				jy += *incy;
				/* L110: */
			}
		}
		else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				temp.r = 0.f, temp.i = 0.f;
				ix = kx;
				if (noconj) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * a_dim1;
						i__4 = ix;
						q__2.r = a[i__3].r * x[i__4].r - a[i__3].i * x[i__4]
							.i, q__2.i = a[i__3].r * x[i__4].i + a[i__3]
							.i * x[i__4].r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						ix += *incx;
						/* L120: */
					}
				}
				else {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						r_cnjg(&q__3, &a[i__ + j * a_dim1]);
						i__3 = ix;
						q__2.r = q__3.r * x[i__3].r - q__3.i * x[i__3].i,
							q__2.i = q__3.r * x[i__3].i + q__3.i * x[i__3]
							.r;
						q__1.r = temp.r + q__2.r, q__1.i = temp.i + q__2.i;
						temp.r = q__1.r, temp.i = q__1.i;
						ix += *incx;
						/* L130: */
					}
				}
				i__2 = jy;
				i__3 = jy;
				q__2.r = alpha->r * temp.r - alpha->i * temp.i, q__2.i =
					alpha->r * temp.i + alpha->i * temp.r;
				q__1.r = y[i__3].r + q__2.r, q__1.i = y[i__3].i + q__2.i;
				y[i__2].r = q__1.r, y[i__2].i = q__1.i;
				jy += *incy;
				/* L140: */
			}
		}
	}
	return 0;
	/*     End of CGEMV . */
} /* cgemv_ */

/* Subroutine */ int clacgv_(integer *n, complex *x, integer *incx)
{
	/*  -- LAPACK auxiliary routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	CLACGV conjugates a complex vector of length N.

	Arguments
	=========

	N       (input) INTEGER
	The length of the vector X.  N >= 0.

	X       (input/output) COMPLEX array, dimension
	(1+(N-1)*abs(INCX))
	On entry, the vector of length N to be conjugated.
	On exit, X is overwritten with conjg(X).

	INCX    (input) INTEGER
	The spacing between successive elements of X.

	=====================================================================


	Parameter adjustments */
	/* System generated locals */
	integer i__1, i__2;
	complex q__1;
	/* Builtin functions */
	/* Local variables */
	static integer i__, ioff;

	--x;

	/* Function Body */
	if (*incx == 1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			i__2 = i__;
			r_cnjg(&q__1, &x[i__]);
			x[i__2].r = q__1.r, x[i__2].i = q__1.i;
			/* L10: */
		}
	}
	else {
		ioff = 1;
		if (*incx < 0) {
			ioff = 1 - (*n - 1) * *incx;
		}
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			i__2 = ioff;
			r_cnjg(&q__1, &x[ioff]);
			x[i__2].r = q__1.r, x[i__2].i = q__1.i;
			ioff += *incx;
			/* L20: */
		}
	}
	return 0;

	/*     End of CLACGV */

} /* clacgv_ */



/* Subroutine */ int csscal_(integer *n, real *sa, complex *cx, integer *incx)
{
	/* System generated locals */
	integer i__1, i__2, i__3, i__4;
	real r__1, r__2;
	complex q__1;
	/* Builtin functions */
	double r_imag(complex *);
	/* Local variables */
	static integer i__, nincx;
	/*  Purpose
	=======
	scales a complex vector by a real constant.
	jack dongarra, linpack, 3/11/78.
	modified 3/93 to return if incx .le. 0.
	modified 12/3/93, array(1) declarations changed to array(*)
	Parameter adjustments */
	--cx;
	/* Function Body */
	if (*n <= 0 || *incx <= 0) {
		return 0;
	}
	if (*incx == 1) {
		goto L20;
	}
	/*        code for increment not equal to 1 */
	nincx = *n * *incx;
	i__1 = nincx;
	i__2 = *incx;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
		i__3 = i__;
		i__4 = i__;
		r__1 = *sa * cx[i__4].r;
		r__2 = *sa * r_imag(&cx[i__]);
		q__1.r = r__1, q__1.i = r__2;
		cx[i__3].r = q__1.r, cx[i__3].i = q__1.i;
		/* L10: */
	}
	return 0;
	/*        code for increment equal to 1 */
L20:
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
		i__1 = i__;
		i__3 = i__;
		r__1 = *sa * cx[i__3].r;
		r__2 = *sa * r_imag(&cx[i__]);
		q__1.r = r__1, q__1.i = r__2;
		cx[i__1].r = q__1.r, cx[i__1].i = q__1.i;
		/* L30: */
	}
	return 0;
} /* csscal_ */

/* Subroutine */ int clahef_(char *uplo, integer *n, integer *nb, integer *kb,
	complex *a, integer *lda, integer *ipiv, complex *w, integer *ldw,
	integer *info)
{
	/*  -- LAPACK routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	CLAHEF computes a partial factorization of a complex Hermitian
	matrix A using the Bunch-Kaufman diagonal pivoting method. The
	partial factorization has the form:

	A  =  ( I  U12 ) ( A11  0  ) (  I    0   )  if UPLO = 'U', or:
	( 0  U22 ) (  0   D  ) ( U12' U22' )

	A  =  ( L11  0 ) (  D   0  ) ( L11' L21' )  if UPLO = 'L'
	( L21  I ) (  0  A22 ) (  0    I   )

	where the order of D is at most NB. The actual order is returned in
	the argument KB, and is either NB or NB-1, or N if N <= NB.
	Note that U' denotes the conjugate transpose of U.

	CLAHEF is an auxiliary routine called by CHETRF. It uses blocked code
	(calling Level 3 BLAS) to update the submatrix A11 (if UPLO = 'U') or
	A22 (if UPLO = 'L').

	Arguments
	=========

	UPLO    (input) CHARACTER*1
	Specifies whether the upper or lower triangular part of the
	Hermitian matrix A is stored:
	= 'U':  Upper triangular
	= 'L':  Lower triangular

	N       (input) INTEGER
	The order of the matrix A.  N >= 0.

	NB      (input) INTEGER
	The maximum number of columns of the matrix A that should be
	factored.  NB should be at least 2 to allow for 2-by-2 pivot
	blocks.

	KB      (output) INTEGER
	The number of columns of A that were actually factored.
	KB is either NB-1 or NB, or N if N <= NB.

	A       (input/output) COMPLEX array, dimension (LDA,N)
	On entry, the Hermitian matrix A.  If UPLO = 'U', the leading
	n-by-n upper triangular part of A contains the upper
	triangular part of the matrix A, and the strictly lower
	triangular part of A is not referenced.  If UPLO = 'L', the
	leading n-by-n lower triangular part of A contains the lower
	triangular part of the matrix A, and the strictly upper
	triangular part of A is not referenced.
	On exit, A contains details of the partial factorization.

	LDA     (input) INTEGER
	The leading dimension of the array A.  LDA >= max(1,N).

	IPIV    (output) INTEGER array, dimension (N)
	Details of the interchanges and the block structure of D.
	If UPLO = 'U', only the last KB elements of IPIV are set;
	if UPLO = 'L', only the first KB elements are set.

	If IPIV(k) > 0, then rows and columns k and IPIV(k) were
	interchanged and D(k,k) is a 1-by-1 diagonal block.
	If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and
	columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k)
	is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) =
	IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were
	interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block.

	W       (workspace) COMPLEX array, dimension (LDW,NB)

	LDW     (input) INTEGER
	The leading dimension of the array W.  LDW >= max(1,N).

	INFO    (output) INTEGER
	= 0: successful exit
	> 0: if INFO = k, D(k,k) is exactly zero.  The factorization
	has been completed, but the block diagonal matrix D is
	exactly singular.

	=====================================================================


	Parameter adjustments */
	/* Table of constant values */
	static complex c_b1 = { 1.f,0.f };
	static integer c__1 = 1;

	/* System generated locals */
	integer a_dim1, a_offset, w_dim1, w_offset, i__1, i__2, i__3, i__4, i__5;
	real r__1, r__2, r__3, r__4;
	complex q__1, q__2, q__3, q__4;
	/* Builtin functions */
	double sqrt(doublereal);
	/* Local variables */
	static integer j, k;
	static real t, r1;
	static complex d11, d21, d22;
	static integer jb, jj, kk, jp, kp, kw, kkw, imax, jmax;
	static real alpha;
	static integer kstep;
	static real absakk;
	static real colmax, rowmax;


	a_dim1 = *lda;
	a_offset = 1 + a_dim1;
	a -= a_offset;
	--ipiv;
	w_dim1 = *ldw;
	w_offset = 1 + w_dim1;
	w -= w_offset;

	/* Function Body */
	*info = 0;

	/*     Initialize ALPHA for use in choosing pivot block size. */

	alpha = (sqrt(17.f) + 1.f) / 8.f;

	if (lsame_(uplo, U_buffer)) {

		/*        Factorize the trailing columns of A using the upper triangle
		of A and working backwards, and compute the matrix W = U12*D
		for use in updating A11 (note that conjg(W) is actually stored)

		K is the main loop index, decreasing from N in steps of 1 or 2

		KW is the column of W which corresponds to column K of A */

		k = *n;
	L10:
		kw = *nb + k - *n;

		/*        Exit from loop */

		if (k <= *n - *nb + 1 && *nb < *n || k < 1) {
			goto L30;
		}

		/*        Copy column K of A to column KW of W and update it */

		i__1 = k - 1;
		ccopy_(&i__1, &a[k * a_dim1 + 1], &c__1, &w[kw * w_dim1 + 1], &c__1);
		i__1 = k + kw * w_dim1;
		i__2 = k + k * a_dim1;
		r__1 = a[i__2].r;
		w[i__1].r = r__1, w[i__1].i = 0.f;
		if (k < *n) {
			i__1 = *n - k;
			q__1.r = -1.f, q__1.i = -0.f;
			cgemv_(Notranspose_buffer, &k, &i__1, &q__1, &a[(k + 1) * a_dim1 + 1],
				lda, &w[k + (kw + 1) * w_dim1], ldw, &c_b1, &w[kw *
				w_dim1 + 1], &c__1);
			i__1 = k + kw * w_dim1;
			i__2 = k + kw * w_dim1;
			r__1 = w[i__2].r;
			w[i__1].r = r__1, w[i__1].i = 0.f;
		}

		kstep = 1;

		/*        Determine rows and columns to be interchanged and whether
		a 1-by-1 or 2-by-2 pivot block will be used */

		i__1 = k + kw * w_dim1;
		absakk = (r__1 = w[i__1].r, dabs(r__1));

		/*        IMAX is the row-index of the largest off-diagonal element in
		column K, and COLMAX is its absolute value */

		if (k > 1) {
			i__1 = k - 1;
			imax = icamax_(&i__1, &w[kw * w_dim1 + 1], &c__1);
			i__1 = imax + kw * w_dim1;
			colmax = (r__1 = w[i__1].r, dabs(r__1)) + (r__2 = r_imag(&w[imax
				+ kw * w_dim1]), dabs(r__2));
		}
		else {
			colmax = 0.f;
		}

		if (dmax(absakk, colmax) == 0.f) {

			/*           Column K is zero: set INFO and continue */

			if (*info == 0) {
				*info = k;
			}
			kp = k;
			i__1 = k + k * a_dim1;
			i__2 = k + k * a_dim1;
			r__1 = a[i__2].r;
			a[i__1].r = r__1, a[i__1].i = 0.f;
		}
		else {
			if (absakk >= alpha * colmax) {

				/*              no interchange, use 1-by-1 pivot block */

				kp = k;
			}
			else {

				/*              Copy column IMAX to column KW-1 of W and update it */

				i__1 = imax - 1;
				ccopy_(&i__1, &a[imax * a_dim1 + 1], &c__1, &w[(kw - 1) *
					w_dim1 + 1], &c__1);
				i__1 = imax + (kw - 1) * w_dim1;
				i__2 = imax + imax * a_dim1;
				r__1 = a[i__2].r;
				w[i__1].r = r__1, w[i__1].i = 0.f;
				i__1 = k - imax;
				ccopy_(&i__1, &a[imax + (imax + 1) * a_dim1], lda, &w[imax +
					1 + (kw - 1) * w_dim1], &c__1);
				i__1 = k - imax;
				clacgv_(&i__1, &w[imax + 1 + (kw - 1) * w_dim1], &c__1);
				if (k < *n) {
					i__1 = *n - k;
					q__1.r = -1.f, q__1.i = -0.f;
					cgemv_(Notranspose_buffer, &k, &i__1, &q__1, &a[(k + 1) *
						a_dim1 + 1], lda, &w[imax + (kw + 1) * w_dim1],
						ldw, &c_b1, &w[(kw - 1) * w_dim1 + 1], &c__1);
					i__1 = imax + (kw - 1) * w_dim1;
					i__2 = imax + (kw - 1) * w_dim1;
					r__1 = w[i__2].r;
					w[i__1].r = r__1, w[i__1].i = 0.f;
				}

				/*              JMAX is the column-index of the largest off-diagonal
				element in row IMAX, and ROWMAX is its absolute value */

				i__1 = k - imax;
				jmax = imax + icamax_(&i__1, &w[imax + 1 + (kw - 1) * w_dim1],
					&c__1);
				i__1 = jmax + (kw - 1) * w_dim1;
				rowmax = (r__1 = w[i__1].r, dabs(r__1)) + (r__2 = r_imag(&w[
					jmax + (kw - 1) * w_dim1]), dabs(r__2));
				if (imax > 1) {
					i__1 = imax - 1;
					jmax = icamax_(&i__1, &w[(kw - 1) * w_dim1 + 1], &c__1);
					/* Computing MAX */
					i__1 = jmax + (kw - 1) * w_dim1;
					r__3 = rowmax, r__4 = (r__1 = w[i__1].r, dabs(r__1)) + (
						r__2 = r_imag(&w[jmax + (kw - 1) * w_dim1]), dabs(
							r__2));
					rowmax = dmax(r__3, r__4);
				}

				if (absakk >= alpha * colmax * (colmax / rowmax)) {

					/*                 no interchange, use 1-by-1 pivot block */

					kp = k;
				}
				else /* if(complicated condition) */ {
					i__1 = imax + (kw - 1) * w_dim1;
					if ((r__1 = w[i__1].r, dabs(r__1)) >= alpha * rowmax) {

						/*                 interchange rows and columns K and IMAX, use 1-by-1
						pivot block */

						kp = imax;

						/*                 copy column KW-1 of W to column KW */

						ccopy_(&k, &w[(kw - 1) * w_dim1 + 1], &c__1, &w[kw *
							w_dim1 + 1], &c__1);
					}
					else {

						/*                 interchange rows and columns K-1 and IMAX, use 2-by-2
						pivot block */

						kp = imax;
						kstep = 2;
					}
				}
			}

			kk = k - kstep + 1;
			kkw = *nb + kk - *n;

			/*           Updated column KP is already stored in column KKW of W */

			if (kp != kk) {

				/*              Copy non-updated column KK to column KP */

				i__1 = kp + kp * a_dim1;
				i__2 = kk + kk * a_dim1;
				r__1 = a[i__2].r;
				a[i__1].r = r__1, a[i__1].i = 0.f;
				i__1 = kk - 1 - kp;
				ccopy_(&i__1, &a[kp + 1 + kk * a_dim1], &c__1, &a[kp + (kp +
					1) * a_dim1], lda);
				i__1 = kk - 1 - kp;
				clacgv_(&i__1, &a[kp + (kp + 1) * a_dim1], lda);
				i__1 = kp - 1;
				ccopy_(&i__1, &a[kk * a_dim1 + 1], &c__1, &a[kp * a_dim1 + 1],
					&c__1);

				/*              Interchange rows KK and KP in last KK columns of A and W */

				if (kk < *n) {
					i__1 = *n - kk;
					cswap_(&i__1, &a[kk + (kk + 1) * a_dim1], lda, &a[kp + (
						kk + 1) * a_dim1], lda);
				}
				i__1 = *n - kk + 1;
				cswap_(&i__1, &w[kk + kkw * w_dim1], ldw, &w[kp + kkw *
					w_dim1], ldw);
			}

			if (kstep == 1) {

				/*              1-by-1 pivot block D(k): column KW of W now holds

				W(k) = U(k)*D(k)

				where U(k) is the k-th column of U

				Store U(k) in column k of A */

				ccopy_(&k, &w[kw * w_dim1 + 1], &c__1, &a[k * a_dim1 + 1], &
					c__1);
				i__1 = k + k * a_dim1;
				r1 = 1.f / a[i__1].r;
				i__1 = k - 1;
				csscal_(&i__1, &r1, &a[k * a_dim1 + 1], &c__1);

				/*              Conjugate W(k) */

				i__1 = k - 1;
				clacgv_(&i__1, &w[kw * w_dim1 + 1], &c__1);
			}
			else {

				/*              2-by-2 pivot block D(k): columns KW and KW-1 of W now
				hold

				( W(k-1) W(k) ) = ( U(k-1) U(k) )*D(k)

				where U(k) and U(k-1) are the k-th and (k-1)-th columns
				of U */

				if (k > 2) {

					/*                 Store U(k) and U(k-1) in columns k and k-1 of A */

					i__1 = k - 1 + kw * w_dim1;
					d21.r = w[i__1].r, d21.i = w[i__1].i;
					r_cnjg(&q__2, &d21);
					c_div(&q__1, &w[k + kw * w_dim1], &q__2);
					d11.r = q__1.r, d11.i = q__1.i;
					c_div(&q__1, &w[k - 1 + (kw - 1) * w_dim1], &d21);
					d22.r = q__1.r, d22.i = q__1.i;
					q__1.r = d11.r * d22.r - d11.i * d22.i, q__1.i = d11.r *
						d22.i + d11.i * d22.r;
					t = 1.f / (q__1.r - 1.f);
					q__2.r = t, q__2.i = 0.f;
					c_div(&q__1, &q__2, &d21);
					d21.r = q__1.r, d21.i = q__1.i;
					i__1 = k - 2;
					for (j = 1; j <= i__1; ++j) {
						i__2 = j + (k - 1) * a_dim1;
						i__3 = j + (kw - 1) * w_dim1;
						q__3.r = d11.r * w[i__3].r - d11.i * w[i__3].i,
							q__3.i = d11.r * w[i__3].i + d11.i * w[i__3]
							.r;
						i__4 = j + kw * w_dim1;
						q__2.r = q__3.r - w[i__4].r, q__2.i = q__3.i - w[i__4]
							.i;
						q__1.r = d21.r * q__2.r - d21.i * q__2.i, q__1.i =
							d21.r * q__2.i + d21.i * q__2.r;
						a[i__2].r = q__1.r, a[i__2].i = q__1.i;
						i__2 = j + k * a_dim1;
						r_cnjg(&q__2, &d21);
						i__3 = j + kw * w_dim1;
						q__4.r = d22.r * w[i__3].r - d22.i * w[i__3].i,
							q__4.i = d22.r * w[i__3].i + d22.i * w[i__3]
							.r;
						i__4 = j + (kw - 1) * w_dim1;
						q__3.r = q__4.r - w[i__4].r, q__3.i = q__4.i - w[i__4]
							.i;
						q__1.r = q__2.r * q__3.r - q__2.i * q__3.i, q__1.i =
							q__2.r * q__3.i + q__2.i * q__3.r;
						a[i__2].r = q__1.r, a[i__2].i = q__1.i;
						/* L20: */
					}
				}

				/*              Copy D(k) to A */

				i__1 = k - 1 + (k - 1) * a_dim1;
				i__2 = k - 1 + (kw - 1) * w_dim1;
				a[i__1].r = w[i__2].r, a[i__1].i = w[i__2].i;
				i__1 = k - 1 + k * a_dim1;
				i__2 = k - 1 + kw * w_dim1;
				a[i__1].r = w[i__2].r, a[i__1].i = w[i__2].i;
				i__1 = k + k * a_dim1;
				i__2 = k + kw * w_dim1;
				a[i__1].r = w[i__2].r, a[i__1].i = w[i__2].i;

				/*              Conjugate W(k) and W(k-1) */

				i__1 = k - 1;
				clacgv_(&i__1, &w[kw * w_dim1 + 1], &c__1);
				i__1 = k - 2;
				clacgv_(&i__1, &w[(kw - 1) * w_dim1 + 1], &c__1);
			}
		}

		/*        Store details of the interchanges in IPIV */

		if (kstep == 1) {
			ipiv[k] = kp;
		}
		else {
			ipiv[k] = -kp;
			ipiv[k - 1] = -kp;
		}

		/*        Decrease K and return to the start of the main loop */

		k -= kstep;
		goto L10;

	L30:

		/*        Update the upper triangle of A11 (= A(1:k,1:k)) as

		A11 := A11 - U12*D*U12' = A11 - U12*W'

		computing blocks of NB columns at a time (note that conjg(W) is
		actually stored) */

		i__1 = -(*nb);
		for (j = (k - 1) / *nb * *nb + 1; i__1 < 0 ? j >= 1 : j <= 1; j +=
			i__1) {
			/* Computing MIN */
			i__2 = *nb, i__3 = k - j + 1;
			jb = min(i__2, i__3);

			/*           Update the upper triangle of the diagonal block */

			i__2 = j + jb - 1;
			for (jj = j; jj <= i__2; ++jj) {
				i__3 = jj + jj * a_dim1;
				i__4 = jj + jj * a_dim1;
				r__1 = a[i__4].r;
				a[i__3].r = r__1, a[i__3].i = 0.f;
				i__3 = jj - j + 1;
				i__4 = *n - k;
				q__1.r = -1.f, q__1.i = -0.f;
				cgemv_(Notranspose_buffer, &i__3, &i__4, &q__1, &a[j + (k + 1) *
					a_dim1], lda, &w[jj + (kw + 1) * w_dim1], ldw, &c_b1,
					&a[j + jj * a_dim1], &c__1);
				i__3 = jj + jj * a_dim1;
				i__4 = jj + jj * a_dim1;
				r__1 = a[i__4].r;
				a[i__3].r = r__1, a[i__3].i = 0.f;
				/* L40: */
			}

			/*           Update the rectangular superdiagonal block */

			i__2 = j - 1;
			i__3 = *n - k;
			q__1.r = -1.f, q__1.i = -0.f;
			cgemm_(Notranspose_buffer, Transpose_buffer, &i__2, &jb, &i__3, &q__1, &a[(
				k + 1) * a_dim1 + 1], lda, &w[j + (kw + 1) * w_dim1], ldw,
				&c_b1, &a[j * a_dim1 + 1], lda);
			/* L50: */
		}

		/*        Put U12 in standard form by partially undoing the interchanges
		in columns k+1:n */

		j = k + 1;
	L60:
		jj = j;
		jp = ipiv[j];
		if (jp < 0) {
			jp = -jp;
			++j;
		}
		++j;
		if (jp != jj && j <= *n) {
			i__1 = *n - j + 1;
			cswap_(&i__1, &a[jp + j * a_dim1], lda, &a[jj + j * a_dim1], lda);
		}
		if (j <= *n) {
			goto L60;
		}

		/*        Set KB to the number of columns factorized */

		*kb = *n - k;

	}
	else {

		/*        Factorize the leading columns of A using the lower triangle
		of A and working forwards, and compute the matrix W = L21*D
		for use in updating A22 (note that conjg(W) is actually stored)

		K is the main loop index, increasing from 1 in steps of 1 or 2 */

		k = 1;
	L70:

		/*        Exit from loop */

		if (k >= *nb && *nb < *n || k > *n) {
			goto L90;
		}

		/*        Copy column K of A to column K of W and update it */

		i__1 = k + k * w_dim1;
		i__2 = k + k * a_dim1;
		r__1 = a[i__2].r;
		w[i__1].r = r__1, w[i__1].i = 0.f;
		if (k < *n) {
			i__1 = *n - k;
			ccopy_(&i__1, &a[k + 1 + k * a_dim1], &c__1, &w[k + 1 + k *
				w_dim1], &c__1);
		}
		i__1 = *n - k + 1;
		i__2 = k - 1;
		q__1.r = -1.f, q__1.i = -0.f;
		cgemv_(Notranspose_buffer, &i__1, &i__2, &q__1, &a[k + a_dim1], lda, &w[k
			+ w_dim1], ldw, &c_b1, &w[k + k * w_dim1], &c__1);
		i__1 = k + k * w_dim1;
		i__2 = k + k * w_dim1;
		r__1 = w[i__2].r;
		w[i__1].r = r__1, w[i__1].i = 0.f;

		kstep = 1;

		/*        Determine rows and columns to be interchanged and whether
		a 1-by-1 or 2-by-2 pivot block will be used */

		i__1 = k + k * w_dim1;
		absakk = (r__1 = w[i__1].r, dabs(r__1));

		/*        IMAX is the row-index of the largest off-diagonal element in
		column K, and COLMAX is its absolute value */

		if (k < *n) {
			i__1 = *n - k;
			imax = k + icamax_(&i__1, &w[k + 1 + k * w_dim1], &c__1);
			i__1 = imax + k * w_dim1;
			colmax = (r__1 = w[i__1].r, dabs(r__1)) + (r__2 = r_imag(&w[imax
				+ k * w_dim1]), dabs(r__2));
		}
		else {
			colmax = 0.f;
		}

		if (dmax(absakk, colmax) == 0.f) {

			/*           Column K is zero: set INFO and continue */

			if (*info == 0) {
				*info = k;
			}
			kp = k;
			i__1 = k + k * a_dim1;
			i__2 = k + k * a_dim1;
			r__1 = a[i__2].r;
			a[i__1].r = r__1, a[i__1].i = 0.f;
		}
		else {
			if (absakk >= alpha * colmax) {

				/*              no interchange, use 1-by-1 pivot block */

				kp = k;
			}
			else {

				/*              Copy column IMAX to column K+1 of W and update it */

				i__1 = imax - k;
				ccopy_(&i__1, &a[imax + k * a_dim1], lda, &w[k + (k + 1) *
					w_dim1], &c__1);
				i__1 = imax - k;
				clacgv_(&i__1, &w[k + (k + 1) * w_dim1], &c__1);
				i__1 = imax + (k + 1) * w_dim1;
				i__2 = imax + imax * a_dim1;
				r__1 = a[i__2].r;
				w[i__1].r = r__1, w[i__1].i = 0.f;
				if (imax < *n) {
					i__1 = *n - imax;
					ccopy_(&i__1, &a[imax + 1 + imax * a_dim1], &c__1, &w[
						imax + 1 + (k + 1) * w_dim1], &c__1);
				}
				i__1 = *n - k + 1;
				i__2 = k - 1;
				q__1.r = -1.f, q__1.i = -0.f;
				cgemv_(Notranspose_buffer, &i__1, &i__2, &q__1, &a[k + a_dim1],
					lda, &w[imax + w_dim1], ldw, &c_b1, &w[k + (k + 1) *
					w_dim1], &c__1);
				i__1 = imax + (k + 1) * w_dim1;
				i__2 = imax + (k + 1) * w_dim1;
				r__1 = w[i__2].r;
				w[i__1].r = r__1, w[i__1].i = 0.f;

				/*              JMAX is the column-index of the largest off-diagonal
				element in row IMAX, and ROWMAX is its absolute value */

				i__1 = imax - k;
				jmax = k - 1 + icamax_(&i__1, &w[k + (k + 1) * w_dim1], &c__1)
					;
				i__1 = jmax + (k + 1) * w_dim1;
				rowmax = (r__1 = w[i__1].r, dabs(r__1)) + (r__2 = r_imag(&w[
					jmax + (k + 1) * w_dim1]), dabs(r__2));
				if (imax < *n) {
					i__1 = *n - imax;
					jmax = imax + icamax_(&i__1, &w[imax + 1 + (k + 1) *
						w_dim1], &c__1);
					/* Computing MAX */
					i__1 = jmax + (k + 1) * w_dim1;
					r__3 = rowmax, r__4 = (r__1 = w[i__1].r, dabs(r__1)) + (
						r__2 = r_imag(&w[jmax + (k + 1) * w_dim1]), dabs(
							r__2));
					rowmax = dmax(r__3, r__4);
				}

				if (absakk >= alpha * colmax * (colmax / rowmax)) {

					/*                 no interchange, use 1-by-1 pivot block */

					kp = k;
				}
				else /* if(complicated condition) */ {
					i__1 = imax + (k + 1) * w_dim1;
					if ((r__1 = w[i__1].r, dabs(r__1)) >= alpha * rowmax) {

						/*                 interchange rows and columns K and IMAX, use 1-by-1
						pivot block */

						kp = imax;

						/*                 copy column K+1 of W to column K */

						i__1 = *n - k + 1;
						ccopy_(&i__1, &w[k + (k + 1) * w_dim1], &c__1, &w[k +
							k * w_dim1], &c__1);
					}
					else {

						/*                 interchange rows and columns K+1 and IMAX, use 2-by-2
						pivot block */

						kp = imax;
						kstep = 2;
					}
				}
			}

			kk = k + kstep - 1;

			/*           Updated column KP is already stored in column KK of W */

			if (kp != kk) {

				/*              Copy non-updated column KK to column KP */

				i__1 = kp + kp * a_dim1;
				i__2 = kk + kk * a_dim1;
				r__1 = a[i__2].r;
				a[i__1].r = r__1, a[i__1].i = 0.f;
				i__1 = kp - kk - 1;
				ccopy_(&i__1, &a[kk + 1 + kk * a_dim1], &c__1, &a[kp + (kk +
					1) * a_dim1], lda);
				i__1 = kp - kk - 1;
				clacgv_(&i__1, &a[kp + (kk + 1) * a_dim1], lda);
				if (kp < *n) {
					i__1 = *n - kp;
					ccopy_(&i__1, &a[kp + 1 + kk * a_dim1], &c__1, &a[kp + 1
						+ kp * a_dim1], &c__1);
				}

				/*              Interchange rows KK and KP in first KK columns of A and W */

				i__1 = kk - 1;
				cswap_(&i__1, &a[kk + a_dim1], lda, &a[kp + a_dim1], lda);
				cswap_(&kk, &w[kk + w_dim1], ldw, &w[kp + w_dim1], ldw);
			}

			if (kstep == 1) {

				/*              1-by-1 pivot block D(k): column k of W now holds

				W(k) = L(k)*D(k)

				where L(k) is the k-th column of L

				Store L(k) in column k of A */

				i__1 = *n - k + 1;
				ccopy_(&i__1, &w[k + k * w_dim1], &c__1, &a[k + k * a_dim1], &
					c__1);
				if (k < *n) {
					i__1 = k + k * a_dim1;
					r1 = 1.f / a[i__1].r;
					i__1 = *n - k;
					csscal_(&i__1, &r1, &a[k + 1 + k * a_dim1], &c__1);

					/*                 Conjugate W(k) */

					i__1 = *n - k;
					clacgv_(&i__1, &w[k + 1 + k * w_dim1], &c__1);
				}
			}
			else {

				/*              2-by-2 pivot block D(k): columns k and k+1 of W now hold

				( W(k) W(k+1) ) = ( L(k) L(k+1) )*D(k)

				where L(k) and L(k+1) are the k-th and (k+1)-th columns
				of L */

				if (k < *n - 1) {

					/*                 Store L(k) and L(k+1) in columns k and k+1 of A */

					i__1 = k + 1 + k * w_dim1;
					d21.r = w[i__1].r, d21.i = w[i__1].i;
					c_div(&q__1, &w[k + 1 + (k + 1) * w_dim1], &d21);
					d11.r = q__1.r, d11.i = q__1.i;
					r_cnjg(&q__2, &d21);
					c_div(&q__1, &w[k + k * w_dim1], &q__2);
					d22.r = q__1.r, d22.i = q__1.i;
					q__1.r = d11.r * d22.r - d11.i * d22.i, q__1.i = d11.r *
						d22.i + d11.i * d22.r;
					t = 1.f / (q__1.r - 1.f);
					q__2.r = t, q__2.i = 0.f;
					c_div(&q__1, &q__2, &d21);
					d21.r = q__1.r, d21.i = q__1.i;
					i__1 = *n;
					for (j = k + 2; j <= i__1; ++j) {
						i__2 = j + k * a_dim1;
						r_cnjg(&q__2, &d21);
						i__3 = j + k * w_dim1;
						q__4.r = d11.r * w[i__3].r - d11.i * w[i__3].i,
							q__4.i = d11.r * w[i__3].i + d11.i * w[i__3]
							.r;
						i__4 = j + (k + 1) * w_dim1;
						q__3.r = q__4.r - w[i__4].r, q__3.i = q__4.i - w[i__4]
							.i;
						q__1.r = q__2.r * q__3.r - q__2.i * q__3.i, q__1.i =
							q__2.r * q__3.i + q__2.i * q__3.r;
						a[i__2].r = q__1.r, a[i__2].i = q__1.i;
						i__2 = j + (k + 1) * a_dim1;
						i__3 = j + (k + 1) * w_dim1;
						q__3.r = d22.r * w[i__3].r - d22.i * w[i__3].i,
							q__3.i = d22.r * w[i__3].i + d22.i * w[i__3]
							.r;
						i__4 = j + k * w_dim1;
						q__2.r = q__3.r - w[i__4].r, q__2.i = q__3.i - w[i__4]
							.i;
						q__1.r = d21.r * q__2.r - d21.i * q__2.i, q__1.i =
							d21.r * q__2.i + d21.i * q__2.r;
						a[i__2].r = q__1.r, a[i__2].i = q__1.i;
						/* L80: */
					}
				}

				/*              Copy D(k) to A */

				i__1 = k + k * a_dim1;
				i__2 = k + k * w_dim1;
				a[i__1].r = w[i__2].r, a[i__1].i = w[i__2].i;
				i__1 = k + 1 + k * a_dim1;
				i__2 = k + 1 + k * w_dim1;
				a[i__1].r = w[i__2].r, a[i__1].i = w[i__2].i;
				i__1 = k + 1 + (k + 1) * a_dim1;
				i__2 = k + 1 + (k + 1) * w_dim1;
				a[i__1].r = w[i__2].r, a[i__1].i = w[i__2].i;

				/*              Conjugate W(k) and W(k+1) */

				i__1 = *n - k;
				clacgv_(&i__1, &w[k + 1 + k * w_dim1], &c__1);
				i__1 = *n - k - 1;
				clacgv_(&i__1, &w[k + 2 + (k + 1) * w_dim1], &c__1);
			}
		}

		/*        Store details of the interchanges in IPIV */

		if (kstep == 1) {
			ipiv[k] = kp;
		}
		else {
			ipiv[k] = -kp;
			ipiv[k + 1] = -kp;
		}

		/*        Increase K and return to the start of the main loop */

		k += kstep;
		goto L70;

	L90:

		/*        Update the lower triangle of A22 (= A(k:n,k:n)) as

		A22 := A22 - L21*D*L21' = A22 - L21*W'

		computing blocks of NB columns at a time (note that conjg(W) is
		actually stored) */

		i__1 = *n;
		i__2 = *nb;
		for (j = k; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {
			/* Computing MIN */
			i__3 = *nb, i__4 = *n - j + 1;
			jb = min(i__3, i__4);

			/*           Update the lower triangle of the diagonal block */

			i__3 = j + jb - 1;
			for (jj = j; jj <= i__3; ++jj) {
				i__4 = jj + jj * a_dim1;
				i__5 = jj + jj * a_dim1;
				r__1 = a[i__5].r;
				a[i__4].r = r__1, a[i__4].i = 0.f;
				i__4 = j + jb - jj;
				i__5 = k - 1;
				q__1.r = -1.f, q__1.i = -0.f;
				cgemv_(Notranspose_buffer, &i__4, &i__5, &q__1, &a[jj + a_dim1],
					lda, &w[jj + w_dim1], ldw, &c_b1, &a[jj + jj * a_dim1]
					, &c__1);
				i__4 = jj + jj * a_dim1;
				i__5 = jj + jj * a_dim1;
				r__1 = a[i__5].r;
				a[i__4].r = r__1, a[i__4].i = 0.f;
				/* L100: */
			}

			/*           Update the rectangular subdiagonal block */

			if (j + jb <= *n) {
				i__3 = *n - j - jb + 1;
				i__4 = k - 1;
				q__1.r = -1.f, q__1.i = -0.f;
				cgemm_(Notranspose_buffer, Transpose_buffer, &i__3, &jb, &i__4, &q__1,
					&a[j + jb + a_dim1], lda, &w[j + w_dim1], ldw, &c_b1,
					&a[j + jb + j * a_dim1], lda);
			}
			/* L110: */
		}

		/*        Put L21 in standard form by partially undoing the interchanges
		in columns 1:k-1 */

		j = k - 1;
	L120:
		jj = j;
		jp = ipiv[j];
		if (jp < 0) {
			jp = -jp;
			--j;
		}
		--j;
		if (jp != jj && j >= 1) {
			cswap_(&j, &a[jp + a_dim1], lda, &a[jj + a_dim1], lda);
		}
		if (j >= 1) {
			goto L120;
		}

		/*        Set KB to the number of columns factorized */

		*kb = k - 1;

	}
	return 0;

	/*     End of CLAHEF */

} /* clahef_ */

/* Subroutine */ int xerbla_(char *srname, integer *info)
{
	/*  -- LAPACK auxiliary routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
	Courant Institute, Argonne National Lab, and Rice University
	November 2006


	Purpose
	=======

	XERBLA  is an error handler for the LAPACK routines.
	It is called by an LAPACK routine if an input parameter has an
	invalid value.  A message is printed and execution stops.

	Installers may consider modifying the STOP statement in order to
	call system-specific exception-handling facilities.

	Arguments
	=========

	SRNAME  (input) CHARACTER*6
	The name of the routine which called XERBLA.

	INFO    (input) INTEGER
	The position of the invalid parameter in the parameter list
	of the calling routine.

	=====================================================================
	*/

	printf("** On entry to %6s, parameter number %ld had an illegal value\n", srname, *info);

	/*     End of XERBLA */

	return 0;
} /* xerbla_ */

integer ieeeck_(integer *ispec, real *zero, real *one)
{
	/*  -- LAPACK auxiliary routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	IEEECK is called from the ILAENV to verify that Infinity and
	possibly NaN arithmetic is safe (i.e. will not trap).

	Arguments
	=========

	ISPEC   (input) INTEGER
	Specifies whether to test just for inifinity arithmetic
	or whether to test for infinity and NaN arithmetic.
	= 0: Verify infinity arithmetic only.
	= 1: Verify infinity and NaN arithmetic.

	ZERO    (input) REAL
	Must contain the value 0.0
	This is passed to prevent the compiler from optimizing
	away this code.

	ONE     (input) REAL
	Must contain the value 1.0
	This is passed to prevent the compiler from optimizing
	away this code.

	RETURN VALUE:  INTEGER
	= 0:  Arithmetic failed to produce the correct answers
	= 1:  Arithmetic produced the correct answers */
	/* System generated locals */
	integer ret_val;
	/* Local variables */
	static real nan1, nan2, nan3, nan4, nan5, nan6, neginf, posinf, negzro,
		newzro;


	ret_val = 1;

	posinf = *one / *zero;
	if (posinf <= *one) {
		ret_val = 0;
		return ret_val;
	}

	neginf = -(*one) / *zero;
	if (neginf >= *zero) {
		ret_val = 0;
		return ret_val;
	}

	negzro = *one / (neginf + *one);
	if (negzro != *zero) {
		ret_val = 0;
		return ret_val;
	}

	neginf = *one / negzro;
	if (neginf >= *zero) {
		ret_val = 0;
		return ret_val;
	}

	newzro = negzro + *zero;
	if (newzro != *zero) {
		ret_val = 0;
		return ret_val;
	}

	posinf = *one / newzro;
	if (posinf <= *one) {
		ret_val = 0;
		return ret_val;
	}

	neginf *= posinf;
	if (neginf >= *zero) {
		ret_val = 0;
		return ret_val;
	}

	posinf *= posinf;
	if (posinf <= *one) {
		ret_val = 0;
		return ret_val;
	}




	/*     Return if we were only asked to check infinity arithmetic */

	if (*ispec == 0) {
		return ret_val;
	}

	nan1 = posinf + neginf;

	nan2 = posinf / neginf;

	nan3 = posinf / posinf;

	nan4 = posinf * *zero;

	nan5 = neginf * negzro;

	nan6 = nan5 * 0.f;

	if (nan1 == nan1) {
		ret_val = 0;
		return ret_val;
	}

	if (nan2 == nan2) {
		ret_val = 0;
		return ret_val;
	}

	if (nan3 == nan3) {
		ret_val = 0;
		return ret_val;
	}

	if (nan4 == nan4) {
		ret_val = 0;
		return ret_val;
	}

	if (nan5 == nan5) {
		ret_val = 0;
		return ret_val;
	}

	if (nan6 == nan6) {
		ret_val = 0;
		return ret_val;
	}

	return ret_val;
} /* ieeeck_ */


integer i_nint(real *x)
{
	return (integer)(*x >= 0 ? floor(*x + .5) : -floor(.5 - *x));
} 
integer iparmq_(integer *ispec, char *name__, char *opts, integer *n, integer*ilo, integer *ihi, integer *lwork)
{
	/*  -- LAPACK auxiliary routine (version 3.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	November 2006


	Purpose
	=======

	This program sets problem and machine dependent parameters
	useful for xHSEQR and its subroutines. It is called whenever
	ILAENV is called with 12 <= ISPEC <= 16

	Arguments
	=========

	ISPEC  (input) integer scalar
	ISPEC specifies which tunable parameter IPARMQ should
	return.

	ISPEC=12: (INMIN)  Matrices of order nmin or less
	are sent directly to xLAHQR, the implicit
	double shift QR algorithm.  NMIN must be
	at least 11.

	ISPEC=13: (INWIN)  Size of the deflation window.
	This is best set greater than or equal to
	the number of simultaneous shifts NS.
	Larger matrices benefit from larger deflation
	windows.

	ISPEC=14: (INIBL) Determines when to stop nibbling and
	invest in an (expensive) multi-shift QR sweep.
	If the aggressive early deflation subroutine
	finds LD converged eigenvalues from an order
	NW deflation window and LD.GT.(NW*NIBBLE)/100,
	then the next QR sweep is skipped and early
	deflation is applied immediately to the
	remaining active diagonal block.  Setting
	IPARMQ(ISPEC=14) = 0 causes TTQRE to skip a
	multi-shift QR sweep whenever early deflation
	finds a converged eigenvalue.  Setting
	IPARMQ(ISPEC=14) greater than or equal to 100
	prevents TTQRE from skipping a multi-shift
	QR sweep.

	ISPEC=15: (NSHFTS) The number of simultaneous shifts in
	a multi-shift QR iteration.

	ISPEC=16: (IACC22) IPARMQ is set to 0, 1 or 2 with the
	following meanings.
	0:  During the multi-shift QR sweep,
	xLAQR5 does not accumulate reflections and
	does not use matrix-matrix multiply to
	update the far-from-diagonal matrix
	entries.
	1:  During the multi-shift QR sweep,
	xLAQR5 and/or xLAQRaccumulates reflections and uses
	matrix-matrix multiply to update the
	far-from-diagonal matrix entries.
	2:  During the multi-shift QR sweep.
	xLAQR5 accumulates reflections and takes
	advantage of 2-by-2 block structure during
	matrix-matrix multiplies.
	(If xTRMM is slower than xGEMM, then
	IPARMQ(ISPEC=16)=1 may be more efficient than
	IPARMQ(ISPEC=16)=2 despite the greater level of
	arithmetic work implied by the latter choice.)

	NAME    (input) character string
	Name of the calling subroutine

	OPTS    (input) character string
	This is a concatenation of the string arguments to
	TTQRE.

	N       (input) integer scalar
	N is the order of the Hessenberg matrix H.

	ILO     (input) INTEGER
	IHI     (input) INTEGER
	It is assumed that H is already upper triangular
	in rows and columns 1:ILO-1 and IHI+1:N.

	LWORK   (input) integer scalar
	The amount of workspace available.

	Further Details
	===============

	Little is known about how best to choose these parameters.
	It is possible to use different values of the parameters
	for each of CHSEQR, DHSEQR, SHSEQR and ZHSEQR.

	It is probably best to choose different parameters for
	different matrices and different parameters at different
	times during the iteration, but this has not been
	implemented --- yet.


	The best choices of most of the parameters depend
	in an ill-understood way on the relative execution
	rate of xLAQR3 and xLAQR5 and on the nature of each
	particular eigenvalue problem.  Experiment may be the
	only practical way to determine which choices are most
	effective.

	Following is a list of default values supplied by IPARMQ.
	These defaults may be adjusted in order to attain better
	performance in any particular computational environment.

	IPARMQ(ISPEC=12) The xLAHQR vs xLAQR0 crossover point.
	Default: 75. (Must be at least 11.)

	IPARMQ(ISPEC=13) Recommended deflation window size.
	This depends on ILO, IHI and NS, the
	number of simultaneous shifts returned
	by IPARMQ(ISPEC=15).  The default for
	(IHI-ILO+1).LE.500 is NS.  The default
	for (IHI-ILO+1).GT.500 is 3*NS/2.

	IPARMQ(ISPEC=14) Nibble crossover point.  Default: 14.

	IPARMQ(ISPEC=15) Number of simultaneous shifts, NS.
	a multi-shift QR iteration.

	If IHI-ILO+1 is ...

	greater than      ...but less    ... the
	or equal to ...      than        default is

	0               30       NS =   2+
	30               60       NS =   4+
	60              150       NS =  10
	150              590       NS =  **
	590             3000       NS =  64
	3000             6000       NS = 128
	6000             infinity   NS = 256

	(+)  By default matrices of this order are
	passed to the implicit double shift routine
	xLAHQR.  See IPARMQ(ISPEC=12) above.   These
	values of NS are used only in case of a rare
	xLAHQR failure.

	(**) The asterisks (**) indicate an ad-hoc
	function increasing from 10 to 64.

	IPARMQ(ISPEC=16) Select structured matrix multiply.
	(See ISPEC=16 above for details.)
	Default: 3.

	================================================================ */
	/* System generated locals */
	integer ret_val, i__1, i__2;
	real r__1;
	/* Builtin functions */
	double log(doublereal);
	/* Local variables */
	static integer nh, ns;

	if (*ispec == 15 || *ispec == 13 || *ispec == 16) {

		/*        ==== Set the number simultaneous shifts ==== */

		nh = *ihi - *ilo + 1;
		ns = 2;
		if (nh >= 30) {
			ns = 4;
		}
		if (nh >= 60) {
			ns = 10;
		}
		if (nh >= 150) {
			/* Computing MAX */
			r__1 = log((real)nh) / log(2.f);
			i__1 = 10, i__2 = nh / i_nint(&r__1);
			ns = max(i__1, i__2);
		}
		if (nh >= 590) {
			ns = 64;
		}
		if (nh >= 3000) {
			ns = 128;
		}
		if (nh >= 6000) {
			ns = 256;
		}
		/* Computing MAX */
		i__1 = 2, i__2 = ns - ns % 2;
		ns = max(i__1, i__2);
	}

	if (*ispec == 12) {


		/*        ===== Matrices of order smaller than NMIN get sent
		.     to xLAHQR, the classic double shift algorithm.
		.     This must be at least 11. ==== */

		ret_val = 75;

	}
	else if (*ispec == 14) {

		/*        ==== INIBL: skip a multi-shift qr iteration and
		.    whenever aggressive early deflation finds
		.    at least (NIBBLE*(window size)/100) deflations. ==== */

		ret_val = 14;

	}
	else if (*ispec == 15) {

		/*        ==== NSHFTS: The number of simultaneous shifts ===== */

		ret_val = ns;

	}
	else if (*ispec == 13) {

		/*        ==== NW: deflation window size.  ==== */

		if (nh <= 500) {
			ret_val = ns;
		}
		else {
			ret_val = ns * 3 / 2;
		}

	}
	else if (*ispec == 16) {

		/*        ==== IACC22: Whether to accumulate reflections
		.     before updating the far-from-diagonal elements
		.     and whether to use 2-by-2 block structure while
		.     doing it.  A small amount of work could be saved
		.     by making this choice dependent also upon the
		.     NH=IHI-ILO+1. */

		ret_val = 0;
		if (ns >= 14) {
			ret_val = 1;
		}
		if (ns >= 14) {
			ret_val = 2;
		}

	}
	else {
		/*        ===== invalid value of ispec ===== */
		ret_val = -1;

	}

	/*     ==== End of IPARMQ ==== */

	return ret_val;
} /* iparmq_ */

void s_copy(register char *a, register char *b, ftnlen la, ftnlen lb)
{
	register char *aend, *bend;

	aend = a + la;

	if (la <= lb)
#ifndef NO_OVERWRITE
		if (a <= b || a >= b + la)
#endif
			while (a < aend)
				*a++ = *b++;
#ifndef NO_OVERWRITE
		else
			for (b += la; a < aend; )
				*--aend = *--b;
#endif

	else {
		bend = b + lb;
#ifndef NO_OVERWRITE
		if (a <= b || a >= bend)
#endif
			while (b < bend)
				*a++ = *b++;
#ifndef NO_OVERWRITE
		else {
			a += lb;
			while (b < bend)
				*--a = *--bend;
			a += lb;
		}
#endif
		while (a < aend)
			*a++ = ' ';
	}
}

integer s_cmp(char *a0, char *b0, ftnlen la, ftnlen lb)
{
	register unsigned char *a, *aend, *b, *bend;
	a = (unsigned char *)a0;
	b = (unsigned char *)b0;
	aend = a + la;
	bend = b + lb;

	if (la <= lb)
	{
		while (a < aend)
			if (*a != *b)
				return(*a - *b);
			else
			{
				++a; ++b;
			}

		while (b < bend)
			if (*b != ' ')
				return(' ' - *b);
			else	++b;
	}

	else
	{
		while (b < bend)
			if (*a == *b)
			{
				++a; ++b;
			}
			else
				return(*a - *b);
		while (a < aend)
			if (*a != ' ')
				return(*a - ' ');
			else	++a;
	}
	return(0);
}

integer ilaenv_(integer *ispec, char *name__, char *opts, integer *n1,
	integer *n2, integer *n3, integer *n4, ftnlen name_len, ftnlen
	opts_len)
{
	/* System generated locals */
	integer ret_val;

	/* Local variables */
	static integer i__;
	static char c1[1], c2[2], c3[3], c4[2];
	static integer ic, nb, iz, nx;
	static logical cname;
	static integer nbmin;
	static logical sname;
	static char subnam[6];


	/*  -- LAPACK auxiliary routine (version 3.1.1) --
	Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
	January 2007


	Purpose
	=======

	ILAENV is called from the LAPACK routines to choose problem-dependent
	parameters for the local environment.  See ISPEC for a description of
	the parameters.

	ILAENV returns an INTEGER
	if ILAENV >= 0: ILAENV returns the value of the parameter specified by ISPEC
	if ILAENV < 0:  if ILAENV = -k, the k-th argument had an illegal value.

	This version provides a set of parameters which should give good,
	but not optimal, performance on many of the currently available
	computers.  Users are encouraged to modify this subroutine to set
	the tuning parameters for their particular machine using the option
	and problem size information in the arguments.

	This routine will not function correctly if it is converted to all
	lower case.  Converting it to all upper case is allowed.

	Arguments
	=========

	ISPEC   (input) INTEGER
	Specifies the parameter to be returned as the value of
	ILAENV.
	= 1: the optimal blocksize; if this value is 1, an unblocked
	algorithm will give the best performance.
	= 2: the minimum block size for which the block routine
	should be used; if the usable block size is less than
	this value, an unblocked routine should be used.
	= 3: the crossover point (in a block routine, for N less
	than this value, an unblocked routine should be used)
	= 4: the number of shifts, used in the nonsymmetric
	eigenvalue routines (DEPRECATED)
	= 5: the minimum column dimension for blocking to be used;
	rectangular blocks must have dimension at least k by m,
	where k is given by ILAENV(2,...) and m by ILAENV(5,...)
	= 6: the crossover point for the SVD (when reducing an m by n
	matrix to bidiagonal form, if max(m,n)/min(m,n) exceeds
	this value, a QR factorization is used first to reduce
	the matrix to a triangular form.)
	= 7: the number of processors
	= 8: the crossover point for the multishift QR method
	for nonsymmetric eigenvalue problems (DEPRECATED)
	= 9: maximum size of the subproblems at the bottom of the
	computation tree in the divide-and-conquer algorithm
	(used by xGELSD and xGESDD)
	=10: ieee NaN arithmetic can be trusted not to trap
	=11: infinity arithmetic can be trusted not to trap
	12 <= ISPEC <= 16:
	xHSEQR or one of its subroutines,
	see IPARMQ for detailed explanation

	NAME    (input) CHARACTER*(*)
	The name of the calling subroutine, in either upper case or
	lower case.

	OPTS    (input) CHARACTER*(*)
	The character options to the subroutine NAME, concatenated
	into a single character string.  For example, UPLO = 'U',
	TRANS = 'T', and DIAG = 'N' for a triangular routine would
	be specified as OPTS = 'UTN'.

	N1      (input) INTEGER
	N2      (input) INTEGER
	N3      (input) INTEGER
	N4      (input) INTEGER
	Problem dimensions for the subroutine NAME; these may not all
	be required.

	Further Details
	===============

	The following conventions have been used when calling ILAENV from the
	LAPACK routines:
	1)  OPTS is a concatenation of all of the character options to
	subroutine NAME, in the same order that they appear in the
	argument list for NAME, even if they are not used in determining
	the value of the parameter specified by ISPEC.
	2)  The problem dimensions N1, N2, N3, N4 are specified in the order
	that they appear in the argument list for NAME.  N1 is used
	first, N2 second, and so on, and unused problem dimensions are
	passed a value of -1.
	3)  The parameter value returned by ILAENV is checked for validity in
	the calling subroutine.  For example, ILAENV is used to retrieve
	the optimal blocksize for STRTRI as follows:

	NB = ILAENV( 1, 'STRTRI', UPLO // DIAG, N, -1, -1, -1 )
	IF( NB.LE.1 ) NB = MAX( 1, N )

	===================================================================== */


	switch (*ispec) {
	case 1:  goto L10;
	case 2:  goto L10;
	case 3:  goto L10;
	case 4:  goto L80;
	case 5:  goto L90;
	case 6:  goto L100;
	case 7:  goto L110;
	case 8:  goto L120;
	case 9:  goto L130;
	case 10:  goto L140;
	case 11:  goto L150;
	case 12:  goto L160;
	case 13:  goto L160;
	case 14:  goto L160;
	case 15:  goto L160;
	case 16:  goto L160;
	}

	/*     Invalid value for ISPEC */

	ret_val = -1;
	return ret_val;

L10:

	/*     Convert NAME to upper case if the first character is lower case. */

	ret_val = 1;
	s_copy(subnam, name__, (ftnlen)6, name_len);
	ic = *(unsigned char *)subnam;
	iz = 'Z';
	if (iz == 90 || iz == 122) {

		/*        ASCII character set */

		if (ic >= 97 && ic <= 122) {
			*(unsigned char *)subnam = (char)(ic - 32);
			for (i__ = 2; i__ <= 6; ++i__) {
				ic = *(unsigned char *)&subnam[i__ - 1];
				if (ic >= 97 && ic <= 122) {
					*(unsigned char *)&subnam[i__ - 1] = (char)(ic - 32);
				}
				/* L20: */
			}
		}

	}
	else if (iz == 233 || iz == 169) {

		/*        EBCDIC character set */

		if (ic >= 129 && ic <= 137 || ic >= 145 && ic <= 153 || ic >= 162 &&
			ic <= 169) {
			*(unsigned char *)subnam = (char)(ic + 64);
			for (i__ = 2; i__ <= 6; ++i__) {
				ic = *(unsigned char *)&subnam[i__ - 1];
				if (ic >= 129 && ic <= 137 || ic >= 145 && ic <= 153 || ic >=
					162 && ic <= 169) {
					*(unsigned char *)&subnam[i__ - 1] = (char)(ic + 64);
				}
				/* L30: */
			}
		}

	}
	else if (iz == 218 || iz == 250) {

		/*        Prime machines:  ASCII+128 */

		if (ic >= 225 && ic <= 250) {
			*(unsigned char *)subnam = (char)(ic - 32);
			for (i__ = 2; i__ <= 6; ++i__) {
				ic = *(unsigned char *)&subnam[i__ - 1];
				if (ic >= 225 && ic <= 250) {
					*(unsigned char *)&subnam[i__ - 1] = (char)(ic - 32);
				}
				/* L40: */
			}
		}
	}

	*(unsigned char *)c1 = *(unsigned char *)subnam;
	sname = *(unsigned char *)c1 == 'S' || *(unsigned char *)c1 == 'D';
	cname = *(unsigned char *)c1 == 'C' || *(unsigned char *)c1 == 'Z';
	if (!(cname || sname)) {
		return ret_val;
	}
	s_copy(c2, subnam + 1, (ftnlen)2, (ftnlen)2);
	s_copy(c3, subnam + 3, (ftnlen)3, (ftnlen)3);
	s_copy(c4, c3 + 1, (ftnlen)2, (ftnlen)2);

	switch (*ispec) {
	case 1:  goto L50;
	case 2:  goto L60;
	case 3:  goto L70;
	}

L50:

	/*     ISPEC = 1:  block size

	In these examples, separate code is provided for setting NB for
	real and complex.  We assume that NB will take the same value in
	single or double precision. */

	nb = 1;

	if (s_cmp(c2, GE_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRF_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nb = 64;
			}
			else {
				nb = 64;
			}
		}
		else if (s_cmp(c3, QRF_buffer, (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3,
			RQF_buffer, (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, LQF_buffer, (ftnlen)
				3, (ftnlen)3) == 0 || s_cmp(c3, QLF_buffer, (ftnlen)3, (ftnlen)3)
			== 0) {
			if (sname) {
				nb = 32;
			}
			else {
				nb = 32;
			}
		}
		else if (s_cmp(c3, HRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nb = 32;
			}
			else {
				nb = 32;
			}
		}
		else if (s_cmp(c3, BRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nb = 32;
			}
			else {
				nb = 32;
			}
		}
		else if (s_cmp(c3, TRI_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nb = 64;
			}
			else {
				nb = 64;
			}
		}
	}
	else if (s_cmp(c2, PO_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRF_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nb = 64;
			}
			else {
				nb = 64;
			}
		}
	}
	else if (s_cmp(c2, SY_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRF_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nb = 64;
			}
			else {
				nb = 64;
			}
		}
		else if (sname && s_cmp(c3, TRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nb = 32;
		}
		else if (sname && s_cmp(c3, GST_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nb = 64;
		}
	}
	else if (cname && s_cmp(c2, HE_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRF_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nb = 64;
		}
		else if (s_cmp(c3, TRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nb = 32;
		}
		else if (s_cmp(c3, GST_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nb = 64;
		}
	}
	else if (sname && s_cmp(c2, OR_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (*(unsigned char *)c3 == 'G') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nb = 32;
			}
		}
		else if (*(unsigned char *)c3 == 'M') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nb = 32;
			}
		}
	}
	else if (cname && s_cmp(c2, UN_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (*(unsigned char *)c3 == 'G') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nb = 32;
			}
		}
		else if (*(unsigned char *)c3 == 'M') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nb = 32;
			}
		}
	}
	else if (s_cmp(c2, GB_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRF_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				if (*n4 <= 64) {
					nb = 1;
				}
				else {
					nb = 32;
				}
			}
			else {
				if (*n4 <= 64) {
					nb = 1;
				}
				else {
					nb = 32;
				}
			}
		}
	}
	else if (s_cmp(c2, PB_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRF_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				if (*n2 <= 64) {
					nb = 1;
				}
				else {
					nb = 32;
				}
			}
			else {
				if (*n2 <= 64) {
					nb = 1;
				}
				else {
					nb = 32;
				}
			}
		}
	}
	else if (s_cmp(c2, TR_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRI_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nb = 64;
			}
			else {
				nb = 64;
			}
		}
	}
	else if (s_cmp(c2, LA_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, UUM_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nb = 64;
			}
			else {
				nb = 64;
			}
		}
	}
	else if (sname && s_cmp(c2, ST_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, EBZ_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nb = 1;
		}
	}
	ret_val = nb;
	return ret_val;

L60:

	/*     ISPEC = 2:  minimum block size */

	nbmin = 2;
	if (s_cmp(c2, GE_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, QRF_buffer, (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, RQF_buffer, (
			ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, LQF_buffer, (ftnlen)3, (
				ftnlen)3) == 0 || s_cmp(c3, QLF_buffer, (ftnlen)3, (ftnlen)3) == 0)
		{
			if (sname) {
				nbmin = 2;
			}
			else {
				nbmin = 2;
			}
		}
		else if (s_cmp(c3, HRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nbmin = 2;
			}
			else {
				nbmin = 2;
			}
		}
		else if (s_cmp(c3, BRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nbmin = 2;
			}
			else {
				nbmin = 2;
			}
		}
		else if (s_cmp(c3, TRI_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nbmin = 2;
			}
			else {
				nbmin = 2;
			}
		}
	}
	else if (s_cmp(c2, SY_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRF_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nbmin = 8;
			}
			else {
				nbmin = 8;
			}
		}
		else if (sname && s_cmp(c3, TRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nbmin = 2;
		}
	}
	else if (cname && s_cmp(c2, HE_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nbmin = 2;
		}
	}
	else if (sname && s_cmp(c2, OR_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (*(unsigned char *)c3 == 'G') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nbmin = 2;
			}
		}
		else if (*(unsigned char *)c3 == 'M') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nbmin = 2;
			}
		}
	}
	else if (cname && s_cmp(c2, UN_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (*(unsigned char *)c3 == 'G') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nbmin = 2;
			}
		}
		else if (*(unsigned char *)c3 == 'M') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nbmin = 2;
			}
		}
	}
	ret_val = nbmin;
	return ret_val;

L70:

	/*     ISPEC = 3:  crossover point */

	nx = 0;
	if (s_cmp(c2, GE_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, QRF_buffer, (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, RQF_buffer, (
			ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, LQF_buffer, (ftnlen)3, (
				ftnlen)3) == 0 || s_cmp(c3, QLF_buffer, (ftnlen)3, (ftnlen)3) == 0)
		{
			if (sname) {
				nx = 128;
			}
			else {
				nx = 128;
			}
		}
		else if (s_cmp(c3, HRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nx = 128;
			}
			else {
				nx = 128;
			}
		}
		else if (s_cmp(c3, BRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			if (sname) {
				nx = 128;
			}
			else {
				nx = 128;
			}
		}
	}
	else if (s_cmp(c2, SY_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (sname && s_cmp(c3, TRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nx = 32;
		}
	}
	else if (cname && s_cmp(c2, HE_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (s_cmp(c3, TRD_buffer, (ftnlen)3, (ftnlen)3) == 0) {
			nx = 32;
		}
	}
	else if (sname && s_cmp(c2, OR_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (*(unsigned char *)c3 == 'G') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nx = 128;
			}
		}
	}
	else if (cname && s_cmp(c2, UN_buffer, (ftnlen)2, (ftnlen)2) == 0) {
		if (*(unsigned char *)c3 == 'G') {
			if (s_cmp(c4, QR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, RQ_buffer,
				(ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, LQ_buffer, (ftnlen)2, (
					ftnlen)2) == 0 || s_cmp(c4, QL_buffer, (ftnlen)2, (ftnlen)2) ==
				0 || s_cmp(c4, HR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
					c4, TR_buffer, (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, BR_buffer, (
						ftnlen)2, (ftnlen)2) == 0) {
				nx = 128;
			}
		}
	}
	ret_val = nx;
	return ret_val;

L80:

	/*     ISPEC = 4:  number of shifts (used by xHSEQR) */

	ret_val = 6;
	return ret_val;

L90:

	/*     ISPEC = 5:  minimum column dimension (not used) */

	ret_val = 2;
	return ret_val;

L100:

	/*     ISPEC = 6:  crossover point for SVD (used by xGELSS and xGESVD) */

	ret_val = (integer)((real)min(*n1, *n2) * 1.6f);
	return ret_val;

L110:

	/*     ISPEC = 7:  number of processors (not used) */

	ret_val = 1;
	return ret_val;

L120:

	/*     ISPEC = 8:  crossover point for multishift (used by xHSEQR) */

	ret_val = 50;
	return ret_val;

L130:

	/*     ISPEC = 9:  maximum size of the subproblems at the bottom of the
	computation tree in the divide-and-conquer algorithm
	(used by xGELSD and xGESDD) */

	ret_val = 25;
	return ret_val;

L140:

	/*     ISPEC = 10: ieee NaN arithmetic can be trusted not to trap

	ILAENV = 0 */
	ret_val = 1;
	if (ret_val == 1) {
		ret_val = ieeeck_(&c__0, &c_b163, &c_b164);
	}
	return ret_val;

L150:

	/*     ISPEC = 11: infinity arithmetic can be trusted not to trap

	ILAENV = 0 */
	ret_val = 1;
	if (ret_val == 1) {
		ret_val = ieeeck_(&c__1, &c_b163, &c_b164);
	}
	return ret_val;

L160:

	/*     12 <= ISPEC <= 16: xHSEQR or one of its subroutines. */

	//ret_val = iparmq_(ispec, name__, opts, n1, n2, n3, n4, name_len, opts_len);
	ret_val = iparmq_(ispec, name__, opts, n1, n2, n3, n4);
	return ret_val;

	/*     End of ILAENV */

} /* ilaenv_ */


/* Subroutine */ int cher_(char *uplo, integer *n, real *alpha, complex *x,
	integer *incx, complex *a, integer *lda)
{
	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;
	real r__1;
	complex q__1, q__2;
	/* Builtin functions */
	/* Local variables */
	static integer i__, j, ix, jx, kx, info;
	static complex temp;
	/*  Purpose
	=======
	CHER   performs the hermitian rank 1 operation
	A := alpha*x*conjg( x' ) + A,
	where alpha is a real scalar, x is an n element vector and A is an
	n by n hermitian matrix.
	Arguments
	==========
	UPLO   - CHARACTER*1.
	On entry, UPLO specifies whether the upper or lower
	triangular part of the array A is to be referenced as
	follows:
	UPLO = 'U' or 'u'   Only the upper triangular part of A
	is to be referenced.
	UPLO = 'L' or 'l'   Only the lower triangular part of A
	is to be referenced.
	Unchanged on exit.
	N      - INTEGER.
	On entry, N specifies the order of the matrix A.
	N must be at least zero.
	Unchanged on exit.
	ALPHA  - REAL            .
	On entry, ALPHA specifies the scalar alpha.
	Unchanged on exit.
	X      - COMPLEX          array of dimension at least
	( 1 + ( n - 1 )*abs( INCX ) ).
	Before entry, the incremented array X must contain the n
	element vector x.
	Unchanged on exit.
	INCX   - INTEGER.
	On entry, INCX specifies the increment for the elements of
	X. INCX must not be zero.
	Unchanged on exit.
	A      - COMPLEX          array of DIMENSION ( LDA, n ).
	Before entry with  UPLO = 'U' or 'u', the leading n by n
	upper triangular part of the array A must contain the upper
	triangular part of the hermitian matrix and the strictly
	lower triangular part of A is not referenced. On exit, the
	upper triangular part of the array A is overwritten by the
	upper triangular part of the updated matrix.
	Before entry with UPLO = 'L' or 'l', the leading n by n
	lower triangular part of the array A must contain the lower
	triangular part of the hermitian matrix and the strictly
	upper triangular part of A is not referenced. On exit, the
	lower triangular part of the array A is overwritten by the
	lower triangular part of the updated matrix.
	Note that the imaginary parts of the diagonal elements need
	not be set, they are assumed to be zero, and on exit they
	are set to zero.
	LDA    - INTEGER.
	On entry, LDA specifies the first dimension of A as declared
	in the calling (sub) program. LDA must be at least
	max( 1, n ).
	Unchanged on exit.
	Level 2 Blas routine.
	-- Written on 22-October-1986.
	Jack Dongarra, Argonne National Lab.
	Jeremy Du Croz, Nag Central Office.
	Sven Hammarling, Nag Central Office.
	Richard Hanson, Sandia National Labs.
	Test the input parameters.
	Parameter adjustments */
	--x;
	a_dim1 = *lda;
	a_offset = 1 + a_dim1;
	a -= a_offset;
	/* Function Body */
	info = 0;
	if (!lsame_(uplo, U_buffer) && !lsame_(uplo, L_buffer)) {
		info = 1;
	}
	else if (*n < 0) {
		info = 2;
	}
	else if (*incx == 0) {
		info = 5;
	}
	else if (*lda < max(1, *n)) {
		info = 7;
	}
	if (info != 0) {
		xerbla_(CHER1_buffer, &info);
		return 0;
	}
	/*     Quick return if possible. */
	if (*n == 0 || *alpha == 0.f) {
		return 0;
	}
	/*     Set the start point in X if the increment is not unity. */
	if (*incx <= 0) {
		kx = 1 - (*n - 1) * *incx;
	}
	else if (*incx != 1) {
		kx = 1;
	}
	/*     Start the operations. In this version the elements of A are
	accessed sequentially with one pass through the triangular part
	of A. */
	if (lsame_(uplo, U_buffer)) {
		/*        Form  A  when A is stored in upper triangle. */
		if (*incx == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = j;
				if (x[i__2].r != 0.f || x[i__2].i != 0.f) {
					r_cnjg(&q__2, &x[j]);
					q__1.r = *alpha * q__2.r, q__1.i = *alpha * q__2.i;
					temp.r = q__1.r, temp.i = q__1.i;
					i__2 = j - 1;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * a_dim1;
						i__4 = i__ + j * a_dim1;
						i__5 = i__;
						q__2.r = x[i__5].r * temp.r - x[i__5].i * temp.i,
							q__2.i = x[i__5].r * temp.i + x[i__5].i *
							temp.r;
						q__1.r = a[i__4].r + q__2.r, q__1.i = a[i__4].i +
							q__2.i;
						a[i__3].r = q__1.r, a[i__3].i = q__1.i;
						/* L10: */
					}
					i__2 = j + j * a_dim1;
					i__3 = j + j * a_dim1;
					i__4 = j;
					q__1.r = x[i__4].r * temp.r - x[i__4].i * temp.i, q__1.i =
						x[i__4].r * temp.i + x[i__4].i * temp.r;
					r__1 = a[i__3].r + q__1.r;
					a[i__2].r = r__1, a[i__2].i = 0.f;
				}
				else {
					i__2 = j + j * a_dim1;
					i__3 = j + j * a_dim1;
					r__1 = a[i__3].r;
					a[i__2].r = r__1, a[i__2].i = 0.f;
				}
				/* L20: */
			}
		}
		else {
			jx = kx;
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = jx;
				if (x[i__2].r != 0.f || x[i__2].i != 0.f) {
					r_cnjg(&q__2, &x[jx]);
					q__1.r = *alpha * q__2.r, q__1.i = *alpha * q__2.i;
					temp.r = q__1.r, temp.i = q__1.i;
					ix = kx;
					i__2 = j - 1;
					for (i__ = 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * a_dim1;
						i__4 = i__ + j * a_dim1;
						i__5 = ix;
						q__2.r = x[i__5].r * temp.r - x[i__5].i * temp.i,
							q__2.i = x[i__5].r * temp.i + x[i__5].i *
							temp.r;
						q__1.r = a[i__4].r + q__2.r, q__1.i = a[i__4].i +
							q__2.i;
						a[i__3].r = q__1.r, a[i__3].i = q__1.i;
						ix += *incx;
						/* L30: */
					}
					i__2 = j + j * a_dim1;
					i__3 = j + j * a_dim1;
					i__4 = jx;
					q__1.r = x[i__4].r * temp.r - x[i__4].i * temp.i, q__1.i =
						x[i__4].r * temp.i + x[i__4].i * temp.r;
					r__1 = a[i__3].r + q__1.r;
					a[i__2].r = r__1, a[i__2].i = 0.f;
				}
				else {
					i__2 = j + j * a_dim1;
					i__3 = j + j * a_dim1;
					r__1 = a[i__3].r;
					a[i__2].r = r__1, a[i__2].i = 0.f;
				}
				jx += *incx;
				/* L40: */
			}
		}
	}
	else {
		/*        Form  A  when A is stored in lower triangle. */
		if (*incx == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = j;
				if (x[i__2].r != 0.f || x[i__2].i != 0.f) {
					r_cnjg(&q__2, &x[j]);
					q__1.r = *alpha * q__2.r, q__1.i = *alpha * q__2.i;
					temp.r = q__1.r, temp.i = q__1.i;
					i__2 = j + j * a_dim1;
					i__3 = j + j * a_dim1;
					i__4 = j;
					q__1.r = temp.r * x[i__4].r - temp.i * x[i__4].i, q__1.i =
						temp.r * x[i__4].i + temp.i * x[i__4].r;
					r__1 = a[i__3].r + q__1.r;
					a[i__2].r = r__1, a[i__2].i = 0.f;
					i__2 = *n;
					for (i__ = j + 1; i__ <= i__2; ++i__) {
						i__3 = i__ + j * a_dim1;
						i__4 = i__ + j * a_dim1;
						i__5 = i__;
						q__2.r = x[i__5].r * temp.r - x[i__5].i * temp.i,
							q__2.i = x[i__5].r * temp.i + x[i__5].i *
							temp.r;
						q__1.r = a[i__4].r + q__2.r, q__1.i = a[i__4].i +
							q__2.i;
						a[i__3].r = q__1.r, a[i__3].i = q__1.i;
						/* L50: */
					}
				}
				else {
					i__2 = j + j * a_dim1;
					i__3 = j + j * a_dim1;
					r__1 = a[i__3].r;
					a[i__2].r = r__1, a[i__2].i = 0.f;
				}
				/* L60: */
			}
		}
		else {
			jx = kx;
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = jx;
				if (x[i__2].r != 0.f || x[i__2].i != 0.f) {
					r_cnjg(&q__2, &x[jx]);
					q__1.r = *alpha * q__2.r, q__1.i = *alpha * q__2.i;
					temp.r = q__1.r, temp.i = q__1.i;
					i__2 = j + j * a_dim1;
					i__3 = j + j * a_dim1;
					i__4 = jx;
					q__1.r = temp.r * x[i__4].r - temp.i * x[i__4].i, q__1.i =
						temp.r * x[i__4].i + temp.i * x[i__4].r;
					r__1 = a[i__3].r + q__1.r;
					a[i__2].r = r__1, a[i__2].i = 0.f;
					ix = jx;
					i__2 = *n;
					for (i__ = j + 1; i__ <= i__2; ++i__) {
						ix += *incx;
						i__3 = i__ + j * a_dim1;
						i__4 = i__ + j * a_dim1;
						i__5 = ix;
						q__2.r = x[i__5].r * temp.r - x[i__5].i * temp.i,
							q__2.i = x[i__5].r * temp.i + x[i__5].i *
							temp.r;
						q__1.r = a[i__4].r + q__2.r, q__1.i = a[i__4].i +
							q__2.i;
						a[i__3].r = q__1.r, a[i__3].i = q__1.i;
						/* L70: */
					}
				}
				else {
					i__2 = j + j * a_dim1;
					i__3 = j + j * a_dim1;
					r__1 = a[i__3].r;
					a[i__2].r = r__1, a[i__2].i = 0.f;
				}
				jx += *incx;
				/* L80: */
			}
		}
	}
	return 0;
	/*     End of CHER  . */
} /* cher_ */


doublereal scabs1_(complex *z__)
{
	/* System generated locals */
	real ret_val, r__1, r__2;
	/* Builtin functions */
	double r_imag(complex *);
	/*  Purpose
	=======
	SCABS1 computes absolute value of a complex number */
	ret_val = (r__1 = z__->r, dabs(r__1)) + (r__2 = r_imag(z__), dabs(r__2));
	return ret_val;
} /* scabs1_ */

integer icamax_(integer *n, complex *cx, integer *incx)
{
	/* System generated locals */
	integer ret_val, i__1;
	/* Local variables */
	static integer i__, ix;
	static real smax;
	/*  Purpose
	=======
	finds the index of element having max. absolute value.
	jack dongarra, linpack, 3/11/78.
	modified 3/93 to return if incx .le. 0.
	modified 12/3/93, array(1) declarations changed to array(*)
	Parameter adjustments */
	--cx;
	/* Function Body */
	ret_val = 0;
	if (*n < 1 || *incx <= 0) {
		return ret_val;
	}
	ret_val = 1;
	if (*n == 1) {
		return ret_val;
	}
	if (*incx == 1) {
		goto L20;
	}
	/*        code for increment not equal to 1 */
	ix = 1;
	smax = scabs1_(&cx[1]);
	ix += *incx;
	i__1 = *n;
	for (i__ = 2; i__ <= i__1; ++i__) {
		if (scabs1_(&cx[ix]) <= smax) {
			goto L5;
		}
		ret_val = i__;
		smax = scabs1_(&cx[ix]);
	L5:
		ix += *incx;
		/* L10: */
	}
	return ret_val;
	/*        code for increment equal to 1 */
L20:
	smax = scabs1_(&cx[1]);
	i__1 = *n;
	for (i__ = 2; i__ <= i__1; ++i__) {
		if (scabs1_(&cx[i__]) <= smax) {
			goto L30;
		}
		ret_val = i__;
		smax = scabs1_(&cx[i__]);
	L30:
		;
	}
	return ret_val;
} /* icamax_ */

/* Subroutine */ int cswap_(integer *n, complex *cx, integer *incx, complex *cy, integer *incy)
{
	/* System generated locals */
	integer i__1, i__2, i__3;
	/* Local variables */
	static integer i__, ix, iy;
	static complex ctemp;
	/*  Purpose
	=======
	interchanges two vectors.
	jack dongarra, linpack, 3/11/78.
	modified 12/3/93, array(1) declarations changed to array(*)
	Parameter adjustments */
	--cy;
	--cx;
	/* Function Body */
	if (*n <= 0) {
		return 0;
	}
	if (*incx == 1 && *incy == 1) {
		goto L20;
	}
	/*       code for unequal increments or equal increments not equal
	to 1 */
	ix = 1;
	iy = 1;
	if (*incx < 0) {
		ix = (-(*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
		iy = (-(*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = ix;
		ctemp.r = cx[i__2].r, ctemp.i = cx[i__2].i;
		i__2 = ix;
		i__3 = iy;
		cx[i__2].r = cy[i__3].r, cx[i__2].i = cy[i__3].i;
		i__2 = iy;
		cy[i__2].r = ctemp.r, cy[i__2].i = ctemp.i;
		ix += *incx;
		iy += *incy;
		/* L10: */
	}
	return 0;
	/*       code for both increments equal to 1 */
L20:
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = i__;
		ctemp.r = cx[i__2].r, ctemp.i = cx[i__2].i;
		i__2 = i__;
		i__3 = i__;
		cx[i__2].r = cy[i__3].r, cx[i__2].i = cy[i__3].i;
		i__2 = i__;
		cy[i__2].r = ctemp.r, cy[i__2].i = ctemp.i;
		/* L30: */
	}
	return 0;
} /* cswap_ */

/* Subroutine */ int ccopy_(integer *n, complex *cx, integer *incx, complex *
	cy, integer *incy)
{
	/* System generated locals */
	integer i__1, i__2, i__3;
	/* Local variables */
	static integer i__, ix, iy;
	/*  Purpose
	=======
	CCOPY copies a vector x to a vector y.
	Further Details
	===============
	jack dongarra, linpack, 3/11/78.
	modified 12/3/93, array(1) declarations changed to array(*)
	Parameter adjustments */
	--cy;
	--cx;
	/* Function Body */
	if (*n <= 0) {
		return 0;
	}
	if (*incx == 1 && *incy == 1) {
		goto L20;
	}
	/*        code for unequal increments or equal increments
	not equal to 1 */
	ix = 1;
	iy = 1;
	if (*incx < 0) {
		ix = (-(*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
		iy = (-(*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = iy;
		i__3 = ix;
		cy[i__2].r = cx[i__3].r, cy[i__2].i = cx[i__3].i;
		ix += *incx;
		iy += *incy;
		/* L10: */
	}
	return 0;
	/*        code for both increments equal to 1 */
L20:
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = i__;
		i__3 = i__;
		cy[i__2].r = cx[i__3].r, cy[i__2].i = cx[i__3].i;
		/* L30: */
	}
	return 0;
} /* ccopy_ */