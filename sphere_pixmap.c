#include <stdio.h>
#include <stdlib.h>
#include "ray.h"
#include "ppm_tools.h"

obj_t *sphere_pixmap_init( FILE *fp, char *objClass )
{
	double vals[4];
	char filename[100];
	FILE *filep;

	sphere_pixmap_t* thisSpherePixmap = malloc( sizeof(sphere_pixmap_t) );

	obj_t *ret = sphere_init( fp, objClass );
	sceneobj_t *sceneobj = (sceneobj_t*)(ret->catpriv);
	sphere_t *sphere = (sphere_t*)(sceneobj->priv);

	//Get the values for this particular type
	parsefile_poundcomments( fp, vals, 4 );
	thisSpherePixmap->brightness_multiplier = vals[0];
	thisSpherePixmap->forward_vec[0] = vals[1];
	thisSpherePixmap->forward_vec[1] = vals[2];
	thisSpherePixmap->forward_vec[2] = vals[3];

	//Get the filename
	fscanf( fp, "%100s", filename );
	filep = fopen( filename, "r" );
	if( filep  == NULL )
	{
		fprintf(stderr, "ERROR - Could not read pixmap filename.\n");
		exit(1);
	}
	thisSpherePixmap->data = parseppm( filep, NULL, &thisSpherePixmap->pixcols, &thisSpherePixmap->pixrows, NULL);

	fclose(filep);

	//Set side_vec
	thisSpherePixmap->side_vec[0] = -1 * thisSpherePixmap->forward_vec[2];
	thisSpherePixmap->side_vec[1] = 0;
	thisSpherePixmap->side_vec[2] = thisSpherePixmap->forward_vec[0];
	vl_unitvec3( thisSpherePixmap->side_vec, thisSpherePixmap->side_vec );

	//Set top_vec
	thisSpherePixmap->top_vec[0] = 0;
	thisSpherePixmap->top_vec[1] = 1;
	thisSpherePixmap->top_vec[2] = 0;


	//Reset the getamb function
	sceneobj->getamb = sphere_pixmap_get_amb;

	sphere->priv = thisSpherePixmap;
	return ret;
}

void sphere_pixmap_dump( FILE *out, obj_t *obj )
{
	sphere_dump( out, obj );
	sphere_pixmap_t *spherepix = (sphere_pixmap_t*)(((sphere_t*)( ((sceneobj_t*)( obj->catpriv ))->priv))->priv);

	fprintf(stderr, "Sphere pix data -");
	fprintf(stderr, "\n\tBrightness: %lf", spherepix->brightness_multiplier);
	fprintf(stderr, "\n\tForward Vec: %.2lf %.2lf %.2lf",
		spherepix->forward_vec[0],
		spherepix->forward_vec[1],
		spherepix->forward_vec[2]);
	fprintf(stderr, "\n\tData: %p\n", spherepix->data );
}

void sphere_pixmap_get_amb( obj_t *obj, double *ambient )
{
	unsigned int row, column;
	double hitvec[3], rowpercent, columnpercent, theta1, theta2;
	double clockvec[3], splitvec[3];
	unsigned char *pix;

	sceneobj_t *sceneobj = (sceneobj_t*)(obj->catpriv);
	sphere_t *sphere = (sphere_t*)(sceneobj->priv);
	sphere_pixmap_t *spherepix = (sphere_pixmap_t*)(sphere->priv);

	vl_diff3( hitvec, sceneobj->hitloc, sphere->center );
	clockvec[0] = hitvec[0];
	clockvec[1] = 0;
	clockvec[2] = hitvec[2];
	vl_sum3( splitvec, spherepix->forward_vec, spherepix->side_vec );
	vl_unitvec3( hitvec, hitvec );
	vl_unitvec3( clockvec, clockvec );
	vl_unitvec3( splitvec, splitvec );

	rowpercent = acos( vl_dot3(spherepix->top_vec, hitvec)  ) / M_PI;
	row = rowpercent * spherepix->pixrows;

	//Calculate the columnpercent
	theta1 = acos( vl_dot3(spherepix->forward_vec, clockvec) );
	theta2 = acos( vl_dot3(spherepix->side_vec, clockvec) );

	if( theta1 >= theta2 )
		columnpercent = (2*M_PI - acos( vl_dot3(splitvec, clockvec) ) ) / (2*M_PI);
	else
		columnpercent = acos( vl_dot3(splitvec, clockvec) ) / (2*M_PI);

	column = columnpercent * spherepix->pixcols;

	pix = &spherepix->data[3 * row * spherepix->pixcols + 3 * column];
	ambient[0] = spherepix->brightness_multiplier * (double)pix[0]/255;
	ambient[1] = spherepix->brightness_multiplier * (double)pix[1]/255;
	ambient[2] = spherepix->brightness_multiplier * (double)pix[2]/255;


}
