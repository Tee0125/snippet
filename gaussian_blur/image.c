#include <math.h>

#include "image.h"

uchar get_pixel( uchar* src, int x, int y, int width, int height  ){

#ifdef ZERO_PADDING
	if( x < 0 || x >= width || y < 0 || y >= height )
		return 0;
	else 
		return src[y*width+x];
#else
	if( x < 0 )
		x = 0;
	else if( x >= width )
		x = width-1;

	if( y < 0 )
		y = 0;
	else if( y >= height )
		y = height-1;

	return src[y*width+x];
#endif

}

uchar clip( double pixel ){

	pixel = floor( pixel+0.5 );
	
	if( pixel >= 255 )
		return 255;
	
	else if( pixel < 0 )
		return 0;
	
	else
		return (unsigned char)pixel;
	
}

void difference( uchar* dst, uchar* s1, uchar* s2, int width, int height ){

	int i, j;
	int diff;
	
	for( j = 0 ; j < height ; j++ ){
		for( i = 0 ; i < width ; i++ ){
			if( (diff = s1[j*width+i] - s2[j*width+i]) < 0 )
				diff = -diff;
			
			dst[j*width+i] = diff;
		}
	}
}
