#include <stdio.h>
#include <stdlib.h>

#include "wavelet.h"

int main( int argc, char** argv ){

	int i;
	char buf[512];

	for( i = 0 ; i < 7 ;i++ ){
		sprintf( buf, "wavelet_lena512_22_%d.raw", i+1 );
		wavelet_encoder_jty( "lena512.raw", buf, 'a', i+1 );

		sprintf( buf, "wavelet_lena512_swe_%d.raw", i+1 );
		wavelet_encoder_jty( "lena512.raw", buf, 'b', i+1 );

		sprintf( buf, "wavelet_lena512_crf_%d.raw", i+1 );
		wavelet_encoder_jty( "lena512.raw", buf, 'c', i+1 );
	}

	return 0;
}

