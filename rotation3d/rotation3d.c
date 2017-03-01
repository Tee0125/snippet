#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.141592
#endif

#define INFILE  "lena512.raw"
#define OUTFILE "lena512_%s.raw"

#define MAX_FILENAME 256

#define IMG_WIDTH  512
#define IMG_HEIGHT 512

#define WIDTH  (IMG_WIDTH*3/2)
#define HEIGHT (IMG_HEIGHT*3/2)

typedef unsigned char uchar;

void load_identity( double axis[9] ){

	axis[0] = 1.0;	axis[1] = 0.0;	axis[2] = 0.0;
	axis[3] = 0.0;	axis[4] = 1.0;	axis[5] = 0.0;
	axis[6] = 0.0;	axis[7] = 0.0;	axis[8] = 1.0;

}

void rotate_xy( double dst_axis[9], double src_axis[9], double degree ){
	
	double radian = degree/180.0*M_PI;

	double c = cos(radian);
	double s = sin(radian);

	dst_axis[0] = src_axis[0] * c - src_axis[1] * s;
	dst_axis[1] = src_axis[1] * c + src_axis[0] * s;
	dst_axis[2] = src_axis[2];

	dst_axis[3] = src_axis[3] * c - src_axis[4] * s;
	dst_axis[4] = src_axis[4] * c + src_axis[3] * s;
	dst_axis[5] = src_axis[5];

	dst_axis[6] = src_axis[6] * c - src_axis[7] * s;
	dst_axis[7] = src_axis[7] * c + src_axis[6] * s;
	dst_axis[8] = src_axis[8];

}

void rotate_yz( double dst_axis[9], double src_axis[9], double degree ){
	
	double radian = degree/180.0*M_PI;

	double c = cos(radian);
	double s = sin(radian);

	dst_axis[0] = src_axis[0];
	dst_axis[1] = src_axis[1] * c - src_axis[2] * s;
	dst_axis[2] = src_axis[2] * c + src_axis[1] * s;

	dst_axis[3] = src_axis[3];
	dst_axis[4] = src_axis[4] * c - src_axis[5] * s;
	dst_axis[5] = src_axis[5] * c + src_axis[4] * s;

	dst_axis[6] = src_axis[6];
	dst_axis[7] = src_axis[7] * c - src_axis[8] * s;
	dst_axis[8] = src_axis[8] * c + src_axis[7] * s;

}

void rotate_zx( double dst_axis[9], double src_axis[9], double degree ){
	
	double radian = degree/180.0*M_PI;

	double c = cos(radian);
	double s = sin(radian);

	dst_axis[0] = src_axis[0] * c + src_axis[2] * s;
	dst_axis[1] = src_axis[1];
	dst_axis[2] = src_axis[2] * c - src_axis[0] * s;

	dst_axis[3] = src_axis[3] * c + src_axis[5] * s;
	dst_axis[4] = src_axis[4];
	dst_axis[5] = src_axis[5] * c - src_axis[3] * s;

	dst_axis[6] = src_axis[6] * c + src_axis[8] * s;
	dst_axis[7] = src_axis[7];
	dst_axis[8] = src_axis[8] * c - src_axis[6] * s;

}

int bound_check( int x, int y ){

	if( x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT )
		return 0;

	return 1;

}

void warping_i( uchar* dst, uchar* src, double axis[9] ){

	double x, y;

	int x_, y_;
	int i, j;

	double* tmp;
	double* tmp2;

	double  wx[2], wy[2];

	tmp  = (double*)malloc(sizeof(double)*WIDTH*HEIGHT);
	tmp2 = (double*)malloc(sizeof(double)*WIDTH*HEIGHT);

	memset( tmp, 0, sizeof(double)*WIDTH*HEIGHT);
	memset( tmp2, 0, sizeof(double)*WIDTH*HEIGHT);

	for( j = 0 ; j < HEIGHT ; j++ ){
		for( i = 0 ; i < WIDTH ; i++ ){

			x = axis[0]*(i-WIDTH/2) + axis[1]*(j-HEIGHT/2) + WIDTH/2;
			y = axis[3]*(i-WIDTH/2) + axis[4]*(j-HEIGHT/2) + HEIGHT/2;

			x_ = (int)floor(x);
			y_ = (int)floor(y);

			wx[1] = x - floor(x);
			wx[0] = 1.0 - wx[1];

			wy[1] = y - floor(y);
			wy[0] = 1.0 - wy[1];

			if( bound_check( x_, y_ ) ){
				tmp[y_*WIDTH+x_]     += src[j*WIDTH+i]*wx[0]*wy[0];
				tmp2[y_*WIDTH+x_]     += wx[0]*wy[0];
			}

			if( bound_check( x_+1, y_ ) ){
				tmp[y_*WIDTH+(x_+1)] += src[j*WIDTH+i]*wx[1]*wy[0];
				tmp2[y_*WIDTH+(x_+1)] += wx[1]*wy[0];
			}

			if( bound_check( x_, y_+1 ) ){
				tmp[(y_+1)*WIDTH+x_] += src[j*WIDTH+i]*wx[0]*wy[1];
				tmp2[(y_+1)*WIDTH+x_] += wx[0]*wy[1];
			}

			if( bound_check( x_+1, y_+1 ) ){
				tmp[(y_+1)*WIDTH+(x_+1)] += src[j*WIDTH+i]*wx[1]*wy[1];
				tmp2[(y_+1)*WIDTH+(x_+1)] += wx[1]*wy[1];
			}

		}
	}	
	for( j = 0 ; j < HEIGHT ; j++ ){
		for( i = 0 ; i < WIDTH ; i++ ){
			dst[j*WIDTH+i] = (uchar)floor(tmp[j*WIDTH+i]/tmp2[j*WIDTH+i] + 0.5);
		}
	}
	free(tmp);
	free(tmp2);

}
	
void warping( uchar* dst, uchar* src, double axis[9] ){

	double x, y;

	int x_, y_;
	int i, j;

	memset( dst, 0, sizeof(uchar)*WIDTH*HEIGHT );
	for( j = 0 ; j < HEIGHT ; j++ ){
		for( i = 0 ; i < WIDTH ; i++ ){

			x = axis[0]*(i-WIDTH/2) + axis[1]*(j-HEIGHT/2);
			y = axis[3]*(i-WIDTH/2) + axis[4]*(j-HEIGHT/2);

			x_ = (int)floor(x) + WIDTH/2;
			y_ = (int)floor(y) + HEIGHT/2;

			if( bound_check( x_, y_ ) )
				dst[y_*WIDTH+x_] = src[j*WIDTH+i];

		}

	}

}

uchar* trim_image( uchar* src, int* width, int* height ){

	int i, j, k, w, h;

	int min_x = WIDTH;
	int max_x = 0;

	int min_y = HEIGHT;
	int max_y = 0;

	uchar* dst;
	for( j = 0 ; j < HEIGHT ; j++ ){
		for( i = 0 ; i < WIDTH ; i++ ){
			if( src[j*WIDTH+i] != 0 ){
				if( j < min_y ) min_y = j;
				if( j > max_y ) max_y = j;

				if( i < min_x ) min_x = i;
				if( i > max_x ) max_x = i;
			}
		}
	}

	w = max_x - min_x;
	h = max_y - min_y;

	if( w < 0 || h < 0 )
		return NULL;

	*width  = w;
	*height = h;

	k = 0;
	dst = (uchar*)malloc(sizeof(uchar)*w*h);
	for( j = 0 ; j < h ; j++ )
		for( i = 0 ; i < w ; i++ )
			dst[k++] = src[(j+min_y)*WIDTH+i+min_x];

	return dst;
}

int main( int argc, char* argv[] ){

	char filename[MAX_FILENAME];

	double src_axis[9];
	double dst_axis[9];

	uchar* dst;
	uchar* src;

	uchar* im;

	FILE* in;
	FILE* out;

	int x, y, i, j, w, h;

	im  = (uchar*)malloc(sizeof(uchar)*IMG_WIDTH*IMG_HEIGHT);

	src = (uchar*)malloc(sizeof(uchar)*WIDTH*HEIGHT);
	dst = (uchar*)malloc(sizeof(uchar)*WIDTH*HEIGHT);

	in = fopen( INFILE, "rb" );
	fread( im, sizeof(uchar), IMG_WIDTH*IMG_HEIGHT, in );
	fclose(in);

	memset( src, 0, sizeof(uchar)*WIDTH*HEIGHT );
	for( j = 0 ; j < IMG_HEIGHT ; j++ ){
		for( i = 0 ; i < IMG_WIDTH ; i++ ){
			x = i + WIDTH/2  - IMG_WIDTH/2;
			y = j + HEIGHT/2 - IMG_HEIGHT/2;

			src[y*WIDTH+x] = im[j*IMG_WIDTH+i];
		}
	}
	free(im);

	load_identity( src_axis );
	rotate_xy( dst_axis, src_axis, 30.0 );

	warping( dst, src, dst_axis );
	im = trim_image( dst, &w, &h );

	snprintf( filename, MAX_FILENAME, OUTFILE, "fwarping" );
	out = fopen( filename, "wb" );
	fwrite(im, sizeof(uchar), w*h, out );
	fclose(out);

	fprintf( stderr, "%s: %dx%d\n", filename, w, h );
	free(im);


	warping_i( dst, src, dst_axis );
	im = trim_image( dst, &w, &h );

	snprintf( filename, MAX_FILENAME, OUTFILE, "fwarping_i" );
	out = fopen( filename, "wb" );
	fwrite(im, sizeof(uchar), w*h, out );
	fclose(out);

	fprintf( stderr, "%s: %dx%d\n", filename, w, h );
	free(im);




	load_identity( src_axis );
	rotate_xy( dst_axis, src_axis, 30.0 );

	warping_i( dst, src, dst_axis );
	im = trim_image( dst, &w, &h );

	snprintf( filename, MAX_FILENAME, OUTFILE, "z30" );
	out = fopen( filename, "wb" );
	fwrite(im, sizeof(uchar), w*h, out );
	fclose(out);

	fprintf( stderr, "%s: %dx%d\n", filename, w, h );
	free(im);


	load_identity( src_axis );
	rotate_zx( dst_axis, src_axis, 30.0 );

	warping_i( dst, src, dst_axis );
	im = trim_image( dst, &w, &h );

	snprintf( filename, MAX_FILENAME, OUTFILE, "y30" );
	out = fopen( filename, "wb" );
	fwrite(im, sizeof(uchar), w*h, out );
	fclose(out);

	fprintf( stderr, "%s: %dx%d\n", filename, w, h );
	free(im);


	load_identity( src_axis );
	rotate_zx( dst_axis, src_axis, 60.0 );
	rotate_yz( src_axis, dst_axis, 30.0 );

	warping_i( dst, src, src_axis );
	im = trim_image( dst, &w, &h );

	snprintf( filename, MAX_FILENAME, OUTFILE, "x30_y60" );
	out = fopen( filename, "wb" );
	fwrite(im, sizeof(uchar), w*h, out );
	fclose(out);

	fprintf( stderr, "%s: %dx%d\n", filename, w, h );
	free(im);


	load_identity( src_axis );
	rotate_yz( dst_axis, src_axis, 30.0 );
	rotate_zx( src_axis, dst_axis, 30.0 );
	rotate_xy( dst_axis, src_axis, 30.0 );

	warping_i( dst, src, dst_axis );
	im = trim_image( dst, &w, &h );

	snprintf( filename, MAX_FILENAME, OUTFILE, "x30_y30_z30" );
	out = fopen( filename, "wb" );
	fwrite(im, sizeof(uchar), w*h, out );
	fclose(out);

	fprintf( stderr, "%s: %dx%d\n", filename, w, h );
	free(im);


	return 0;
}

