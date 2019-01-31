#ifndef INVERSE_H
#define INVERSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>


typedef long int integer;
typedef unsigned long int uinteger;
typedef char *address;
typedef short int shortint;
typedef float real;
typedef double doublereal;
typedef struct { real r, i; } complex;
typedef struct { doublereal r, i; } doublecomplex;
typedef long int logical;
typedef short int shortlogical;
typedef char logical1;
typedef char integer1;
typedef long int flag;
typedef long int ftnlen;
typedef long int ftnint;

/*close*/
typedef struct
{
	flag cerr;
	ftnint cunit;
	char *csta;
} cllist;

// Signal types
#ifndef SIGINT
#define SIGINT          2   // interrupt
#endif

#ifndef SIGILL
#define SIGILL          4   // illegal instruction - invalid function image
#endif

#ifndef SIGFPE
#define SIGFPE          8   // floating point exception
#endif 

#ifndef SIGSEGV
#define SIGSEGV         11  // segment violation
#endif

#ifndef SIGTERM
#define SIGTERM         15  // Software termination signal from kill
#endif 

#ifndef SIGBREAK
#define SIGBREAK        21  // Ctrl-Break sequence
#endif 

#ifndef SIGABRT
#define SIGABRT         22  // abnormal termination triggered by abort call
#endif 

//typedef void (__CRTDECL* _crt_signal_t)(int);

#ifndef SIG_DFL
//#define SIG_DFL ((_crt_signal_t)0)     // default signal action
#define SIG_DFL (0)     // default signal action
#endif

#ifndef SIG_IGN
//#define SIG_IGN ((_crt_signal_t)1)     // ignore signal
#define SIG_IGN (1)     // ignore signal
#endif

#ifndef SIG_GET
//#define SIG_GET ((_crt_signal_t)2)     // return current value
#define SIG_GET (2)     // return current value
#endif

#ifndef SIG_SGE
//#define SIG_SGE ((_crt_signal_t)3)     // signal gets error
#define SIG_SGE (3)     // signal gets error
#endif

#ifndef SIG_ACK
//#define SIG_ACK ((_crt_signal_t)4)     // acknowledge
#define SIG_ACK (4)     // acknowledge
#endif

#define MXUNIT 100

#define abs(x) ((x) >= 0 ? (x) : -(x))
#define dabs(x) (doublereal)abs(x)
#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))
#define dmin(a,b) (doublereal)min(a,b)
#define dmax(a,b) (doublereal)max(a,b)
#define bit_test(a,b)	((a) >> (b) & 1)
#define bit_clear(a,b)	((a) & ~((uinteger)1 << (b)))
#define bit_set(a,b)	((a) |  ((uinteger)1 << (b)))

#define err(f,m,s) {if(f) errno= m; else f__fatal(m,s); return(m);}
#define errfl(f,m,s) return err__fl((int)f,m,s)
#ifndef errno
#define errno (*_errno())
#endif 



#ifndef FOPEN
#define FOPEN fopen
#endif

#ifndef FREOPEN
#define FREOPEN freopen
#endif

#ifndef FSEEK
#define FSEEK fseek
#endif

#ifndef FSTAT
#define FSTAT fstat
#endif

#ifndef FTELL
#define FTELL ftell
#endif
#ifndef OFF_T
#define OFF_T long
#endif
#define NO_TRUNCATE


int chetrf_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, complex *work, integer *lwork, integer *info);
int chetri_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, complex *work, integer *info);


/*units*/
typedef struct
{
	FILE *ufd;	/*0=unconnected*/
	char *ufnm;
#ifndef MSDOS
	long uinode;
	int udev;
#endif
	int url;	/*0=sequential*/
	flag useek;	/*true=can backspace, use dir, ...*/
	flag ufmt;
	flag urw;	/* (1 for can read) | (2 for can write) */
	flag ublnk;
	flag uend;
	flag uwrt;	/*last io was write*/
	flag uscrtch;
} unit;

						/*rewind, backspace, endfile*/
typedef struct
{
	flag aerr;
	ftnint aunit;
} alist;



enum CBLAS_ORDER { CblasRowMajor = 101, CblasColMajor = 102 };


logical lsame_(char *ca, char *cb); 
/* Subroutine */ int chetf2_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, integer *info);
/* Subroutine */ int clahef_(char *uplo, integer *n, integer *nb, integer *kb, complex *a, integer *lda, integer *ipiv, complex *w, integer *ldw, integer *info);
/* Subroutine */ int xerbla_(char *srname, integer *info);
/* Subroutine */ int cgemm_(char *transa, char *transb, integer *m, integer *n, integer *k, complex *alpha, complex *a, integer *lda, complex *b, integer *ldb, complex *beta, complex *c__, integer *ldc);
/* Subroutine */ int cgemv_(char *trans, integer *m, integer *n, complex *alpha, complex *a, integer *lda, complex *x, integer *incx, complex *beta, complex *y, integer *incy);
/* Subroutine */ int clacgv_(integer *n, complex *x, integer *incx);
/* Subroutine */ int csscal_(integer *n, real *sa, complex *cx, integer *incx);
/* Subroutine */ int cher_(char *uplo, integer *n, real *alpha, complex *x, integer *incx, complex *a, integer *lda);
/* Subroutine */ integer icamax_(integer *n, complex *cx, integer *incx);
/* Subroutine */ int ccopy_(integer *n, complex *cx, integer *incx, complex *cy, integer *incy);
/* Subroutine */ int chemv_(char *uplo, integer *n, complex *alpha, complex *a, integer *lda, complex *x, integer *incx, complex *beta, complex *y, integer *incy);
/* Subroutine */ int cswap_(integer *n, complex *cx, integer *incx, complex *cy, integer *incy);

integer ieeeck_(integer *ispec, real *zero, real *one);
integer icamax_(integer *n, complex *cx, integer *incx);
integer ilaenv_(integer *ispec, char *name__, char *opts, integer *n1, integer *n2, integer *n3, integer *n4, ftnlen name_len, ftnlen opts_len);
double r_imag(complex *z);
void r_cnjg(complex *r, complex *z);
doublereal slapy2_(real *x, real *y);
logical sisnan_(real *sin__);
logical slaisnan_(real *sin1, real *sin2);
void c_div(complex *c, complex *a, complex *b);
double c_abs(complex *z);
/* Complex */ void cdotc_(complex * ret_val, integer *n, complex *cx, integer*incx, complex *cy, integer *incy);
integer iparmq_(integer *ispec, char *name__, char *opts, integer *n, integer*ilo, integer *ihi, integer *lwork);
doublereal scabs1_(complex *z__);
double f__cabs(double real, double imag);
integer i_nint(real *x);
void s_copy(register char *a, register char *b, ftnlen la, ftnlen lb);
void sig_die(register char *s, int kill);
void f_exit(void);
integer f_clos(cllist *a);
integer s_cmp(char *a0, char *b0, ftnlen la, ftnlen lb);
int t_runc(alist *a);
static int copy(FILE *from, register long len, FILE *to);


#ifdef __cplusplus				
};
#endif

#endif