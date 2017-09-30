#include <stdio.h>
#include <stdlib.h>
#include "ray.h"

obj_t *sphere_init( FILE *fp, char *objClass )
{
	double *vals = malloc( sizeof(double)*4 );

	obj_t *ret = sceneobj_init( fp, objClass );

	sphere_t *thissphere = malloc( sizeof(sphere_t) );

	//Get values from the input file
	parsefile_poundcomments( fp, vals, 4 );
	thissphere->center[0] = vals[0];
	thissphere->center[1] = vals[1];
	thissphere->center[2] = vals[2];
	thissphere->radius = vals[3];

	//Set the getamb function
	( (sceneobj_t*)(ret->catpriv) )->getamb = sphere_get_amb;
	//Set the getdiff function
	( (sceneobj_t*)(ret->catpriv) )->getdiff = sphere_get_diff;
	// Set the getspec function
	( (sceneobj_t*)(ret->catpriv) )->getspec = sphere_get_spec;
	//Set the hits function to hits_sphere()
	( (sceneobj_t*)(ret->catpriv) )->hits = sphere_hits;
	//Set up the object to return
	( (sceneobj_t*)(ret->catpriv) )->priv = thissphere;

	free(vals);
	return ret;
}

double sphere_hits( double *base, double *dir, obj_t *obj )
{
	double cprime[3], vprime[3], unitdir[3],  a, b, c, dist;
	double hitpoint[3], hitnormal[3], b2minus4ac;

	vl_unitvec3( unitdir, dir );
	sceneobj_t *sceneobj = (sceneobj_t*)(obj->catpriv);
	sphere_t *sphere = (sphere_t*)(sceneobj->priv);

	//Move the center of the sphere to (0,0,0) to make calcs easier.
	cprime[0] = 0.0; cprime[1] = 0.0; cprime[2] = 0.0;
	vl_diff3( vprime, base, sphere->center );

	a = vl_dot3( unitdir, unitdir );
	b = 2 * vl_dot3( vprime, unitdir );
	c = vl_dot3( vprime, vprime ) - ((sphere->radius)*(sphere->radius));

	b2minus4ac = b*b - 4*a*c;

	if( b2minus4ac <= 0 )
	{
		sceneobj->distance = -1;
		return -1;
	}

	//Compute the distance
	dist = ( -b - sqrt(b2minus4ac) ) / (2*a);
	sceneobj->distance = dist;

	//Compute the hitpoint
	vl_scale3( hitpoint, unitdir, dist );
	vl_sum3( hitpoint, hitpoint, base );
	sceneobj->hitloc[0] = hitpoint[0]; 
	sceneobj->hitloc[1] = hitpoint[1]; 
	sceneobj->hitloc[2] = hitpoint[2];

	//Make sure the sphere isn't in front of the screen
	if( hitpoint[2] > 0 )
	{
		sceneobj->distance = -1;
		return -1;
	}

	//Compute the unit normal at the hitpoint
	vl_diff3( hitnormal, hitpoint, sphere->center );
	vl_unitvec3( hitnormal, hitnormal );
	sceneobj->normal[0] = hitnormal[0]; 
	sceneobj->normal[1] = hitnormal[1]; 
	sceneobj->normal[2] = hitnormal[2];

	return dist;
}

void sphere_get_amb( obj_t *obj, double *ambient )
{
        sceneobj_t *sceneobj = (sceneobj_t*)(obj->catpriv);

        ambient[0] = sceneobj->material.ambient[0];
        ambient[1] = sceneobj->material.ambient[1];
        ambient[2] = sceneobj->material.ambient[2];
}

void sphere_get_diff( obj_t *obj, double *diff )
{
	sceneobj_t *sceneobj = obj->catpriv;

	diff[0] = sceneobj->material.diffuse[0];
	diff[1] = sceneobj->material.diffuse[1];
	diff[2] = sceneobj->material.diffuse[2];
}

void sphere_get_spec( obj_t *obj, double *spec )
{
	sceneobj_t *sceneobj = obj->catpriv;

	spec[0] = sceneobj->material.specular[0];
	spec[1] = sceneobj->material.specular[1];
	spec[2] = sceneobj->material.specular[2];
}

void sphere_dump( FILE *out, obj_t *object )
{
	sceneobj_dump( out, object );

	sphere_t *thissphere = (sphere_t*)( ( (sceneobj_t*)(object->catpriv) )->priv);

	fprintf(out, "\nSphere data -");
	fprintf(out, "\n\tCenter: %.2lf %.2lf %.2lf",
		thissphere->center[0],
		thissphere->center[1],
		thissphere->center[2]);
	fprintf(out, "\n\tRadius: %.2lf\n",
		thissphere->radius);
}

