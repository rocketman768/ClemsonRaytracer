#include <stdio.h>
#include <stdlib.h>
#include "ray.h"
#include "ppm_tools.h"

#define AA_SAMPLES 10
#define I_TOLERANCE 0.05

void make_image( FILE *outFP, model_t *model )
{
	int x,y;
	unsigned char *pixmap, *pixmap_location;
	size_t bytes_written;
	proj_t *projection = (proj_t*)(model->proj);

	pixmap = (unsigned char*)
		malloc( 3*sizeof(unsigned char)*projection->win_size_pixel[0]
			*projection->win_size_pixel[1]);
	pixmap_location = pixmap;

	for( y=0; y < projection->win_size_pixel[1]; y++ )
	{
		for( x=0; x < projection->win_size_pixel[0]; x++ )
		{
			make_pixel( model, x, y, pixmap_location );
			pixmap_location += 3;
		}
	}

	write_ppm_header( outFP,
		COLOR,
		projection->win_size_pixel[0],
		projection->win_size_pixel[1] );

	bytes_written = fwrite( pixmap, sizeof(unsigned char),
		3*projection->win_size_pixel[0]*projection->win_size_pixel[1],
		outFP);

	if( bytes_written != 3*projection->win_size_pixel[0]*projection->win_size_pixel[1] )
		fprintf(stderr, "\nERROR: only wrote %u bytes.\n", bytes_written);

	free(projection);
	free(pixmap);
}

void make_pixel( model_t *model, int x, int y, unsigned char *pixval )
{
	double center[3], gold1[3], gold2[3], randpoint[3], intensity[3], dir[3];
	double intens2[3], intens3[3], intens4[3];
	int count;
	proj_t *projection = model->proj;

	map_pix_to_world( projection, x, y, center );
	golden_scatter( projection, center, gold1, gold2 );

	intensity[0]=0; intensity[1]=0; intensity[2]=0;
	vl_scale3( intens2, intens2, 0.0 );
	vl_scale3( intens3, intens3, 0.0 );

	// Trace the center ray
	//Compute unit vector "dir" as "view_point" to "center"
	vl_diff3( dir, center, projection->view_point );
	vl_unitvec3( dir, dir );

	ray_trace( model, projection->view_point, dir, intensity, 0.0, NULL );

	// Trace two other rays in the pixel
	vl_diff3( dir, gold1, projection->view_point );
	vl_unitvec3( dir, dir );
	ray_trace( model, projection->view_point, dir, intens2, 0.0, NULL );
	vl_diff3( dir, gold2, projection->view_point );
	vl_unitvec3( dir, dir );
	ray_trace( model, projection->view_point, dir, intens3, 0.0, NULL );

	// If the two other rays come back with significantly different colors,
	// then we want to shoot more rays for anti-aliasing.
	if( (AA_SAMPLES != 0) && 
		(fabs(intens2[0]-intensity[0]) > I_TOLERANCE ||
		fabs(intens2[1]-intensity[1]) > I_TOLERANCE || 
		fabs(intens2[2]-intensity[2]) > I_TOLERANCE || 
		fabs(intens3[0]-intensity[0]) > I_TOLERANCE || 
		fabs(intens3[1]-intensity[1]) > I_TOLERANCE || 
		fabs(intens3[2]-intensity[2]) > I_TOLERANCE ) )
	{
		// The middle sample will count for 1/3rd the intensity.
		// The rest of the samples will account equally for the rest.

		vl_scale3( intensity, intensity, (double)1/(double)3 );
		vl_scale3( intens2, intens2, (double)1/(double)(3.0/2*(AA_SAMPLES)) );
		vl_scale3( intens3, intens3, (double)1/(double)(3.0/2*(AA_SAMPLES)) );
		vl_sum3( intensity, intensity, intens2 );
		vl_sum3( intensity, intensity, intens3 );

		for( count=0; count < (AA_SAMPLES-2); count++ )
		{
			vl_diff3( intens4, intens4, intens4 );

			rand_scatter( projection, center, randpoint );
			vl_diff3(dir, randpoint, projection->view_point);
			vl_unitvec3(dir,dir);
			ray_trace( model, projection->view_point, dir, intens4, 0.0, NULL );

			vl_scale3( intens4, intens4, (double)1/(double)(3.0/2*(AA_SAMPLES)) );
			vl_sum3( intensity, intensity, intens4 );
		}
	}

	// Truncate color intensities to the range [0,1]
	// then convert to range [0,255]
	for( count=0; count < 3; count++ )
	{
		if( intensity[count] > 1 )
		{
			intensity[count] = 1;
		}
		else if( intensity[count] < 0 )
			intensity[count] = 0;

		pixval[count] = 255 * intensity[count];
	}

}

