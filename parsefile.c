#include <stdio.h>
#include <stdlib.h>

void parsefile_poundcomments( FILE *fp, double *vals, unsigned int num_elts )
{
	unsigned char buf[256];
	unsigned int vals_read = 0;
	char single_c;

	while(1)
	{
		if( fscanf( fp, "%lf", &vals[vals_read] ) != 1 )
		{
			if( feof(fp) || ferror(fp) )
			{
				fprintf(stderr, "\nERROR - Premature EOF.\n");
				exit(1);
			}

			fscanf(fp, "%c", &single_c );
			if( single_c == '#' )
				fgets( buf, 256, fp );
			else
			{
				fprintf(stderr, "\nERROR - Improperly formatted input file.\n");
				exit(1);
			}

		}
		else
			vals_read++;

		if( vals_read == num_elts )
			break;
	}

	/** Consume the rest of the line **/
	fgets( buf, 256, fp );
}

