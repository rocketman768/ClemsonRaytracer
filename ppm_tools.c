/***************************************************/
// Author:
// 	Philip Greggory Lee. 30 Jan 2006 - 9 Feb 2006
// Description:
//	Useful functions for dealing with .ppm files.
/***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm_tools.h"

int write_ppm_header( FILE *ppm, int type, unsigned int columns, unsigned int rows )
{
	char *ptype;

	if( type == COLOR )
		ptype = "P6";
	else if( type == BANDW )
		ptype = "P5";
	else
		return 1;

	if( fprintf(ppm, "%s\n%u\n%u\n%u\n", ptype, columns, rows, 255) <= 0 )
		return 1;

	return 0;
}

/* Reads a .ppm file "infile" (already open for reading).	*/
/* Places rows and columns and brightness of the image into	*/
/* "rows" , "columns" , and "brightness".			*/
/* Reads the data section into "data".				*/
/* Returns 0 on success, 1 on failure.				*/

unsigned char *parseppm( FILE *infile, int *file_t, int *columns, int *rows, int *brightness ) 
{
	int vals[3];						// converted values
	int count = 0;						// # of vals converted
	char fileType[3];					// PPM file type code
	char tmpArray[256];
	unsigned char *data;

	//Input file type (first 2 bytes of file)
	if (fscanf(infile, "%2s", fileType) != 1)
	{
		fprintf(stderr, "Error: input file is empty\n");
		return NULL;
	}

	// Validate that this is a recognized PPM file type
	if ((strcmp(fileType, "P5") != 0) && (strcmp(fileType, "P6") != 0))
	{
		fprintf(stderr,"Error: file is not a .ppm file\n");
		return NULL;
	}

	int valuesRead;
	char tmp;

	//The main header-checker loop
	while(1)
	{
		valuesRead = 0;
		tmpArray[255] = 0x01;		//Give the last index the value 0x01. You'll see why later in this while loop

		//First, try to read in a value
		if( (valuesRead = fscanf(infile, "%d", &vals[count])) == EOF )
		{
			fprintf(stderr, "Error: PPM Header Error - premature end of file\n\n");
			return NULL;
		}
		count += valuesRead;
		if( count >= 3 )
			break;

		//If we didn't get a value...
		if( valuesRead != 1 )
		{
			//...check to see if we are reading a comment
			fscanf(infile, "%c", &tmp);
			if( tmp != '#' )
			{
				fprintf(stderr, "Error: PPM Header Error - junk characters in the header\n\n");
				return NULL;
			}

			//Get the rest of the line (up to 255 characters) if we find a '#'
			fgets( tmpArray, sizeof(tmpArray), infile );
			//If we read in 255 chars, print a possible error.
			if( tmpArray[255] == '\0' )
				fprintf(stderr, "Warning: PPM Header Warning - comment contains >= 255 characters");
		}
	}

	*columns = vals[0];
	*rows = vals[1];
	if( brightness != NULL )
		*brightness = vals[2];

	//Get the rest of the line so that we start reading binary data after
	//this function is called.
	fgets( tmpArray, sizeof(tmpArray), infile );

	//Allocate some space for the pixel data
	data = malloc( sizeof(pixel)*(*rows)*(*columns) );
	if( data == NULL )
	{
		fprintf(stderr, "\nError: memory allocation error.\n");
		return NULL;
	}

	//Read the pixel data
	size_t bytes_read = fread( data, sizeof(pixel), (*rows)*(*columns), infile );
	if( bytes_read < (*rows)*(*columns) )
	{
		fprintf(stderr, "\nError: could not properly read the infile.\n");
		return NULL;
	}

	//Now set all our return values
	if( file_t != NULL )
	{
		if( strcmp( fileType, "P6" ) == 0 )
			*file_t = COLOR;
		else if( strcmp( fileType, "P5" ) == 0 )
			*file_t = BANDW;
		else
			*file_t = -1;
	}
 
	return data;
}

unsigned char *ppm_get_pixel( unsigned char *data, int total_cols, int col, int row )
{
	unsigned char *ret;

	ret = &data[3 * row * total_cols + 3 * col];

	return ret;
}
