#ifndef __MATRIX_H__
#define __MATRIX_H__

typedef struct {
	int col;
	int row;

	double** var;
} matrix;

#ifdef __cplusplus
extern "C" {
#endif

matrix* matrix_new( int row, int col );
void matrix_free( matrix* m );

matrix* matrix_multiple( matrix* a, matrix* b );
matrix* matrix_inv( matrix* m );

matrix* matrix_transpose( matrix* m );

void matrix_init( matrix* m, double* src );
void matrix_extract( double* dst, matrix* m );

void matrix_load_identity( matrix* m );

#ifdef __cplusplus
}
#endif

#endif
