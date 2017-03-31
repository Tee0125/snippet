#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "image.h"
#include "filter.h"

#include "ocr.h"

char* name;
int main( int argc, char** argv ){

	image*  src;
	bitmap* bmap;
	
	int i;
	
	char* file;

	char buf[255];
	char buf2[255];

	name = buf;
	
	if( argc == 1 ){
		(*argv) = "txt_sample.png";
		argc++;
	}
	else 
		argv++;

	filter_init();
	ocr_init();
	
	fprintf( stderr, "init finished\n" );

	while( argc > 1 ){
		
		file = *argv;

		argv++;
		argc--;

		// open image
		fprintf( stderr, "open image %s\n", file );
		if( (src = open_img( file )) == NULL ){
			fprintf( stderr, "failed open image %s\n", file );
			return -1;
		}

		// make bitmap
		bmap = make_bitmap_from_img( src );
		
		fprintf( stderr, "image is loaded\n" );
		
		// pre-processing
		
		fprintf( stderr, "starting find characters\n" );	
		// matching with segmentation
		segmentation( bmap );
		
		fprintf( stderr, "all characters are founded\n" );

		destroy_bitmap(bmap);
		gdImageDestroy(src);
		
	}
	// complete

	return 0;

}
