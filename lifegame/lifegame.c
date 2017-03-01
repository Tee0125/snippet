/*
 * Eletric Electronic a215218 Jung Tae young
 *
 * Tested in gcc 3.2.2 with gentoo linux
 * Tested in gcc 3.2 with solaris 2.7 ( sol.ce.hongik.ac.kr )
 * Tested in gcc 3.2 with Redhat Linux 8.0 ( linux.ce.hongik.ac.kr )
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

/* The idea in the board.. using in memory allocation */
#define MEM_COL 82
#define MEM_ROW 32

#define MAX_COL 80
#define MAX_ROW 30

/* the center_x - 1, center_y - 1 (to put the seeds) */ 
#define CENTER_X 40
#define CENTER_Y 15

/* define FILENAME for output */
#define OUTPUT_FILENAME "lifegame.out"

/* Make Next Generation */
void next_gen( char** next, char** current );

/* check around the spot and return next generation's var */
char check_it( char** current , int col, int row );

/* Print the generation to FILE */
void print_gene( char** current, FILE *out );

/* Put the first seed of life to the cell */
void put_seeds( char** current, int seed_type );

int main( int argc, char *argv[] )
{

	int seed_type;
	int generation;

	/* the space pointed by current, next */
	char var[2][MEM_ROW][MEM_COL];
	
	char* ( current[MEM_ROW] );
	char* ( next[MEM_ROW] );

	/* tmp pointer for using (next <-> current) */
	char* tmp;
	
	FILE* output;

	/* using i to set, change (next, current) & count -> generation */
	int i = 0, count = 0;

	/* check the number of arguments */
	if( argc != 3 ){

		fprintf( stderr, "%s: First argument error.\n", *argv );
		exit(1);

	}

	/* check the range of first argument */
	seed_type = atoi( argv[1] );
	if( seed_type < 1 || seed_type > 8  ){

		fprintf( stderr, "%s: First argument error.\n", *argv );
		exit(2);

	}
	
	/* check the range of second argument */
	generation = atoi( argv[2] );
	if( generation < 0 || generation > 100000  ){

		fprintf( stderr, "%s: Second argument error.\n", *argv );
		exit(3);

	}
	
	/* Open the file for output.. */
	if( (output = fopen( OUTPUT_FILENAME, "w" )) ){
			
		/* Initionalize cells */
		memset( var, '0', sizeof( var ) );

		/* set the current, next pointer */
		for( i = 0 ; i < MEM_ROW ; i++ ){

			current[i] = var[0][i];
			next[i] = var[1][i];

		}
		
		/* put the seeds of life */
		put_seeds( current , seed_type );
		
		/* print current seeds of life */
		print_gene( current, output );
		
		while( count < generation ) /* range of the generation */
		{
				
			/* make next generation */
			next_gen( next, current );
	
			/* print.. next generation to the file.. */
			print_gene( next, output );

			/* Change next <-> current.. */
			for( i = 1 ; i <= MAX_ROW ; i++ ){

				tmp = next[i];
				next[i] = current[i];

				current[i] = tmp;
				
				/* To avoid mistakes.. */
				tmp = NULL;

			}
			
			/* increase generation */
			count++;


		}

		/* Close File Pointer */
		fclose( output );
		return 0;
		
	}
	else {

		fprintf( stderr ,"Error in open the file %s.out for output\n", *argv );
		exit(4);

	}

}

/* Make Next Generation */
void next_gen( char** next, char** current ){
	
	int i, j;

	for( j = 1 ; j <= MAX_ROW ; j++ ){

		for( i = 1 ; i <= MAX_COL ; i++ ){

			/* check around the spot.. and put the result to next generation */
			next[j][i] = check_it( current, i, j );

		}

	}

}

/* check around the spot and return next generation's var */
char check_it( char** current , int col, int row ){

	int i, j;
	int count = 0;

	for( j = -1 ; j < 2 ; j++ ){

		for( i = -1 ; i < 2 ; i++ ){

			/* current spot */
			if( i == 0 && j == 0 ){
						
				continue; 
			}
			else {

				/* count the seed around the spot */
				if( current[j+row][i+col] == '1' )
					count++;

			}


		}


	}
	
	/* current spot is '1' -> (count >= 4)|(count <=1)?'0':'1' */
	if( current[row][col] == '1' ){
				
		if( count <= 1 || count >= 4 )	
			return '0';
		
		else
			return '1';

	}

	/* current spot is '0' -> (count == 4)?'1':'0' */
	else{

		if( count == 3 )	
			return '1';
		
		else
			return '0';

	}
	
}

/* Print the generation to FILE */
void print_gene( char** current, FILE *out ){

	int i, j;

	for( j = 1 ; j <= MAX_ROW ; j++ ){

		for( i = 1 ; i <= MAX_COL ; i++ ){

			/* Put current spot to the FILE */
			putc( current[j][i], out );

		}
		/* go next line */
		putc( '\n', out );

	}
	/* current generation is ended.. add newline */
	putc( '\n', out );

}
	
/* Put the first seed of life to the cell */
void put_seeds( char** current, int seed_type ){

	int i, j;	
	const char seeds[8][3][3] = {

		/* Seed of life ... type 1 -> seeds[0] */
		{
			{	'0', '1', '0'	},
			{	'0', '1', '1'	},
			{	'1', '1', '0'	}
		},

		/* Seed of life ... type 2 -> seeds[1] */
		{
			{	'0', '1', '0'	},
			{	'1', '1', '1'	},
			{	'0', '0', '1'	}
		},

		/* Seed of life ... type 3 -> seeds[2] */
		{
			{	'1', '0', '1'	},
			{	'1', '1', '1'	},
			{	'0', '1', '0'	}

		},

		/* Seed of life ... type 4 -> seeds[3] */
		{
			{	'1', '1', '1'	},
			{	'0', '1', '0'	},
			{	'0', '0', '1'	}
		},

		/* Seed of life ... type 5 -> seeds[4] */
		{
			{	'1', '1', '1'	},
			{	'1', '1', '1'	},
			{	'1', '0', '1'	}
		},

		/* Seed of life ... type 6 -> seeds[5] */
		{
			{	'1', '0', '1'	},
			{	'1', '1', '0'	},
			{	'1', '0', '1'	}
		},

		/* Seed of life ... type 7 -> seeds[6] */
		{
			{	'0', '1', '0'	},
			{	'1', '1', '1'	},
			{	'0', '1', '0'	}
		},

		/* Seed of life ... type 8 -> seeds[7] */
		{
			
			{	'1', '1', '0'	},
			{	'0', '1', '1'	},
			{	'1', '0', '0'	}
		}

	};
		
	/* array's start is 0.. not 1 */
	seed_type--;
	
	/* Put the seeds of life */
	for( i = 0 ; i < 3 ; i++ ){

		for( j = 0 ; j < 3 ; j++ ){

			/*
			 * CENTER_X is 40, CENTER_Y is 15 
			 * 
			 * and the array's col = 1 ~ 80
			 * and the array's row = 1 ~ 30
			 *
			 * the 40,15 is 41,16 in here
			 * 
			 * so center_x - 1 is 40 (CENTER_X) 
			 * &  center_y - 1 is 15 (CENTER_Y)
			 * 
			 */
			current[ CENTER_Y + i ][ CENTER_X + j ] = seeds[seed_type][i][j];

		}

	}
			
}

