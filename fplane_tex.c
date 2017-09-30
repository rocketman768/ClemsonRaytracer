#include <stdlib.h>
#include <stdio.h>
#include "ray.h"
#include "ppm_tools.h"

obj_t *fplanetex_init( FILE *fp, char *objClass )
{
	double vals[3];
	char filename[100];
	FILE *filep;

	fplanetex_t *thisFplaneTex = malloc( sizeof(fplanetex_t) );

	obj_t *ret = fplane_init(fp, objClass);
	sceneobj_t *sceneobj = (sceneobj_t*)(ret->catpriv);
	plane_t *plane = (plane_t*)(sceneobj->priv);
	fplane_t *fplane = (fplane_t*)(plane->priv);

	//Get the values for this particular type
	parsefile_poundcomments( fp, vals, 3 );
	thisFplaneTex->brightness_multiplier = vals[0];
	thisFplaneTex->size[0] = vals[1];
	thisFplaneTex->size[1] = vals[2];

	//Get the filename
	fscanf( fp, "%100s", filename );
	filep = fopen( filename, "r" );
	if( filep == NULL )
	{
		fprintf(stderr, "ERROR - Could not read the texture file.\n");
		exit(1);
	}
	thisFplaneTex->data = parseppm( filep, NULL, &thisFplaneTex->pixcols, &thisFplaneTex->pixrows, NULL );

	sceneobj->getamb = fplanetex_getamb;

	fplane->priv = thisFplaneTex;

	fclose(filep);
	return ret;
}

void fplanetex_dump( FILE *out, obj_t *obj )
{
	fplane_t *fplane = (fplane_t*)(((plane_t*)( ((sceneobj_t*)( obj->catpriv ))->priv))->priv);
	fplanetex_t *fplanetex = (fplanetex_t*)(fplane->priv);

	fplane_dump( out, obj );
	fprintf( stderr, "FplaneTex data - " );
	fprintf( stderr, "\n\tBrightness: %lf", fplanetex->brightness_multiplier );
	fprintf( stderr, "\n\tSize: %.2lf %.2lf", fplanetex->size[0], fplanetex->size[1] );
	fprintf( stderr, "\n\tData: %p\n", fplanetex->data ); 
}

void fplanetex_getamb( obj_t *obj, double *ambient )
{
	sceneobj_t *sceneobj = (sceneobj_t*)(obj->catpriv);
	plane_t *plane = ((plane_t*)(sceneobj->priv));
	fplane_t *fplane = ((fplane_t*)(plane->priv));
	fplanetex_t *fplaneTex = ((fplanetex_t*)(fplane->priv));

	double newhit[3];
	double xpercent, ypercent;
	unsigned int row, column;
	unsigned char *pix;

	vl_diff3( newhit, sceneobj->hitloc, plane->point );
	vl_xform3( newhit, fplane->rotmat, newhit );

	xpercent = newhit[0] / fplaneTex->size[0];
	ypercent = newhit[1] / fplaneTex->size[1];

	row = ypercent * fplaneTex->pixrows;
	column = xpercent * fplaneTex->pixcols;

	// To repeat the texture across the entire fplane
	row = row % fplaneTex->pixrows;
	column = column % fplaneTex->pixcols;

	pix = &fplaneTex->data[ 3 * row * fplaneTex->pixcols + 3 * column];
	ambient[0] = fplaneTex->brightness_multiplier * (double)pix[0]/255;
	ambient[1] = fplaneTex->brightness_multiplier * (double)pix[1]/255;
	ambient[2] = fplaneTex->brightness_multiplier * (double)pix[2]/255;
 
}

