#include <math.h>

#include <stdlib.h>

#include "image.h"
#include "gaussian.h"

static void build_mask( double* mask, int* n, int d ){
	
	int i;
	double var, div, sum = 0.0;
	
	div = pow(2*M_PI*d*d, 0.5); 
	
	if( d <= 0 ){
		mask[0] = 1.0;
		*n = 1;

		return;
	}
	
	for( i = 0 ; i < MASK_LEN ; i++ ){	
		var = pow(M_E, (i*i)/(-2.0*d*d)) / div;
		if( var < 0.001 )
			break;

		mask[i] = var;

		if( i == 0 )
			sum += var;
		else 
			sum += var + var;
	}
	*n = i;

	for( i = 0 ; i < *n ; i++ )
		mask[i] /= sum;
	
}

void gaussian_blur( uchar* dst, uchar* src, int sigma, int w, int h ){
	
	int i, j, k, n;

	uchar* tmp;
	double pixel;

	double mask[MASK_LEN];
	
	build_mask( mask, &n, sigma );
	
	tmp = (uchar*)malloc(sizeof(uchar)*w*h);	
	for( j = 0 ; j < h ; j++ ){
		for( i = 0 ; i < w ; i++ ){
			pixel = mask[0] * get_pixel( src, i, j, w, h );	
			for( k = 1 ; k < n ; k++ ){
				pixel += mask[k] * get_pixel( src, i-k, j, w, h ); 
				pixel += mask[k] * get_pixel( src, i+k, j, w, h ); 
			}
			tmp[j*w+i] = clip(pixel);
		}
	}
	
	for( j = 0 ; j < h ; j++ ){
		for( i = 0 ; i < w ; i++ ){
			pixel = mask[0] * get_pixel( tmp, i, j, w, h );	
			for( k = 1 ; k < n ; k++ ){
				pixel += mask[k] * get_pixel( tmp, i, j-k, w, h ); 
				pixel += mask[k] * get_pixel( tmp, i, j+k, w, h ); 
			}
			dst[j*w+i] = clip(pixel);
		}
	}

	free(tmp);

}

