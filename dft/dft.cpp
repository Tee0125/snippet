#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "dft_table.h"

#define WIDTH 512
#define HEIGHT 512

#define PWIDTH  (WIDTH+16)
#define PHEIGHT (HEIGHT+16)

// 1D filter coefficient - type 1
double coef1[] = {
	0.010333,
	0.207561,
	0.564210,
	0.207561,
	0.010333
};

// 1D filter coefficient - type 2
double coef2[] = {
	0.000134,
	0.004432,
	0.053991,
	0.241971,
	0.398943,
	0.241971,
	0.053991,
	0.004432,
	0.000134
};

// 1D filter coefficient - type 3
double coef3[] = {
	0.001573,
	0.016220,
	0.085877,
	0.233437,
	0.325787,
	0.233437,
	0.085877,
	0.016220,
	0.001573
};

complex complex_add( complex src1, complex src2 );
complex complex_mul( complex src1, complex src2 );

void build_complex( complex* dst, unsigned char* src );

void dft( complex* dst, complex* src );
void idft( complex* dst, complex* src );

void load_image( complex* dst, const char* filename );
void save_image( complex* src, const char* filename );
void save_frequency( complex* src, const char* filename );

void load_filter_1( complex* dst );
void load_filter_2( complex* dst );
void load_filter_3( complex* dst );

int main(int argc, char* argv[])
{

	complex	*src, *dst, *coef, *fcoef, *result, *fresult;
	int i, j;

	coef	= new complex[PWIDTH*PHEIGHT];
	fcoef	= new complex[PWIDTH*PHEIGHT];

	src		= new complex[PWIDTH*PHEIGHT];
	dst		= new complex[PWIDTH*PHEIGHT];

	fresult	= new complex[PWIDTH*PHEIGHT];
	result	= new complex[PWIDTH*PHEIGHT];

	load_image( src, "lena512.raw" );
	fprintf( stderr, "Image is loaded\n" );

	// transform the image
	dft( dst, src );
	fprintf( stderr, "Image is transformed.\n\n" );

	save_frequency( dst, "ft_lena512.raw" );

	// transform the filter
	load_filter_1( coef );
	fprintf( stderr, "Filter (for sigma=0.5) is loaded\n" );

	dft( fcoef, coef );
	save_frequency( fcoef, "ft_filter1.raw" );
	fprintf( stderr, "Filter1 is transformed.\n" );

	// multiply filter * image;
	for( j = 0 ; j < PHEIGHT ; j++ )
		for( i = 0 ; i <  PWIDTH ; i++ )
			fresult[j*PHEIGHT+i] = complex_mul( dst[j*PHEIGHT+i], fcoef[j*PHEIGHT+i] );

	// inverse transform the result;
	idft( result, fresult );
	save_image( result, "filtered_lena512_0.5.raw" );
	fprintf( stderr, "Result is inverse transformed\n\n" );



	// transform the filter
	load_filter_2( coef );
	fprintf( stderr, "Filter (for sigma=1.0) is loaded\n" );

	dft( fcoef, coef );
	save_frequency( fcoef, "ft_filter2.raw" );
	fprintf( stderr, "Filter2 is transformed.\n" );

	// multiply filter * image;
	for( j = 0 ; j < PHEIGHT ; j++ )
		for( i = 0 ; i <  PWIDTH ; i++ )
			fresult[j*PHEIGHT+i] = complex_mul( dst[j*PHEIGHT+i], fcoef[j*PHEIGHT+i] );

	// inverse transform the result;
	idft( result, fresult );
	save_image( result, "filtered_lena512_1.0.raw" );
	fprintf( stderr, "Result is inverse transformed\n\n" );



	// transform the filter
	load_filter_3( coef );
	fprintf( stderr, "Filter (for sigma=1.5) is loaded\n" );

	dft( fcoef, coef );
	save_frequency( fcoef, "ft_filter3.raw" );
	fprintf( stderr, "Filter1 is transformed.\n" );

	// multiply filter * image;
	for( j = 0 ; j < PHEIGHT ; j++ )
		for( i = 0 ; i <  PWIDTH ; i++ )
			fresult[j*PHEIGHT+i] = complex_mul( dst[j*PHEIGHT+i], fcoef[j*PHEIGHT+i] );

	// inverse transform the result;
	idft( result, fresult );
	save_image( result, "filtered_lena512_1.5.raw" );
	fprintf( stderr, "Result is inverse transformed\n\n" );


	delete src;
	delete dst;

	delete coef;
	delete fcoef;

	return 0;
}

void load_image( complex* dst, const char* filename ){

	FILE* in;

	unsigned char* pel;
	pel = new unsigned char[WIDTH*HEIGHT];

	in = fopen( filename, "rb" );
	fread( pel, sizeof(unsigned char), WIDTH*HEIGHT, in );
	fclose(in);

	build_complex( dst, pel );
	delete pel;

}

void save_image( complex* src, const char* filename ){

	FILE* out;

	int i, j;
	double t;

	unsigned char* pel;

	pel = new unsigned char[WIDTH*HEIGHT];

	for( j = 8 ; j < PHEIGHT-8 ; j++ ){
		for( i = 8 ; i < PWIDTH-8 ; i++ ){
			t = floor( src[j*PWIDTH+i].re + 0.5 );
			pel[(j-8)*WIDTH+i-8] = (t>=255.0)?255:(t<=0.0f)?0:(unsigned char)t;
		}
	}

	out = fopen( filename, "wb" );
	fwrite( pel, sizeof(unsigned char), WIDTH*HEIGHT, out );
	fclose(out);

	delete pel;

}

void save_frequency( complex* src, const char* filename ){

	FILE* out;

	int i, j;
	double t;

	unsigned char* pel;
	double max, min;

	pel = new unsigned char[PWIDTH*PHEIGHT];

	max = floor(log(sqrt(src[0].re*src[0].re+src[0].im*src[0].im))+0.5);
	min = floor(log(sqrt(src[0].re*src[0].re+src[0].im*src[0].im))+0.5);
	
	for( j = 0 ; j < PHEIGHT ; j++ ){
		for( i = 0 ; i < PWIDTH ; i++ ){
			t = floor(log(sqrt(src[j*PWIDTH+i].re*src[j*PWIDTH+i].re+src[j*PWIDTH+i].im*src[j*PWIDTH+i].im))+0.5);
			if( max < t )
				max = t;
			if( min > t )
				min = t;
		}
	}

	for( j = 0 ; j < PHEIGHT ; j++ ){
		for( i = 0 ; i < PWIDTH ; i++ ){
			
			t = floor(log(sqrt(src[j*PWIDTH+i].re*src[j*PWIDTH+i].re+src[j*PWIDTH+i].im*src[j*PWIDTH+i].im))+0.5);
			t = 255/(max-min)*(t-min);

			//t = floor( src[j*PWIDTH+i].re + 0.5 );
			pel[j*PWIDTH+i] = (t>=255.0)?255:(t<=0.0f)?0:(unsigned char)t;
		}
	}

	out = fopen( filename, "wb" );
	fwrite( pel, sizeof(unsigned char), PWIDTH*PHEIGHT, out );
	fclose(out);

	delete pel;

}

void load_filter_1( complex* dst ){

	int i, j, x, y;

	memset( dst, 0, PWIDTH*PHEIGHT*sizeof(complex) );
	for( j = 0 ; j < 5 ; j++ ){
		for( i = 0 ; i < 5; i++ ){
			x = ((i-2)+PWIDTH)%PWIDTH;
			y = ((j-2)+PHEIGHT)%PHEIGHT;

			dst[y*PWIDTH+x].re = coef1[j]*coef1[i];
		}
	}
}

void load_filter_2( complex* dst ){

	int i, j, x, y;

	memset( dst, 0, PWIDTH*PHEIGHT*sizeof(complex) );
	for( j = 0 ; j < 9 ; j++ ){
		for( i = 0 ; i < 9; i++ ){
			x = ((i-4)+PWIDTH)%PWIDTH;
			y = ((j-4)+PHEIGHT)%PHEIGHT;

			dst[y*PWIDTH+x].re = coef2[j]*coef2[i];
		}
	}
}

void load_filter_3( complex* dst ){

	int i, j, x, y;

	memset( dst, 0, PWIDTH*PHEIGHT*sizeof(complex) );
	for( j = 0 ; j < 9 ; j++ ){
		for( i = 0 ; i < 9; i++ ){
			x = ((i-4)+PWIDTH)%PWIDTH;
			y = ((j-4)+PHEIGHT)%PHEIGHT;

			dst[y*PWIDTH+x].re = coef3[j]*coef3[i];
		}
	}
}

complex complex_add( complex src1, complex src2 ){

	complex ret;

	ret.re = src1.re + src2.re;
	ret.im = src1.im + src2.im;

	return ret;
}
complex complex_mul( complex src1, complex src2 ){

	complex ret;

	ret.re = src1.re * src2.re - src1.im * src2.im;
	ret.im = src1.re * src2.im + src1.im * src2.re;

	return ret;
}

void dft( complex* dst, complex* src ){

	int i, j, k;
	complex* tmp;
	
	tmp = new complex[PWIDTH*PHEIGHT];

	// forward transform
	memset( tmp, 0, sizeof(complex)*PWIDTH*PHEIGHT );
	memset( dst, 0, sizeof(complex)*PWIDTH*PHEIGHT );

	for( j = 0 ; j < PHEIGHT ; j++ )
		for( i = 0 ; i < PWIDTH ; i++ )
			for( k = 0 ; k < PWIDTH ; k++ )
				tmp[j*PWIDTH+i] = complex_add( tmp[j*PWIDTH+i], complex_mul( t_dft[j*PWIDTH+k], src[k*PWIDTH+i] ) );

	for( j = 0 ; j < PHEIGHT ; j++ )
		for( i = 0 ; i < PWIDTH ; i++ )
			for( k = 0 ; k < PHEIGHT ; k++ )
				dst[j*PWIDTH+i] = complex_add( dst[j*PWIDTH+i], complex_mul( t_dft[k*PWIDTH+i], tmp[j*PWIDTH+k] ) );
	
	delete tmp;
}

void idft( complex* dst, complex* src ){

	int i, j, k;
	complex* tmp;
	
	tmp = new complex[PWIDTH*PHEIGHT];

	memset( tmp, 0, sizeof(complex)*PWIDTH*PHEIGHT );
	memset( dst, 0, sizeof(complex)*PWIDTH*PHEIGHT );

	for( j = 0 ; j < PHEIGHT ; j++ )
		for( i = 0 ; i < PWIDTH ; i++ )
			for( k = 0 ; k < PWIDTH ; k++ )
				tmp[j*PWIDTH+i] = complex_add( tmp[j*PWIDTH+i], complex_mul( t_idft[j*PWIDTH+k], src[k*PWIDTH+i] ) );

	for( j = 0 ; j < PHEIGHT ; j++ )
		for( i = 0 ; i < PWIDTH ; i++ )
			for( k = 0 ; k < PHEIGHT ; k++ )
				dst[j*PWIDTH+i] = complex_add( dst[j*PWIDTH+i], complex_mul( t_idft[k*PWIDTH+i], tmp[j*PWIDTH+k] ) );
			
	delete tmp;

}

void build_complex( complex* dst, unsigned char* src ){

	int i, j;

	// fill zero
	memset( dst, 0, sizeof(complex)*PWIDTH*PHEIGHT );

	// boundary copy
	for( j = 0 ; j < 8 ; j++ ){
		for( i = 6 ; i < 8 ; i++ )
			dst[j*PWIDTH+i].re = (float)src[0];

		for( i = PWIDTH-8 ; i < PWIDTH ; i++ )
			dst[j*PWIDTH+i].re = (float)src[WIDTH-1];

		for( i = 8 ; i < PWIDTH-8 ; i++ )
			dst[j*PWIDTH+i].re = (float)src[i-8];
	}

	for( j = PHEIGHT-8 ; j < PHEIGHT ; j++ ){
		for( i = 0 ; i < 8 ; i++ )
			dst[j*PWIDTH+i].re = (float)src[(HEIGHT-1)*WIDTH];

		for( i = PWIDTH-8 ; i < PWIDTH ; i++ )
			dst[j*PWIDTH+i].re = (float)src[HEIGHT*WIDTH-1];

		for( i = 8 ; i < PWIDTH-8 ; i++ )
			dst[j*PWIDTH+i].re = (float)src[(HEIGHT-1)*WIDTH+i-8];
	}

	for( j = 8 ; j < PHEIGHT-8 ; j++ ){
		for( i = 0 ; i < 8 ; i++ )
			dst[j*PWIDTH+i].re = (float)src[(j-8)*WIDTH];

		for( i = 8 ; i < PWIDTH-8 ; i++ )
			dst[j*PWIDTH+i].re = (float)src[(j-8)*WIDTH+i-8];

		for( i = PWIDTH-8 ; i < PWIDTH ; i++ )
			dst[j*PWIDTH+i].re = (float)src[(j-7)*WIDTH-1];
	}

}
