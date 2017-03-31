/*
 *  image.h
 *  
 *
 *  Created by ÅÂ¿µ Á¤ on 06. 11. 20.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _IMAGE_H_
#define _IMAGE_H_

#define ZERO_PADDING
#define THRESHOLD 70

#define USE_GD
#include <gd.h>

typedef gdImage image;

typedef struct {
	int width;
	int height;

	char** pixel;
} bitmap;

#define image_x(i) gdImageSX(i)
#define image_y(i) gdImageSY(i)

#define image_create( x, y ) gdImageCreateTrueColor( x, y)
#define image_destroy(i) gdImageDestroy(i)

#define image_open(n) open_img(n)
#define image_write(n,i) write_img(n,i)

image* open_img( char* fname );
void write_img( char* fname, gdImagePtr src );

void image_get( image* src, int x, int y, int* r, int* g, int* b );
void image_set( image* dst, int x, int y, int r, int g, int b );

#define bitmap_x(i) (i->width)
#define bitmap_y(i) (i->height)

bitmap* bitmap_create( int x, int y );
void bitmap_destroy( bitmap* bmap );

void bitmap_get( bitmap* src, int x, int y, int* var );
void bitmap_set( bitmap* dst, int x, int y, int var );

image*  convert_b2i( bitmap* bmap );
bitmap* convert_i2b( image* im );

void img_thresholding( gdImagePtr im, int threshold );
#endif
