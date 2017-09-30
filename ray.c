#include <stdio.h>
#include <stdlib.h>
#include "ray.h"

void ray_trace( model_t *model, 
	double base[3], 
	double dir[3], 
	double intensity[3], 
	double total_dist, 
	obj_t *lasthit )
{
	int count;
	obj_t *closest = find_closest_obj( model->scene, base, dir, lasthit );
	double *ambient = malloc( sizeof(double)*3 );
	double *specular = malloc( sizeof(double)*3 );

	if( closest == NULL || total_dist > 1000 )
	{
		free(ambient);
		free(specular);
		return;
	}

	sceneobj_t *sceneobj = (sceneobj_t*)(closest->catpriv);

	total_dist += sceneobj->distance;

	// Add the AMBIENT reflectivity to the total intensity
	sceneobj->getamb( closest, ambient );
	intensity[0] += ambient[0];
	intensity[1] += ambient[1];
	intensity[2] += ambient[2];


	// Add the DIFFUSE illumination to the total intensity
	diffuse_illumination( model, closest, intensity );

	// Divide the intensity by distance from the viewpoint
	for( count=0; count < 3; count++ )
		intensity[count] = intensity[count] / total_dist;

	// Add the SPECULAR (reflections) illumination to the total intensity
	sceneobj->getspec( closest, specular );
	if( specular[0] != 0 || specular[1] != 0 || specular[2] != 0 )
	{
		double *u, *v, *n, *intens2;
		u = malloc( 3*sizeof(double) );
		v = malloc( 3*sizeof(double) );
		n = malloc( 3*sizeof(double) );
		intens2 = malloc( 3*sizeof(double) );

		intens2[0] = 0;
		intens2[1] = 0;
		intens2[2] = 0;


		// u = -dir
		vl_scale3( u, dir, -1 );
		if( vl_length3(u) != 1 )
			vl_unitvec3(u,u);

		vl_unitvec3( n, sceneobj->normal );

		// v = 2*(u dot n)*n - u
		vl_scale3( v, n, 2*vl_dot3(u,n) );
		vl_diff3( v, v, u );
		vl_unitvec3( v, v );

		ray_trace( model, sceneobj->hitloc, v, intens2, total_dist, closest );

		intensity[0] += intens2[0]*specular[0];
		intensity[1] += intens2[1]*specular[1];
		intensity[2] += intens2[2]*specular[2];

		free(u); free(v); free(n); free(intens2);
	}

	free(ambient);
	free(specular);
}

obj_t *find_closest_obj(
	list_t *scene,
	double base[],
	double unitDir[],
	void *lasthit)
{

	/***********************************************************************/
	/** Returns the closest obj_t or NULL if the ray doesn't hit anything **/
	/***********************************************************************/

	obj_t *currentObj = scene->head;
	obj_t *closest = NULL;
	sceneobj_t *currentSceneObj;
	double distance, minDistance=0;

	//Get the objid of the closest object
	while( currentObj != NULL )
	{
		// Ignore the object which has generated the "base point
		if( currentObj == (obj_t*)lasthit )
			currentObj = currentObj->next;
		if( currentObj == NULL )
			break;

		currentSceneObj = (sceneobj_t*)(currentObj->catpriv);
		distance = currentSceneObj->hits( base, unitDir, currentObj );

		if( distance > 0 )
		{
			// Some stupid shit to make sure the next "if"
			// condition holds the first time through.
			if( minDistance == 0 )
				minDistance = distance + 1;

			if( distance < minDistance )
			{
				minDistance = distance;
				closest = currentObj;
			}
		}

		currentObj = currentObj->next;
	}

	return closest;

}

