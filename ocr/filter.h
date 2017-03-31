/*
 *  filter.h
 *  
 *
 *  Created by ÅÂ¿µ Á¤ on 06. 11. 20.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FILTER_H_
#define _FILTER_H_

#include "image.h"

#define LOG_SIZE	5

int** _laplace;
int** _blur;
int** _smooth;
int** _log;

void filter_init();

#define filter_laplace(dst,src)	_convolution( dst, src, _laplace, 3, 1 )
#define filter_blur(dst,src)	_convolution( dst, src, _blur, 3, 16 )
#define filter_smooth(dst,src)	_convolution( dst, src, _smooth, 3, 9 )
#define filter_log(dst,src)		_convolution( dst, src, _log, LOG_SIZE, 1 )

void filter_median( image* dst, image* src );
void filter_gradient( image* dst, image* src );
void filter_homogeneity( image* dst, image* src );
void filter_difference( image* dst, image* src );

void _convolution( image* dst, image* src, int** w, int wsize, int div );

void gray_scale( image* dst, image* src );

void gray_level_transform( image* dst, image* src, int x, int y );
void gray_level_transform2( image* dst, image* src );

void histogram_stretch( image* dst, image* src, int x, int y );
void histogram_equalize( image* dst, image* src );
#endif
