#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "matrix.h"

#define EPS (1e-16)

matrix* matrix_new( int row, int col ){
	matrix* m;
	int		i;

	m = (matrix*)malloc(sizeof(matrix));

	m->row = row;
	m->col = col;

	m->var = (double**)malloc(sizeof(double*)*row);
	for( i = 0 ; i < row ; i++ ){
		m->var[i] = (double*)malloc(sizeof(double)*col);
		memset( m->var[i], 0, sizeof(double)*col );
	}

	return m;
}

void matrix_free( matrix* m ){

	int i;

	for( i = 0 ; i < m->row ; i++ ){
		free(m->var[i]);
	}
	free(m->var);
	free(m);

}

void matrix_init( matrix* m, double* src ){

	int i, j;
	int width = m->col;

	for( j = 0 ; j < m->row ; j++ )
		for( i = 0 ; i < width ; i++ )
			m->var[j][i] = src[j*width+i];

}

void matrix_extract( double* dst, matrix* m ){

	int i, j;
	int width = m->col;

	for( j = 0 ; j < m->row ; j++ )
		for( i = 0 ; i < width ; i++ )
			dst[j*width+i] = m->var[j][i];

}

void matrix_load_identity( matrix* m ){

	int i, j, iter;

	if( m->col != m->row )
		return;

	iter = m->col;
	for( j = 0 ; j < iter ; j++ )
		for( i = 0 ; i < iter ; i++ )
			if( i == j )
				m->var[j][i] = 1.0;
			else
				m->var[j][i] = 0.0;

}

matrix* matrix_transpose( matrix* m ){

	int i, j;
	matrix* t;

	t = matrix_new( m->col, m->row );
	for( j = 0 ; j < m->row ; j++ ){
		for( i = 0 ; i < m->col ; i++ ){
			t->var[i][j] = m->var[j][i];
		}
	}
	return t;
}

matrix* matrix_multiple( matrix* a, matrix* b ){

	matrix* m;
	int col, row, iter;
	int i, j, k;

	if( a->col != b->row )
		return NULL;

	row = a->row;
	col = b->col;

	iter = a->col;

	m = matrix_new( row, col );
	for( j = 0 ; j < row ; j++ ){
		for( i = 0 ; i < col ; i++ ){
			for( k = 0 ; k < iter ; k++ ){
				m->var[j][i] += a->var[j][k]*b->var[k][i];
			}
		}
	}
	return m;

}

matrix* matrix_inv( matrix* m ){

	matrix *inv, *n;
	int iter, i, j, k;

	double v;

	double tmp;
	int    max_key;

	if( m->col != m->row )
		return NULL;

	iter = m->row;
	inv = matrix_new( m->row, m->col );
	n = matrix_new( m->row, m->col*2 );

	// copy it
	for( j = 0 ; j < iter ; j++ )
		for( i = 0 ; i < iter ; i++ )
			n->var[j][i] = m->var[j][i];

	// insert identity matrix
	for( i = 0 ; i < iter ; i++ )
		n->var[i][i+iter] = 1.0;

	// start gauss elimination
	for( i = 0 ; i < iter ; i++ ){

		// find max
		max_key = i;
		for( j = i+1 ; j < iter ; j++ )
			if( n->var[j][i] > n->var[max_key][i] )
				max_key = j;

		// swap with current row
		if( max_key != i ){
			for( j = 0 ; j < iter*2 ; j++ ){
				tmp = n->var[i][j];
				n->var[i][j] = n->var[max_key][j];
				n->var[max_key][j] = tmp;
			}
		}

		// normalize
		v = n->var[i][i];
		for( j = i+1 ; j < iter*2 ; j++ )
			n->var[i][j] /= v + EPS;

		for( j = i+1 ; j < iter ; j++ ){
			v = n->var[j][i];
			n->var[j][i] = 0.0;
			for( k = i+1 ; k < iter*2 ; k++ ){
				n->var[j][k] -= n->var[i][k]*v;
			}
		}

	}
	for( i = iter-2 ; i >= 0 ; i-- ){

		for( j = i ; j >= 0 ; j-- ){
			v = n->var[j][i+1];
			for( k = 0 ; k < iter*2 ; k++ ){
				n->var[j][k] -= n->var[i+1][k]*v;
			}
		}
	}

	// copy it
	for( j = 0 ; j < iter ; j++ )
		for( i = 0 ; i < iter ; i++ )
			inv->var[j][i] = n->var[j][i+iter];

	matrix_free(n);

	return inv;
}


void matrix_print( matrix* m ){

	int i, j;
	for( j = 0 ; j < m->row ; j++ ){
		for( i = 0 ; i < m->col ; i++ ){
			fprintf( stdout, "%3.5lf ", m->var[j][i] );
		}
		fprintf( stdout, "\n" );
	}
	fprintf( stdout, "\n" );

}
