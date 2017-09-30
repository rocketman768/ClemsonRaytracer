/***************************************************/
// Author:
// 	Philip Greggory Lee. 30 Jan 2006 - 9 Feb 2006
// Description:
//		Useful functions for dealing with .ppm files.
/***************************************************/

#define COLOR 1
#define BANDW 0

typedef struct{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} pixel;

int write_ppm_header( FILE *ppm, int type, unsigned int columns, unsigned int rows );
unsigned char *parseppm( FILE *infile, int *file_t, int *columns, int *rows, int *brightness );
unsigned char *ppm_get_pixel( unsigned char *data, int total_cols, int col, int row );

