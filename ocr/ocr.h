/*
 *  ocr.h
 *  
 *
 *  Created by ÅÂ¿µ Á¤ on 06. 11. 20.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _OCR_H_
#define _OCR_H_

#include "image.h"

struct _ortho {
	int* x;
	int* y;
};

struct _ring {
	int* r;
	int length;
};

typedef struct _ortho*	ortho_ptr;
typedef struct _ortho	ortho;

typedef struct _ring*	ring_ptr;

void get_center( bitmap* src, int* x, int* y );

ortho_ptr create_ortho( int x, int y );
ortho_ptr get_ortho( bitmap* src );

ring_ptr create_ring( int r );
ring_ptr get_ring( bitmap* src );

void print_ring( ring_ptr ring );
void destroy_ring( ring_ptr ring );

void get_segment2( bitmap* src, int x, int y, char** ch, char** checked );
void get_segment( bitmap* src, int x, int y, char** ch, char** checked );

void segmentation( bitmap* src );

void ocr_init();

#endif
