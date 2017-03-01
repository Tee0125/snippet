#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "gaussian.h"

#define WIDTH 512
#define HEIGHT 512

#define INFILE "lena512.raw"
#define OUTFILE "lena512_blured_%d.raw" 
#define DIFFFILE "lena512_blured_diff_%d.raw" 

#define LEVEL 20

int main( int argc, char** argv ){

	FILE* in;
	FILE* out;

	char  filename[255];
	
	int i;
	
	uchar* src;
	uchar* dst[LEVEL];
	uchar* diff[LEVEL-1];

	src = (uchar*)malloc(sizeof(uchar)*WIDTH*HEIGHT);

	for( i = 0 ; i < LEVEL ; i++ )
		dst[i]  = (uchar*)malloc(sizeof(uchar)*WIDTH*HEIGHT);
	
	for( i = 0 ; i < LEVEL-1 ; i++ )
		diff[i] = (uchar*)malloc(sizeof(uchar)*WIDTH*HEIGHT);
	
	
	in = fopen( INFILE, "rb" );
	fread( src, sizeof(uchar), WIDTH*HEIGHT, in );
	fclose( in );

	for( i = 0 ; i < LEVEL ; i++ ){
		gaussian_blur( dst[i], src, i, WIDTH, HEIGHT );

		sprintf( filename, OUTFILE, i );

		out = fopen( filename, "wb" );
		fwrite( dst[i], sizeof(uchar), WIDTH*HEIGHT, out );
		fclose( out );
	}
	
	for( i = 0 ; i < LEVEL-1 ; i++ ){
		difference( diff[i], dst[i+1], dst[i], WIDTH, HEIGHT );
		
		sprintf( filename, DIFFFILE, i );

		out = fopen( filename, "wb" );
		fwrite( diff[i], sizeof(uchar), WIDTH*HEIGHT, out );
		fclose( out );
	}
	
	return 0;
}
