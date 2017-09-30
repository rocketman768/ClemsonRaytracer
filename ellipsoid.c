#include <stdio.h>
#include <stdlib.h>
#include "ray.h"

obj_t *ellipsoid_init( FILE *fp, char *objClass )
{
	double vals[6];

	ellipsoid_t *thisellipsoid = malloc( sizeof(ellipsoid_t) );

	obj_t *ret = sceneobj_init( fp, objClass );

	//Get ellipsoid-specific values from fp
	parsefile_poundcomments( fp, vals, 6 );
	thisellipsoid->center[0] = vals[0];
	thisellipsoid->center[1] = vals[1];
	thisellipsoid->center[2] = vals[2];
	thisellipsoid->a = vals[3];
	thisellipsoid->b = vals[4];
	thisellipsoid->c = vals[5];

	// Set the getamb function
	((sceneobj_t*)(ret->catpriv))->getamb = ellipsoid_get_amb;
	// Set the getdiff function
	((sceneobj_t*)(ret->catpriv))->getdiff = ellipsoid_get_diff;
	// Set the getspec function
	((sceneobj_t*)(ret->catpriv))->getspec = ellipsoid_get_spec;
	// Set the corresponding sceneobj's hit function to ellipsoid_hits()
	((sceneobj_t*)(ret->catpriv))->hits = ellipsoid_hits;
	// Set up this object with the correct plane info we read in
        ((sceneobj_t*)(ret->catpriv))->priv = thisellipsoid;

	return ret;
}

double ellipsoid_hits( double *base, double *dir, obj_t *obj )
{
	/*****************************************************************
	** Returns the length of the vector that hits the ellipsoid.	**
	** If the vector does not hit the ellipsoid, returns -1		**
	*****************************************************************/

	double a,b,c, b2minus4ac, vprime[3], dist, hitpoint[3], hitnormal[3], fx[3], fy[3];
	double transhit[3], denom;

	sceneobj_t *sceneobj = (sceneobj_t*)(obj->catpriv);
	ellipsoid_t *ellipsoid = (ellipsoid_t*)(sceneobj->priv);

	double mu[3] = { ellipsoid->b * ellipsoid->b * ellipsoid->c * ellipsoid->c,
		ellipsoid->a * ellipsoid->a * ellipsoid->c * ellipsoid->c,
		ellipsoid->a * ellipsoid->a * ellipsoid->b * ellipsoid->b};

	vl_diff3( vprime, base, ellipsoid->center );

	a = mu[0] * dir[0] * dir[0] + mu[1] * dir[1] * dir[1] + mu[2] * dir[2] * dir[2];
	b = 2*( mu[0] * vprime[0] * dir[0] + mu[1] * vprime[1] * dir[1] + mu[2] * vprime[2] * dir[2]);
	c = mu[0]*vprime[0]*vprime[0] + mu[1]*vprime[1]*vprime[1] + mu[2]*vprime[2]*vprime[2] - mu[0]*ellipsoid->a*ellipsoid->a;

	b2minus4ac = b*b - 4*a*c;

	if( b2minus4ac <= 0 )
	{
		sceneobj->distance = -1;
		return -1;
	}

	//Compute the distance
	dist = (-b - sqrt(b2minus4ac)) / (2*a);
	sceneobj->distance = dist;

	//Compute the translated hitpoint
	vl_scale3( transhit, dir, dist );
	vl_sum3( transhit, transhit, vprime );

	//Compute the real hitpoint
	vl_scale3( hitpoint, dir, dist );
	vl_sum3( hitpoint, hitpoint, base );
	sceneobj->hitloc[0] = hitpoint[0];
	sceneobj->hitloc[1] = hitpoint[1];
	sceneobj->hitloc[2] = hitpoint[2];

	//Make sure the ellipsoid isn't in front of the screen
	if( hitpoint[2] > 0 )
	{
		sceneobj->distance = -1;
		return -1;
	}

	//Compute the partial derivatives with respect to x and y
	denom = sqrt( 1 - (transhit[0]*transhit[0])/(ellipsoid->a*ellipsoid->a)
			- (transhit[1]*transhit[1])/(ellipsoid->b*ellipsoid->b)
		);

	//If we hit the ellipse on the boundary 
	//between the top and bottom halves...
	if( denom == 0 )
	{
		vl_diff3( hitnormal, hitpoint, ellipsoid->center );
		vl_unitvec3( hitnormal, hitnormal );
		sceneobj->normal[0] = hitnormal[0]; 
		sceneobj->normal[1] = hitnormal[1]; 
		sceneobj->normal[2] = hitnormal[2];
		return dist;
	}

	fx[0] = 1; fx[1] = 0;
	fx[2] = (transhit[0]*ellipsoid->c) / (denom*ellipsoid->a*ellipsoid->a);
	fy[0] = 0; fy[1] = 1;
	fy[2] = (-ellipsoid->c*transhit[1]) / (denom*ellipsoid->b*ellipsoid->b); 

	//Compute the normal by normalizing the cross product
	vl_cross3( hitnormal, fy, fx );
	vl_unitvec3( hitnormal, hitnormal );
	// Account for the fact that an ellipsoid is piecewise
	// and the normal on Z- is pointing towards the center...
	if( (hitpoint[2] - base[2]) < 0 )
		vl_scale3( hitnormal, hitnormal, -1 );
	sceneobj->normal[0] = hitnormal[0];
	sceneobj->normal[1] = hitnormal[1];
	sceneobj->normal[2] = hitnormal[2];

	return dist;
}

void ellipsoid_get_amb( obj_t *obj, double *ambient )
{
        sceneobj_t *sceneobj = (sceneobj_t*)(obj->catpriv);

        ambient[0] = sceneobj->material.ambient[0];
        ambient[1] = sceneobj->material.ambient[1];
        ambient[2] = sceneobj->material.ambient[2];
}

void ellipsoid_get_diff( obj_t *obj, double *diff )
{
	sceneobj_t *sceneobj = obj->catpriv;

	diff[0] = sceneobj->material.diffuse[0];
	diff[1] = sceneobj->material.diffuse[1];
	diff[2] = sceneobj->material.diffuse[2];
}

void ellipsoid_get_spec( obj_t *obj, double *spec )
{
	sceneobj_t *sceneobj = obj->catpriv;

	spec[0] = sceneobj->material.specular[0];
	spec[1] = sceneobj->material.specular[1];
	spec[2] = sceneobj->material.specular[2];
}

void ellipsoid_dump( FILE *out, obj_t *object )
{
	sceneobj_dump( out, object );

	ellipsoid_t *thisellipsoid = (ellipsoid_t*)( ((sceneobj_t*)( object->catpriv ))->priv);

        fprintf( out, "\nEllipsoid data -" );
        fprintf( out, "\n\tCenter: %.2lf %.2lf %.2lf",
		thisellipsoid->center[0],
		thisellipsoid->center[2],
		thisellipsoid->center[3]);
        fprintf( out, "\n\tSemi-major axes: %.2lf %.2lf %.2lf\n",
		thisellipsoid->a,
		thisellipsoid->b,
		thisellipsoid->c);
}
