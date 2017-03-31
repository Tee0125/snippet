/*
 *  image.c
 *  
 *
 *  Created by ÅÂ¿µ Á¤ on 06. 11. 20.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "image.h"

image* open_img( char* fname ){

#ifdef USE_GD
	FILE* in;
	char* ext;
	
	image* im = NULL;
	
	ext = strrchr( fname, '.' );
	if( ext == NULL )
		return NULL;
	else 
		ext++;
	
	if( (in = fopen(fname, "rb")) == NULL )
		return NULL;
		
	if( strncasecmp( ext, "jpg", 3 ) == 0 )
		im = gdImageCreateFromJpeg(in);
		
	else if( strncasecmp( ext, "jpeg", 4 ) == 0 )
		im = gdImageCreateFromJpeg(in);
		
	else if( strncasecmp( ext, "gif", 3 ) == 0 )
		im = gdImageCreateFromGif(in);
		
	else if( strncasecmp( ext, "png", 3 ) == 0 )
		im = gdImageCreateFromPng(in);
	
	fclose(in);
	
	return im;
	
#else
	return NULL;
#endif

}

void write_img( char* fname, image* src ){

#ifdef USE_GD
	FILE* out;
	char* ext;
	
	ext = strrchr( fname, '.' );
	if( ext == NULL )
		return;
	else 
		ext++;	
		
	out = fopen( fname, "wb" );
	if( strncasecmp( ext, "jpg", 3 ) == 0 )
		gdImageJpeg( src, out, 100);
		
	else if( strncasecmp( ext, "jpeg", 4 ) == 0 )
		gdImageJpeg( src, out, 100);
		
	else if( strncasecmp( ext, "gif", 3 ) == 0 )
		gdImageGif( src, out );
		
	else if( strncasecmp( ext, "png", 3 ) == 0 )
		gdImagePng( src, out );
	
	fclose(out);
#endif
	
}

void image_get( image* src, int x, int y, int* r, int* g, int* b ){
	
	int pixel;
	
	if( x < 0 ){
#ifdef ZERO_PADDING
		(*r) = 0;
		(*g) = 0;
		(*b) = 0;
		return;
#else 
		x = 0;
#endif
	}
	else if( x >= image_x(src) ){
#ifdef ZERO_PADDING
		(*r) = 0;
		(*g) = 0;
		(*b) = 0;
		return;
#else 
		x = image_x(src)-1;
#endif
	}

	if( y < 0 ){
#ifdef ZERO_PADDING
		(*r) = 0;
		(*g) = 0;
		(*b) = 0;
		return;
#else 
		y = 0;
#endif
	}
	else if( y >= image_y(src) ){
#ifdef ZERO_PADDING
		(*r) = 0;
		(*g) = 0;
		(*b) = 0;
		return;
#else 
		y = image_y(src)-1;
#endif
	}
	pixel = gdImageGetPixel(src, x, y);
	
	(*r) = gdImageRed(src, pixel);
	(*g) = gdImageGreen(src, pixel);
	(*b) = gdImageBlue(src, pixel);

}

void image_set( image* dst, int x, int y, int r, int g, int b ){
	int pixel;
	
	if( r > 255 )		r = 255;
	else if( r < 0 )	r = 0;

	if( g > 255 )		g = 255;
	else if( g < 0 )	g = 0;

	if( b > 255 )		b = 255;
	else if( b < 0 )	b = 0;

	pixel = gdImageColorAllocate( dst, r, g, b );
	gdImageSetPixel( dst, x, y, pixel);
}

void image_thresholding( image* im, int threshold ){

	int i, j;
	int r, g, b;

	r = g = b = 0;
	for( j = 0 ; j < image_y(im) ; j++ ){

		for( i = 0 ; i < image_x(im) ; i++ ){
			image_get( im, i, j, &r, &g, &b );

			r = (r > threshold) ? 255: 0;
			g = (g > threshold) ? 255: 0;
			b = (b > threshold) ? 255: 0;

			image_set( im, i, j, r, g, b );
		}
	}
}

bitmap* bitmap_create( int x, int y ){

	bitmap* b;
	int i;

	b = (bitmap*) malloc( sizeof(bitmap) );
	b->pixel = (char**) malloc( sizeof(char*)*y );
	b->pixel[0] = (char*) malloc( sizeof(char)*x*y );

	for( i = 1 ; i < y ; i++ )
		b->pixel[i] = b->pixel[i-1] + x;
	
	b->width = x;
	b->height = y;

	return b;
}

void bitmap_destroy( bitmap* bmap ){

	free( bmap->pixel[0] );
	free( bmap->pixel );
	free( bmap );

}

void bitmap_get( bitmap* src, int x, int y, int* var ){

	(*var) = src->pixel[y][x];

}

void bitmap_set( bitmap* dst, int x, int y, int var ){
	
	if( var > 255 )
		var = 255;
	else if( var < 0 )
		var = 0;

	dst->pixel[y][x] = var;

}

image*  convert_b2i( bitmap* bmap ){

	image* im;

	int i, j;
	int var;

	im = image_create( bitmap_x(bmap), bitmap_y(bmap) );
	for( j = 0 ; j < bitmap_y(bmap) ; j++ ){
		for( i = 0 ; i < bitmap_x(bmap) ; i++ ){
			bitmap_get( bmap, i, j, &var );
			image_set( im, i, j, var, var, var );
		}
	}	
	return im;

}

bitmap* convert_i2b( image* im ){

	bitmap* bmap;

	int i, j;
	int r, g, b;
	
	bmap = bitmap_create( image_x(im), image_y(im) );
	for( j = 0 ; j < image_y(im) ; j++ ){
		for( i = 0 ; i < image_x(im) ; i++ ){
			image_get( im, i, j, &r, &g, &b );
			bitmap_set( bmap, i, j, (30*r+59*g+11*b)/100 );
		}
	}
	return bmap;	

}

void bitmap_thresholding( bitmap* im, int threshold ){

	int i, j;
	int var;

	for( j = 0 ; j < bitmap_y(im) ; j++ ){

		for( i = 0 ; i < bitmap_x(im) ; i++ ){
			bitmap_get( im, i, j, &var );
			var = (var > threshold) ? 255: 0;

			bitmap_set( im, i, j, var );
		}
	}
}

