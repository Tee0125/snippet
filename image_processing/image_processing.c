#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "image.h"
#include "filter.h"
#include "html.h"

int main( int argc, char** argv ){

	image* src;
	image* dst;

	filter_init();
	html_init();

	src = image_open( "sample.jpg" );
	dst = image_create( image_x(src), image_y(src) );
	html_print( "Sample Image", "sample.jpg" );	
	print_histogram( "r_original.txt", src);

	if( src == NULL )
		return -1;

	gray_scale( dst, src );
	image_write( "r_gray.jpg", dst );
	html_print( "gray scaled", "r_gray.jpg" );	

	gray_level_transform( dst, src, 255, 100 );
	image_write( "r_gray1_a.jpg", dst );
	html_print( "gray level transformation #1", "r_gray1_a.jpg" );	

	gray_level_transform( dst, src, 100, 255 );
	image_write( "r_gray1_b.jpg", dst );
	html_print( "gray level transformation #2", "r_gray1_b.jpg" );	

	gray_level_transform2( dst, src );
	image_write( "r_gray2.jpg", dst );
	html_print( "gray level transformation #2", "r_gray2.jpg" );	

	histogram_stretch( dst, src, 0, 255 );
	image_write( "r_hstretch.jpg", dst );
	html_print( "histogram stretching", "r_hstretch.jpg" );	
	print_histogram( "r_hstretch.txt", dst);

	histogram_equalize( dst, src );
	image_write( "r_equalize.jpg", dst );
	html_print( "histogram equalize", "r_equalize.jpg" );	
	print_histogram( "r_hequalize.txt", dst);

	filter_smooth( dst, src );
	image_write( "r_smooth.jpg", dst );
	html_print( "filter smooth", "r_smooth.jpg" );	

	filter_median( dst, src );
	image_write( "r_median.jpg", dst );
	html_print( "filter median", "r_median.jpg" );	

	filter_blur( dst, src );
	image_write( "r_blur.jpg", dst );
	html_print( "filter blur", "r_blur.jpg" );	

	filter_gradient( dst, src );
	image_write( "r_gradient.jpg", dst );
	html_print( "filter gradient (sobel)", "r_gradient.jpg" );	

	filter_laplace( dst, src );
	image_write( "r_laplace.jpg", dst );
	html_print( "filter laplace", "r_laplace.jpg" );	

	filter_homogeneity( dst, src );
	image_write( "r_homo.jpg", dst );
	html_print( "filter homogeneity", "r_homo.jpg" );	

	filter_difference( dst, src );
	image_write( "r_diff.jpg", dst );
	html_print( "filter difference", "r_diff.jpg" );	

	filter_log( dst, src );
	image_write( "r_log.jpg", dst );
	html_print( "filter log (laplace of gaussian: window_size=7)", "r_log.jpg" );	

	image_destroy( dst );
	image_destroy( src );

	return 0;

}
