#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>

#include "wavelet.h"

#define WIDTH 512
#define HEIGHT 512

#define CLIPPING

#define clip(x) (unsigned char)((x<=0)?0:(x>=255)?255:x)

static int get_pixel( int* im, int x, int y, int w, int h, int step ){

	if( x < 0 )
		x = abs(x);
	else if( x >= w )
		x = 2*w-x-2;

	if( y < 0 )
		y = abs(y);
	else if( y >= h )
		y = 2*h-y-2;

	return im[y*step+x];

}

// block type casting - from 8bit character type to 32bit integer type
static void cvt_c2i( int* dst, unsigned char* src, int w, int h ){

	int i, j, k;

	k = 0;
	for( j = 0 ; j < h ; j++ ){
		for( i = 0 ; i < w ; i++ ){
			dst[k] = src[k];
			k++;
		}
	}

}

// block type casting - from 32bit integer type to 8bit character type
static void cvt_i2c( unsigned char* dst, int* src, int w, int h ){

	int i, j, k;

	k = 0;
	for( j = 0 ; j < h ; j++ ){
		for( i = 0 ; i < w ; i++ ){
			dst[k] = clip(src[k]);
			k++;
		}
	}
	
}

// copy block
static void copy( int* dst, int* src, int w, int h ){

	int i, j, k;

	k = 0;
	for( j = 0 ; j < h ; j++ ){
		for( i = 0 ; i < w ; i++ ){
			dst[k] = src[k];
			k++;
		}
	}
	
}

// wavelet transform kernel - 5/3, ...
static const int coef[3][10] = {
	{  0,  1,  1,  0,  2,  0,  1,  1,  0,  4 },
	{ -1,  9,  9, -1, 16, -1,  9,  9, -1, 32 },
	{ -1,  9,  9, -1, 16, -1,  5,  5, -1, 16 }
};

static void wavelet_transform( int* dst, int* src, int w, int h, int type ){

	int i, j, k, ll, l, r, rr;
	int* t;
	int* tmp2;

	const int* c = coef[type];

	t = (int*)malloc( sizeof(int)*w*h );
	tmp2 = (int*)malloc( sizeof(int)*w*h );

	// vertical
	for( i = 0 ; i < w ; i++ ){
		for( j = 1 ; j < h ; j+=2 ){
			ll = get_pixel( src, i, j-3, w, h, WIDTH );
			l  = get_pixel( src, i, j-1, w, h, WIDTH );
			r  = get_pixel( src, i, j+1, w, h, WIDTH );
			rr = get_pixel( src, i, j+3, w, h, WIDTH );

			t[j*w+i] = src[j*WIDTH+i] - (c[0]*ll+c[1]*l+c[2]*r+c[3]*rr)/c[4];
		}

		for( j = 0 ; j < h ; j+=2 ){
			ll = get_pixel( t, i, j-3, w, h, w );
			l  = get_pixel( t, i, j-1, w, h, w );
			r  = get_pixel( t, i, j+1, w, h, w );
			rr = get_pixel( t, i, j+3, w, h, w );

			t[j*w+i] = src[j*WIDTH+i] + (c[5]*ll+c[6]*l+c[7]*r+c[8]*rr)/c[9];
		}
	} 

	// horizontal
	for( j = 0 ; j < h ; j++ ){
		for( i = 1 ; i < w ; i+=2 ){
			ll = get_pixel( t, i-3, j, w, h, w );
			l  = get_pixel( t, i-1, j, w, h, w );
			r  = get_pixel( t, i+1, j, w, h, w );
			rr = get_pixel( t, i+3, j, w, h, w );

			t[j*w+i] -= (c[0]*ll + c[1]*l + c[2]*r + c[3]*rr)/c[4];
		}

		for( i = 0 ; i < w ; i+=2 ){
			ll = get_pixel( t, i-3, j, w, h, w );
			l  = get_pixel( t, i-1, j, w, h, w );
			r  = get_pixel( t, i+1, j, w, h, w );
			rr = get_pixel( t, i+3, j, w, h, w );

			t[j*w+i] += (c[5]*ll + c[6]*l + c[7]*r + c[8]*rr)/c[9];
		}
	}
	
	// rearrangement
	for( j = 0 ; j < h ; j++ ){
		k = 0;
		for( i = 0 ; i < w ; i+=2 ){
			tmp2[j*w+k] = t[j*w+i];
			k++;
		}

		for( i = 1 ; i < w ; i+=2 ){
			tmp2[j*w+k] = t[j*w+i];
			k++;
		}
	}

	for( i = 0 ; i < w ; i++ ){
		k = 0;
		for( j = 0 ; j < h ; j+=2 ){
			dst[k*WIDTH+i] = tmp2[j*w+i];
			k++;
		}

		for( j = 1 ; j < h ; j+=2 ){
			dst[k*WIDTH+i] = tmp2[j*w+i];
			k++;
		}
	}
	free(t);	
	free(tmp2);

}

// inverse transform
static void wavelet_itransform( int* dst, int* src, int w, int h, int type ){

	int i, j, k, l, r, ll, rr;
	int* t;
	int* tmp2;

	const int* c = coef[type];
	
	t = (int*)malloc( sizeof(int)*w*h );
	tmp2 = (int*)malloc( sizeof(int)*w*h );

	// rearrangement
	for( i = 0 ; i < w ; i++ ){
		k = 0;
		for( j = 0 ; j < h ; j+=2 ){
			tmp2[j*w+i] = src[k*WIDTH+i];
			k++;
		}

		for( j = 1 ; j < h ; j+=2 ){
			tmp2[j*w+i] = src[k*WIDTH+i];
			k++;
		}
	}
	
	for( j = 0 ; j < h ; j++ ){
		k = 0;
		for( i = 0 ; i < w ; i+=2 ){
			t[j*w+i] = tmp2[j*w+k];
			k++;
		}

		for( i = 1 ; i < w ; i+=2 ){
			t[j*w+i] = tmp2[j*w+k];
			k++;
		}
	}

	// horizontal
	for( j = 0 ; j < h ; j++ ){
		for( i = 0 ; i < w ; i+=2 ){
			ll = get_pixel( t, i-3, j, w, h, w );
			l  = get_pixel( t, i-1, j, w, h, w );
			r  = get_pixel( t, i+1, j, w, h, w );
			rr = get_pixel( t, i+3, j, w, h, w );

			dst[j*WIDTH+i] = t[j*w+i] - (c[5]*ll+c[6]*l+c[7]*r+c[8]*rr)/c[9];
		}
		
		for( i = 1 ; i < w ; i+=2 ){
			ll = get_pixel( dst, i-3, j, w, h, WIDTH );
			l  = get_pixel( dst, i-1, j, w, h, WIDTH );
			r  = get_pixel( dst, i+1, j, w, h, WIDTH );
			rr = get_pixel( dst, i+3, j, w, h, WIDTH );

			dst[j*WIDTH+i] = t[j*w+i] + (c[0]*ll+c[1]*l+c[2]*r+c[3]*rr)/c[4];
		}
	}

	// vertical
	for( i = 0 ; i < w ; i++ ){
		for( j = 0 ; j < h ; j+=2 ){
			ll = get_pixel( dst, i, j-3, w, h, WIDTH );
			l  = get_pixel( dst, i, j-1, w, h, WIDTH );
			r  = get_pixel( dst, i, j+1, w, h, WIDTH );
			rr = get_pixel( dst, i, j+3, w, h, WIDTH );

			dst[j*WIDTH+i] -= (c[5]*ll + c[6]*l + c[7]*r + c[8]*rr)/c[9];
		}
		
		for( j = 1 ; j < h ; j+=2 ){
			ll = get_pixel( dst, i, j-3, w, h, WIDTH );
			l  = get_pixel( dst, i, j-1, w, h, WIDTH );
			r  = get_pixel( dst, i, j+1, w, h, WIDTH );
			rr = get_pixel( dst, i, j+3, w, h, WIDTH );

			dst[j*WIDTH+i] += (c[0]*ll + c[1]*l + c[2]*r + c[3]*rr)/c[4];
		}
	} 
	free(t);	
	free(tmp2);
	
}

//#define CLIPPING

void wavelet_encoder_jty( unsigned char* ifile, unsigned char* ofile, char filter, int label ){

	FILE *in, *out;
	int i;

	int w = WIDTH, h = HEIGHT, type;

	unsigned char *src;
	int* dst;

	src = (unsigned char*)malloc( sizeof(unsigned char)*w*h );
	dst = (int*)malloc( sizeof(int)*w*h );

	in  = fopen( ifile, "rb" );
	if( in == NULL){
		fprintf( stderr, "Error opening file %s\n", ifile );
		return;
	}
	fread( src, sizeof(unsigned char), WIDTH*HEIGHT, in );
	fclose(in);

	// convert to int~!
	cvt_c2i( dst, src, w, h );

	// classify filter type
	filter = tolower(filter);
	if( filter == 'a' )
		type = TWOTWO;
	else if( filter == 'b' )
		type = SWE;
	else if( filter == 'c' )
		type = CRF;
	else {
		fprintf( stderr, "Unknown filter\n" );
		return;
	}

	// transform
	for( i=0 ; i < label; i++, w/=2, h/=2 )
		wavelet_transform( dst, dst, w, h, type );

#ifdef CLIPPING
	{
		unsigned char* tmp = (unsigned char*)malloc(sizeof(unsigned char)*WIDTH*HEIGHT);
		cvt_i2c( tmp, dst, WIDTH, HEIGHT );
		out = fopen( ofile, "wb" );
		fwrite( tmp, sizeof(unsigned char), WIDTH*HEIGHT, out );
		fclose(out);
		free(tmp);
	}
#else
	out = fopen( ofile, "wb" );
	if( out == NULL){
		fprintf( stderr, "Error opening file %s\n", ofile );
		return;
	}
	fwrite( dst, sizeof(int), WIDTH*HEIGHT, out );
	fclose(out);
#endif

	free(dst);
	free(src);
}


void wavelet_decoder_jty( unsigned char* ifile, unsigned char* ofile, char filter, int label ){

	FILE *in, *out;
	int i;

	int w = WIDTH, h = HEIGHT, type;

	unsigned char *dst;
	int* src;

	dst = (unsigned char*)malloc( sizeof(unsigned char)*WIDTH*HEIGHT );
	src = (int*)malloc( sizeof(int)*WIDTH*HEIGHT );

	in  = fopen( ifile, "rb" );
	if( in == NULL){
		fprintf( stderr, "Error opening file %s\n", ifile );
		return;
	}
	fread( src, sizeof(int), WIDTH*HEIGHT, in );
	fclose(in);

	// classify filter type
	filter = tolower(filter);
	if( filter == 'a' )
		type = TWOTWO;
	else if( filter == 'b' )
		type = SWE;
	else if( filter == 'c' )
		type = CRF;
	else {
		fprintf( stderr, "Unknown filter\n" );
		return;
	}

	// inverse transform
	for( i=0, w>>=(label-1), h>>=(label-1) ; i < label; i++, w<<=1, h<<=1 )
		wavelet_itransform( src, src, w, h, type );

	// convert to unsigned character from int
	cvt_i2c( dst, src, WIDTH, HEIGHT );

	out = fopen( ofile, "wb" );
	if( out == NULL){
		fprintf( stderr, "Error opening file %s\n", ofile );
		return;
	}
	fwrite( dst, sizeof(unsigned char), WIDTH*HEIGHT, out );
	fclose(out);

	free(dst);
	free(src);
}
