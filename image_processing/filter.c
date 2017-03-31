/*
 *  filter.c
 *  
 *
 *  Created by ÅÂ¿µ Á¤ on 06. 11. 20.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "filter.h"

static int __laplace_5[3][3] = {
	 2,	-1,	 2,
	-1,	-4,	-1,
	 2,	-1,	 2
};

static int __laplace_6[3][3] = {
	 0,	 1,	 0,
	 1,	-4,	 1,
	 0,	 1,	 0
};

static int __laplace_4[3][3] = {
	 1,	 1,	 1,
	 1,	-8,	 1,
	 1,	 1,	 1
};


static int __blur[3][3] = {
	 1,	 2,	 1,
	 2,	 4,	 2,
	 1,	 2,	 1
};

static int __smooth[3][3] = {
	 1,	 1,	 1,
	 1,	 1,	 1,
	 1,	 1,	 1
};

#if LOG_SIZE == 3
static int __log_window[3][3] = {
	 1,	 2,	 1,
	 2,	-4,	 2,
	 1,	 2,	 1
};
#elif LOG_SIZE == 5
static int __log_window[5][5] = {
	 0,	 0,	-1,	 0,	 0,
	 0,	-1,	-2,	-1,	 0,
	-1,	-2,	16,	-2,	-1,
	 0,	-1,	-2,	-1,	 0,
	 0,	 0,	-1,	 0,	 0
};
#else
#undef LOG_SIZE
#define LOG_SIZE 7

static int __log_window[7][7] = {
	 0,	 0,	-1,	-1,	-1,	 0,	 0,
	 0,	-2,	-3,	-3,	-3,	-2,	 0,
	-1,	-3,	 5,	 5,	 5,	-3,	-1,
	-1,	-3,	 5,	16,	 5,	-3,	-1,
	-1,	-3,	 5,	 5,	 5,	-3,	-1,
	 0,	-2,	-3,	-3,	-3,	-2,	 0,
	 0,	 0,	-1,	-1,	-1,	 0,	 0
};
#endif

void filter_init(){

	int i;

	_laplace = (int**)malloc(sizeof(int*)*3);
	_blur = (int**)malloc(sizeof(int*)*3);
	_smooth = (int**)malloc(sizeof(int*)*3);
	_log = (int**)malloc(sizeof(int*)*LOG_SIZE);

	for( i = 0 ; i < 3 ; i++ ){
		_laplace[i]	= __laplace_4[i];
		_blur[i]	= __blur[i];
		_smooth[i]	= __smooth[i];
		_log[i]		= __log_window[i];
	}
	for( ; i < LOG_SIZE ; i++ )
		_log[i]		= __log_window[i];

}

void gray_scale( image* dst, image* src ){

	int i, j;
	int r, g, b;

	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			image_get( src, i, j, &r, &g, &b );

			r = (30 * r + 59 * g + 11 * b) / 100;
			image_set( dst, i, j, r, r, r );
		}
	}
}

void _convolution( image* dst, image* src, int** w, int wsize, int div ){


	int i, j, k, l;
	int h_wsize;

	int r, g, b;
	int nr, ng, nb;

	h_wsize = wsize/2;
	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			
			r = g = b = 0;
			for( k = 0 ; k < wsize ; k++ ){
				for( l = 0 ; l < wsize ; l++ ){

					image_get(src, i-h_wsize+l, j-h_wsize+k, &nr, &ng, &nb );
					
					r += nr * w[k][l];
					g += ng * w[k][l];
					b += nb * w[k][l];

				}
			}			
			image_set( dst, i, j, r/div, g/div, b/div);
			
		}
	}
}

void gray_level_transform( image* dst, image* src, int x, int y ){

	int i, j;
	double d;
	
	int r, g, b;
	
	d = (double)y / (double)x;
	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			image_get( src, i, j, &r, &g, &b );
			image_set( dst, i, j, r*d, g*d, b*d );
		}
	}
	
}

void gray_level_transform2( image* dst, image* src ){

	int i, j;
	int r, g, b;
	
	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			image_get( src, i, j, &r, &g, &b );

			if( r < 100 )
				r *= 50.0 / 100.0;
			else if( r < 200 )
				r *= (double)(200-50) / (double)(200-100);

			if( g < 100 )
				g *= 50.0 / 100.0;
			else if( g < 200 )
				g *= (double)(200-50) / (double)(200-100);

			if( b < 100 )
				b *= 50.0 / 100.0;
			else if( b < 200 )
				b *= (double)(200-50) / (double)(200-100);

			image_set( dst, i, j, r, g, b );
		}
	}
	
}


static int image_pixel_comp( const void* l, const void* r ){
	int* lv = (int*)l;
	int* rv = (int*)r;
	return *lv - *rv;
} 

void filter_median( image* dst, image* src ){

	int i, j, k, l, m;
	int r[9], g[9], b[9];

	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			
			// get median value
			for( k = 0, m = 0 ; k < 3 ; k++ )
				for( l = 0 ; l < 3 ; l++, m++ )
					image_get(src, i-1+l, j-1+k, &r[m], &g[m], &b[m]);
					
			qsort( r, 9, sizeof(int), image_pixel_comp );
			qsort( g, 9, sizeof(int), image_pixel_comp );
			qsort( b, 9, sizeof(int), image_pixel_comp );
			
			image_set( dst, i, j, r[4], g[4], b[4] );
			
		}
		
	}
	
}

void filter_gradient( image* dst, image* src ){

	int i, j, k, l;
	int r, g, b;
	
	int dx[3], dy[3]; // d/dx & d/dy
	
	// sobel masks
	int sobel_x[3][3] = {
		-1,	 0,	 1,
		-2,	 0,	 2,
		-1,	 0,	 1
	};
	
	int sobel_y[3][3] = {
		 1,	 2,	 1,
		 0,	 0,	 0,
		-1,	-2,	-1
	};
	
	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
	
			dx[0] = dx[1] = dx[2] = 0;
			dy[0] = dy[1] = dy[2] = 0;
			for( k = 0 ; k < 3 ; k++ ){
				for( l = 0 ; l < 3 ; l++ ){
					image_get(src, i-1+l, j-1+k, &r, &g, &b);
					
					dx[0] += sobel_x[k][l] * r;
					dx[1] += sobel_x[k][l] * g;
					dx[2] += sobel_x[k][l] * b;
					
					dy[0] += sobel_y[k][l] * r;
					dy[1] += sobel_y[k][l] * g;
					dy[2] += sobel_y[k][l] * b;
				}
			}
			dx[0] = abs(dx[0]);
			dx[1] = abs(dx[1]);
			dx[2] = abs(dx[2]);

			dy[0] = abs(dy[0]);
			dy[1] = abs(dy[1]);
			dy[2] = abs(dy[2]);
			
			// infinite norm
			r = (dx[0] > dy[0]) ? dx[0]:dy[0];
			g = (dx[1] > dy[1]) ? dx[1]:dy[1];
			b = (dx[2] > dy[2]) ? dx[2]:dy[2];
			
			image_set( dst, i, j, r, g, b );
			
		}
		
	}
	
}

void filter_homogeneity( image* dst, image* src ){

	int i, j, k, l, m;
	int r[9], g[9], b[9];

	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			
			image_get(src, i, j, &r[4], &g[4], &b[4] );
			
			// get homogeneity var
			for( k = 0, m = 0 ; k < 3 ; k++ ){
				for( l = 0 ; l < 3 ; l++, m++ ){
					if( m != 4 ){ // l = 1 && k = 1
						image_get(src, i-1+l, j-1+k, &r[m], &g[m], &b[m]);
						r[m] = abs( r[m] - r[4] );
						g[m] = abs( g[m] - g[4] );
						b[m] = abs( b[m] - b[4] );
					}
				}
			}
			r[4] = g[4] = b[4] = 0;
			
			qsort( r, 9, sizeof(int), image_pixel_comp );
			qsort( g, 9, sizeof(int), image_pixel_comp );
			qsort( b, 9, sizeof(int), image_pixel_comp );
			
			image_set( dst, i, j, r[8], g[8], b[8] );
			
		}
		
	}
	
}

void filter_difference( image* dst, image* src ){

	int i, j, k, l, m;
	int r[2], g[2], b[2];
	int s_r[2], s_g[2], s_b[2];
	
	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			
			image_get(src, i-1, j, &s_r[0], &s_g[0], &s_b[0] );
			image_get(src, i+1, j, &s_r[1], &s_g[1], &s_b[1] );
			
			r[0] = abs(s_r[0] - s_r[1]);
			g[0] = abs(s_g[0] - s_g[1]);
			b[0] = abs(s_b[0] - s_b[1]);
			
			image_get(src, i, j-1, &s_r[0], &s_g[0], &s_b[0] );
			image_get(src, i, j+1, &s_r[1], &s_g[1], &s_b[1] );
			
			r[1] = abs(s_r[0] - s_r[1]);
			g[1] = abs(s_g[0] - s_g[1]);
			b[1] = abs(s_b[0] - s_b[1]);
			
			if( r[1] > r[0] ) r[0] = r[1];
			if( g[1] > g[0] ) g[0] = g[1];
			if( b[1] > b[0] ) b[0] = b[1];

			image_get(src, i-1, j-1, &s_r[0], &s_g[0], &s_b[0] );
			image_get(src, i+1, j+1, &s_r[1], &s_g[1], &s_b[1] );
			
			if( r[1] > r[0] ) r[0] = r[1];
			if( g[1] > g[0] ) g[0] = g[1];
			if( b[1] > b[0] ) b[0] = b[1];
			
			image_get(src, i-1, j+1, &s_r[0], &s_g[0], &s_b[0] );
			image_get(src, i+1, j-1, &s_r[1], &s_g[1], &s_b[1] );
			
			r[1] = abs(s_r[0] - s_r[1]);
			g[1] = abs(s_g[0] - s_g[1]);
			b[1] = abs(s_b[0] - s_b[1]);
			
			if( r[1] > r[0] ) r[0] = r[1];
			if( g[1] > g[0] ) g[0] = g[1];
			if( b[1] > b[0] ) b[0] = b[1];
			
			image_set( dst, i, j, r[0], g[0], b[0] );
			
		}
		
	}
	
}


void init_histogram( int* hist ){

	int i;
	for( i = 0 ; i < 256 ;  i++ )
		hist[i] = 0;
		
}

void get_histogram( int* hist, image* src ){

	int i, j;
	int r, g, b;
	
	int pel;
	
	// initialize the histogram;
	for( i = 0 ; i < 256 ; i++ )
		hist[i] = 0;
		
	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){

			// gray scale = 0.30 * r + 0.59 * g + 0.11 * b
			image_get( src, i, j, &r, &g, &b );
			pel  = (30 * r + 59 * g + 11 * b) / 100;	
			
			if( pel > 255 )	pel = 255;					
			else if( pel < 0 ) pel = 0;
					
			hist[pel]++;

		}
		
	}
	
}

void print_histogram( char* filename, image* src ){

	FILE* out;

	int histogram[256];
	int i, j;

	int r, g, b;
	int pel;
	
	out = fopen( filename, "w" );

	//fprintf( out, "%s\t%s\n", "i", "H(i)" );
	// initialize the histogram;
	for( i = 0 ; i < 256 ; i++ )
		histogram[i] = 0;
		
	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){

			// gray scale = 0.30 * r + 0.59 * g + 0.11 * b
			image_get( src, i, j, &r, &g, &b );

			pel  = (30 * r + 59 * g + 11 * b) / 100;
			histogram[pel]++;

		}
		
	}
	for( i = 0 ; i < 256 ; i++ )
		fprintf( out, "%d\t%d\n", i, histogram[i] );
	
	fclose(out);
}

void histogram_stretch( image* dst, image* src, int l, int h ){

	int hist[256];
	
	int i, j, org_l, org_h;
	double _a, _b;
	
	int r, g, b;

	get_histogram( hist, src );

	for( org_l = 0 ; org_l < 255 && hist[org_l] == 0 ; org_l++ );
	for( org_h = 255 ; org_h >= 0 && hist[org_h] == 0 ; org_h-- );
	
	_a = (double)(h-l) / (double)(org_h-org_l);
	_b = (double)(org_h*l - org_l*h) / (double)(org_h-org_l);

	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			image_get( src, i, j, &r, &g, &b );

			r = _a * r + _b;
			g = _a * g + _b;
			b = _a * b + _b;

			image_set( dst, i, j, r, g, b );
		}
	}
}

void histogram_equalize( image* dst, image* src ){

	int hist[256];
	int nhist[256];

	int l, h, n, w;
	int i, j;

	int r, g, b;

	get_histogram( hist, src );

	for( l = 0 ; l < 255 && hist[l] == 0 ; l++ );
	for( h = 255 ; h >= 0 && hist[h] == 0 ; h-- );
	
	/* g_max = 255 && g_min  = 0 */
	w = 255;
	n = image_x(src) * image_y(src);

	nhist[0] = hist[0];
	for( i = 1 ; i < 256 ; i++ )
		nhist[i] = nhist[i-1] + hist[i]; 

	for( i = 0 ; i < 256 ; i++ )
		nhist[i] = lround( (double)nhist[i] * (double)w / (double)n );

	for( j = 0 ; j < image_y(src) ; j++ ){
		for( i = 0 ; i < image_x(src) ; i++ ){
			image_get( src, i, j, &r, &g, &b );
			image_set( dst, i, j, nhist[r], nhist[g], nhist[b] );
		}
	}

}


