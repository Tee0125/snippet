#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "matrix.h"

#define WIDTH  640
#define HEIGHT 480

#define INFILE  "test2.raw"
#define OUTFILE "test2_%s.raw"

typedef unsigned char uchar;

static int bound_check( int x, int y ){

	if( x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT )
		return 0;

	return 1;

}

void fwarping( uchar* dst, uchar* src, matrix* p ){

	int i, j, x, y;
	double w;

	memset( dst, 0, sizeof(uchar)*WIDTH*HEIGHT );
	if( p->row != 3 || p->col != 3 )
		return;

	for( j = 0 ; j < HEIGHT ; j++ ){
		for( i = 0 ; i < WIDTH ; i++ ){
			w = p->var[2][0]*i + p->var[2][1]*j + p->var[2][2];

			x = (int)((p->var[0][0]*i + p->var[0][1]*j + p->var[0][2]) / w);
			y = (int)((p->var[1][0]*i + p->var[1][1]*j + p->var[1][2]) / w);
			
			if( !bound_check(x, y) )
				continue;

			dst[y*WIDTH+x] = src[j*WIDTH+i];
		}
	}

}

void fwarping_i( uchar* dst, uchar* src, matrix* p ){

	double px, py, w;
	int i, j, x, y;

	double* pixel;
	double* ratio;

	double  wx[2], wy[2];

	pixel = (double*)malloc(sizeof(double)*WIDTH*HEIGHT);
	ratio = (double*)malloc(sizeof(double)*WIDTH*HEIGHT);

	memset( pixel, 0, sizeof(double)*WIDTH*HEIGHT);
	memset( ratio, 0, sizeof(double)*WIDTH*HEIGHT);

	if( p->row != 3 || p->col != 3 )
		return;

	for( j = 0 ; j < HEIGHT ; j++ ){
		for( i = 0 ; i < WIDTH ; i++ ){

			w = p->var[2][0]*i + p->var[2][1]*j + p->var[2][2];

			px = (p->var[0][0]*i + p->var[0][1]*j + p->var[0][2]) / w;
			py = (p->var[1][0]*i + p->var[1][1]*j + p->var[1][2]) / w;

			x = (int)floor(px);
			y = (int)floor(py);

			wx[1] = px - x;
			wx[0] = 1.0 - wx[1];

			wy[1] = py - y;
			wy[0] = 1.0 - wy[1];

			if( bound_check( x, y ) ){
				pixel[y*WIDTH+x]     += src[j*WIDTH+i]*wx[0]*wy[0];
				ratio[y*WIDTH+x]     += wx[0]*wy[0];
			}

			if( bound_check( x+1, y ) ){
				pixel[y*WIDTH+(x+1)] += src[j*WIDTH+i]*wx[1]*wy[0];
				ratio[y*WIDTH+(x+1)] += wx[1]*wy[0];
			}

			if( bound_check( x, y+1 ) ){
				pixel[(y+1)*WIDTH+x] += src[j*WIDTH+i]*wx[0]*wy[1];
				ratio[(y+1)*WIDTH+x] += wx[0]*wy[1];
			}

			if( bound_check( x+1, y+1 ) ){
				pixel[(y+1)*WIDTH+(x+1)] += src[j*WIDTH+i]*wx[1]*wy[1];
				ratio[(y+1)*WIDTH+(x+1)] += wx[1]*wy[1];
			}

		}

	}	
	for( j = 0 ; j < HEIGHT ; j++ )
		for( i = 0 ; i < WIDTH ; i++ )
			dst[j*WIDTH+i] = (uchar)round(pixel[j*WIDTH+i]/ratio[j*WIDTH+i]);
		
	free(ratio);
	free(pixel);

}

void bwarping( uchar* dst, uchar* src, matrix* p ){

	double w;

	int i, j, x, y;

	matrix* inv = matrix_inv(p);

	for( j = 0 ; j < HEIGHT ; j++ ){
		for( i = 0 ; i < WIDTH ; i++ ){
			w = inv->var[2][0]*i + inv->var[2][1]*j + inv->var[2][2];

			x = (int)((inv->var[0][0]*i + inv->var[0][1]*j + inv->var[0][2])/w);
			y = (int)((inv->var[1][0]*i + inv->var[1][1]*j + inv->var[1][2])/w);
			
			if( !bound_check(x, y) )
				continue;

			dst[j*WIDTH+i] = src[y*WIDTH+x];
		}
	}
	matrix_free(inv);
}

void bwarping_i( uchar* dst, uchar* src, matrix* p ){

	double w, pixel, ratio, px, py;

	double wx[2];
	double wy[2];

	int i, j, x, y;

	matrix* inv = matrix_inv(p);

	for( j = 0 ; j < HEIGHT ; j++ ){
		for( i = 0 ; i < WIDTH ; i++ ){
			ratio = pixel = 0.0;
			w = inv->var[2][0]*i + inv->var[2][1]*j + inv->var[2][2];

			px = (inv->var[0][0]*i + inv->var[0][1]*j + inv->var[0][2]) / w;
			py = (inv->var[1][0]*i + inv->var[1][1]*j + inv->var[1][2]) / w;
			
			wx[1] = px - floor(px);
			wx[0] = 1.0 - wx[1];

			wy[1] = py - floor(py);
			wy[0] = 1.0 - wy[1];

			x = floor(px);
			y = floor(py);

			if( bound_check(x, y) ){
				pixel += wx[0]*wy[0]*src[y*WIDTH+x];
				ratio += wx[0]*wy[0];
			}
			if( bound_check(x+1, y) ){
				pixel += wx[1]*wy[0]*src[y*WIDTH+x+1];
				ratio += wx[1]*wy[0];
			}
			if( bound_check(x, y+1) ){
				pixel += wx[0]*wy[1]*src[(y+1)*WIDTH+x];
				ratio += wx[0]*wy[1];
			}
			if( bound_check(x+1, y+1) ){
				pixel += wx[1]*wy[1]*src[(y+1)*WIDTH+x+1];
				ratio += wx[1]*wy[1];
			}
			dst[j*WIDTH+i] = (uchar)floor( pixel/ratio + 0.5 );

		}

	}
	matrix_free(inv);

}

matrix* projection_matrix( double* x, double* y, double* _x, double* _y ){

	matrix* a;
	matrix* b;
	matrix* c;
	
	matrix* a_inv;
	matrix* projection;
	
	a = matrix_new( 8, 8 );
	b = matrix_new( 8, 1 );

	a->var[0][0] = x[0];
	a->var[0][1] = y[0];
	a->var[0][2] = 1.0;
	a->var[0][6] = -1 * _x[0] * x[0];
	a->var[0][7] = -1 * _x[0] * y[0];
	
	a->var[1][0] = x[1];
	a->var[1][1] = y[1];
	a->var[1][2] = 1.0;
	a->var[1][6] = -1 * _x[1] * x[1];
	a->var[1][7] = -1 * _x[1] * y[1];
	
	a->var[2][0] = x[2];
	a->var[2][1] = y[2];
	a->var[2][2] = 1.0;
	a->var[2][6] = -1 * _x[2] * x[2];
	a->var[2][7] = -1 * _x[2] * y[2];
	
	a->var[3][0] = x[3];
	a->var[3][1] = y[3];
	a->var[3][2] = 1.0;
	a->var[3][6] = -1 * _x[3] * x[3];
	a->var[3][7] = -1 * _x[3] * y[3];
	
	a->var[4][3] = x[0];
	a->var[4][4] = y[0];
	a->var[4][5] = 1.0;
	a->var[4][6] = -1 * x[0] * _y[0];
	a->var[4][7] = -1 * y[0] * _y[0];

	a->var[5][3] = x[1];
	a->var[5][4] = y[1];
	a->var[5][5] = 1.0;
	a->var[5][6] = -1 * x[1] * _y[1];
	a->var[5][7] = -1 * y[1] * _y[1];

	a->var[6][3] = x[2];
	a->var[6][4] = y[2];
	a->var[6][5] = 1.0;
	a->var[6][6] = -1 * x[2] * _y[2];
	a->var[6][7] = -1 * y[2] * _y[2];

	a->var[7][3] = x[3];
	a->var[7][4] = y[3];
	a->var[7][5] = 1.0;
	a->var[7][6] = -1 * x[3] * _y[3];
	a->var[7][7] = -1 * y[3] * _y[3];
	
	b->var[0][0] = _x[0];
	b->var[1][0] = _x[1];
	b->var[2][0] = _x[2];
	b->var[3][0] = _x[3];
	b->var[4][0] = _y[0];
	b->var[5][0] = _y[1];
	b->var[6][0] = _y[2];
	b->var[7][0] = _y[3];

	a_inv = matrix_inv(a);
	c = matrix_multiple( a_inv, b );

	projection = matrix_new( 3, 3 );
	projection->var[0][0] = c->var[0][0];
	projection->var[0][1] = c->var[1][0];
	projection->var[0][2] = c->var[2][0];
	
	projection->var[1][0] = c->var[3][0];
	projection->var[1][1] = c->var[4][0];
	projection->var[1][2] = c->var[5][0];
	
	projection->var[2][0] = c->var[6][0];
	projection->var[2][1] = c->var[7][0];
	projection->var[2][2] = 1.0;

	matrix_free(a);
	matrix_free(b);
	matrix_free(c);

	matrix_free(a_inv);

	return projection;
}

int main(int argc, char* argv[]){
	
	matrix *projection;

	FILE* in;
	FILE* out;

	uchar* src;
	uchar* dst;

	/*
	double x[] = {
		277.0,	536.0,	284.0,	541.0
	};
	double y[] = {
		61.0,	8.0,	387.0,	450.0
	};
	*/
	double x[] = {
		235.0,	355.0,	242.0,	362.0
	};
	double y[] = {
		51.0,	82.0,	443.0,	412.0
	};

	
	double _x[] = {
		240.0,	350.0,	240.0,	350.0
	};
	double _y[] = {
		82.0,	82.0,	412.0,	412.0
	};
	

	/*
	double _x[] = {
		20.0,	620.0,	20.0,	620.0
	};
	double _y[] = {
		20.0,	20.0,	460.0,	460.0
	};
	
	double _x[] = {
		220.0,	420.0,	220.0,	420.0
	};
	double _y[] = {
		140.0,	140.0,	340.0,	340.0
	};
	*/

	char filename[255];
	
	projection = projection_matrix( x, y, _x, _y );
	
	src = (uchar*)malloc(sizeof(uchar)*WIDTH*HEIGHT );
	dst = (uchar*)malloc(sizeof(uchar)*WIDTH*HEIGHT );
	
	in = fopen( INFILE, "rb" );
	fread( src, sizeof(uchar), WIDTH*HEIGHT, in );
	fclose(in);

	fwarping( dst, src, projection );

	sprintf( filename, OUTFILE, "fwarping" );
	out = fopen( filename, "wb" );
	fwrite( dst, sizeof(uchar), WIDTH*HEIGHT, out );
	fclose(out);

	fwarping_i( dst, src, projection );

	sprintf( filename, OUTFILE, "fwarping_i" );
	out = fopen( filename, "wb" );
	fwrite( dst, sizeof(uchar), WIDTH*HEIGHT, out );
	fclose(out);

	bwarping( dst, src, projection );

	sprintf( filename, OUTFILE, "bwarping" );
	out = fopen( filename, "wb" );
	fwrite( dst, sizeof(uchar), WIDTH*HEIGHT, out );
	fclose(out);

	bwarping_i( dst, src, projection );
	
	sprintf( filename, OUTFILE, "bwarping_i" );
	out = fopen( filename, "wb" );
	fwrite( dst, sizeof(uchar), WIDTH*HEIGHT, out );
	fclose(out);

	return 0;
}


