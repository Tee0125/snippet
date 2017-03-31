#include <stdio.h>
#include <math.h>

int main( int argc, char** argv ){

	int i, j;

	for( i = 0 ; i < 105; i++ ){
		for ( j = 0 ; j < 105 ; j++ ){
			if( (j % 15) == 0 )
				printf( "\n" );
			printf( "%3d, ", (int)round(sqrt( i*i + j*j )) );
		}
	}
	return 0;
}
