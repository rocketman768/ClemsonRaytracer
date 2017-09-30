/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ray.h"

int main( int argc, char **argv )
{
	clock_t beg=NULL, end=NULL;
	int cols;
	FILE *outputFP;
	model_t *model  = (model_t *)malloc(sizeof(model_t));
	int rc;

	// Set up the drand48 engine
	long int *rand_init1 = malloc( sizeof(long int) );
	// Note, I do not initialize the integer...
	srand48( *rand_init1 ^ (long int)time(NULL) );
	free( rand_init1 );

	if (argc < 3)
	{
		fprintf(stderr, "\nError:\n\tUSAGE: ./ray3 output.ppm columns.\n");
		exit(1);
	}

	outputFP = fopen( argv[1], "w" );
	if( outputFP == NULL )
	{
		fprintf(stderr, "\nError: cannot open output file.\n");
		exit(1);
	}

	cols = atoi(argv[2]);

	if (cols < 1)
	{
		fprintf(stderr, 
			"Error: row or column number on command line non-numeric\n");
		exit(1);
	}

	model->proj = projection_init(stdin, cols);
	projection_dump(model->proj);

	model->lights  = list_init();
	model->scene   = list_init();

	rc = model_init(stdin, model);
	//model_dump(stderr, model);

	if( rc == 0 )
	{
		beg = clock();
		make_image( outputFP, model );
		end = clock();
	}

	printf("\nTime elapsed: %.3lf\n\n", (double)(end-beg)/(double)CLOCKS_PER_SEC);

	free(model);
	return 0;
}

