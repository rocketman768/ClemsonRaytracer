#include <stdio.h>
#include <stdlib.h>
#include "ray.h"

obj_t *plane_init( FILE *fp, char *objClass )
{
	double vals[6];

	plane_t *thisplane = malloc( sizeof(plane_t) );

	obj_t *ret = sceneobj_init( fp, objClass );

	//Get plane-specific values from the file
	parsefile_poundcomments( fp, vals, 6 );
	thisplane->normal[0] = vals[0];
	thisplane->normal[1] = vals[1];
	thisplane->normal[2] = vals[2];
	thisplane->point[0] = vals[3];
	thisplane->point[1] = vals[4];
	thisplane->point[2] = vals[5];

	// Set this object's get_amb function to plane_get_amb()
	((sceneobj_t*)(ret->catpriv))->getamb = plane_get_amb;
	// Set this object's get_diff function to plane_get_diff()
	((sceneobj_t*)(ret->catpriv))->getdiff = plane_get_diff;
	// Set this object's getspec function to plane_get_spec()
	((sceneobj_t*)(ret->catpriv))->getspec = plane_get_spec;
	// Set this object's hits function to hits_plane()
	((sceneobj_t*)(ret->catpriv))->hits = plane_hits;
	// Set up this object with the correct plane info we read in
	((sceneobj_t*)(ret->catpriv))->priv = thisplane; 

	return ret;
}

double plane_hits( double *base, double *dir, obj_t *obj )
{
	/********************************************************
	** Returns the length of the vector that hits the plane	*
	** If the vector does not hit the plane, returns -1	*
	*********************************************************/

	double unitdir[3], hitpoint[3], dist, NdotD;
	plane_t *plane;

	sceneobj_t *sceneobj = (sceneobj_t*)(obj->catpriv);
	plane = (plane_t*)(sceneobj->priv);
	vl_unitvec3( unitdir, dir );

	NdotD = vl_dot3( plane->normal, unitdir );
	if( NdotD == 0.0 )
		return -1;

	dist = (vl_dot3( plane->normal, plane->point ) 
		- vl_dot3( plane->normal, base )) / NdotD;

	//Make sure we didn't hit ourselves in the head
	if( dist < 0 )
	{
		sceneobj->distance = -1;
		return -1;
	}


	//Compute hitpoint
	vl_scale3( hitpoint, unitdir, dist );
	vl_sum3( hitpoint, hitpoint, base );
	sceneobj->hitloc[0] = hitpoint[0];
	sceneobj->hitloc[1] = hitpoint[1]; 
	sceneobj->hitloc[2] = hitpoint[2];

	//Make sure the plane isn't in front of the screen
	if( hitpoint[2] > 0 )
	{
		sceneobj->distance = -1;
		return -1;
	}

	//Set the normal at the hitpoint
	sceneobj->normal[0] = plane->normal[0]; 
	sceneobj->normal[1] = plane->normal[1]; 
	sceneobj->normal[2] = plane->normal[2];

	//Set the distance
	sceneobj->distance = dist;
	return dist;
}

void plane_get_amb( obj_t *obj, double *ambient )
{
	sceneobj_t *sceneobj = (sceneobj_t*)(obj->catpriv);

	ambient[0] = sceneobj->material.ambient[0];
	ambient[1] = sceneobj->material.ambient[1];
	ambient[2] = sceneobj->material.ambient[2];
}

void plane_get_spec( obj_t *obj, double *spec )
{
	sceneobj_t *sceneobj = obj->catpriv;

	spec[0] = sceneobj->material.specular[0];
	spec[1] = sceneobj->material.specular[1];
	spec[2] = sceneobj->material.specular[2];
}

void plane_get_diff( obj_t *obj, double *diff )
{
	sceneobj_t *sceneobj = obj->catpriv;

	diff[0] = sceneobj->material.diffuse[0];
	diff[1] = sceneobj->material.diffuse[1];
	diff[2] = sceneobj->material.diffuse[2];
}

void plane_dump( FILE *out, obj_t *object )
{
	sceneobj_dump( out, object );

	plane_t *thisplane = 
		(plane_t*)( ((sceneobj_t*)( object->catpriv ))->priv);

	fprintf( out, "\nPlane data -" );
	fprintf( out, "\n\tNormal: %.2lf %.2lf %.2lf",
		thisplane->normal[0],
		thisplane->normal[1],
		thisplane->normal[2]);
	fprintf( out, "\n\tPoint: %.2lf %.2lf %.2lf\n",
		thisplane->point[0],
		thisplane->point[1],
		thisplane->point[2]);

}

