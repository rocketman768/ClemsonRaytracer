#include <stdlib.h>
#include <stdio.h>
#include "ray.h"

obj_t *fplane_init( FILE *fp, char *objClass )
{
	fplane_t *thisfplane = malloc(sizeof(fplane_t));
	double vals[5], x[3],y[3],z[3], orient_unit[3], plane_unit[3];
	obj_t *ret = plane_init( fp, objClass );
	plane_t *plane = (((sceneobj_t *)(ret->catpriv))->priv);

	parsefile_poundcomments( fp, vals, 5 );
	thisfplane->orient[0] = vals[0];
	thisfplane->orient[1] = vals[1];
	thisfplane->orient[2] = vals[2];
	thisfplane->size[0] = vals[3];
	thisfplane->size[1] = vals[4];

	/* Change the default plane hits to fplane_hits */
	((sceneobj_t *)(ret->catpriv))->hits = fplane_hits;

	/* Calculate the x vector */
	vl_unitvec3( orient_unit, thisfplane->orient );
	vl_unitvec3( plane_unit, plane->normal );

	vl_project3( x, plane_unit, orient_unit );
	vl_unitvec3( x, x );

	/* Calculate the z vector */
	vl_unitvec3( z, plane->normal );

	/* Calculate the y vector */
	vl_cross3( y, z, x );
	vl_unitvec3( y, y );

	/* Produce the rotation matrix */
	thisfplane->rotmat[0][0] = x[0];
	thisfplane->rotmat[0][1] = x[1];
	thisfplane->rotmat[0][2] = x[2];
	
	thisfplane->rotmat[1][0] = y[0];
	thisfplane->rotmat[1][1] = y[1];
	thisfplane->rotmat[1][2] = y[2];

	thisfplane->rotmat[2][0] = z[0];
	thisfplane->rotmat[2][1] = z[1];
	thisfplane->rotmat[2][2] = z[2];

	/* Link the fplane to the plane */
	plane->priv = thisfplane;
	return ret;
}

double fplane_hits( double *base, double *dir, obj_t *obj )
{
	double t, newhit[3];
	sceneobj_t *sceneobj;
	plane_t *plane;
	fplane_t *fplane;

	t = plane_hits( base, dir, obj );

	if( t < 0 )
		return t;

	sceneobj = ((sceneobj_t *)(obj->catpriv));
	plane = ((plane_t *)(sceneobj->priv));
	fplane = ((fplane_t *)(plane->priv));

	/* Get new coordinates for the hitpoint */
	vl_diff3( newhit, sceneobj->hitloc, plane->point );
	vl_xform3( newhit, fplane->rotmat, newhit );

	/* See if we hit the plane inside the boundaries */
	if( (newhit[0] > 0 && newhit[0] < fplane->size[0]) &&
		(newhit[1] > 0 && newhit[1] < fplane->size[1]))
	{
		//fprintf( stderr, "\nHit: %.2lf %.2lf", newhit[0], newhit[1] );
		return t;
	}

	else
		return -1;
}

void fplane_dump( FILE *out, obj_t *obj )
{
	plane_dump( out, obj );

	plane_t *plane = (plane_t*)( ((sceneobj_t*)( obj->catpriv ))->priv);
	fplane_t *fplane = (fplane_t *)(plane->priv);

	fprintf( out, "\nFinite plane data - ");
	fprintf( out, "\n\tOrientation: %.2lf %.2lf %.2lf",
		fplane->orient[0],
		fplane->orient[1],
		fplane->orient[2]);
	fprintf( out, "\n\tSize: %.2lf %.2lf\n",
		fplane->size[0],
		fplane->size[1]);
}

