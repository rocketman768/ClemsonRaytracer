#include <stdlib.h>
#include "ray.h"

void diffuse_illumination(
	model_t	*model,
	obj_t	*hitobj,
	double	*intensity)
{
	/*****************************************************************/
	/** Computes diffuse illumination on the hitpoint of an object. **/
	/*****************************************************************/

	obj_t *light = model->lights->head;

	while( light != NULL )
	{
		process_light( model->scene, hitobj, light, intensity );
		light = light->next;
	}
}

int process_light(
	list_t	*lst,
	obj_t	*hitobj,
	obj_t	*lobj,
	double	*ivec)
{
	double *occludevec = malloc( sizeof(double)*3 );
	double *normal = malloc( sizeof(double)*3 );
	lightobj_t *light = lobj->catpriv;
	sceneobj_t *sceneobj = hitobj->catpriv;
	obj_t *closestobj;
	double costheta;
	double dist_from_hit_to_light;

	normal[0] = sceneobj->normal[0];
	normal[1] = sceneobj->normal[1];
	normal[2] = sceneobj->normal[2];

	vl_diff3( occludevec, light->center, sceneobj->hitloc );
	dist_from_hit_to_light = vl_length3(occludevec);
	vl_unitvec3( occludevec, occludevec );
	vl_unitvec3( normal, normal );
	costheta = vl_dot3( occludevec, normal );

#ifdef DBG_DIFFUSE
	light_t *lt = lobj->priv;
	vl_vecprn1("hit object id was       ", &hitobj->objid);
	vl_vecprn3("hit point was           ", hitobj->hitloc);
	vl_vecprn3("normal at hitpoint      ", hitobj->normal);
	vl_vecprn1("light object id was     ", &lobj->objid);
	vl_vecprn3("light center was        ", light->center);
	vl_vecprn3("unit vector to light is ", occludevec);
	vl_vecprn1("distance to light is    ", &dist_from_hit_to_light);
	vl_vecprn1("cosine is               ", &costheta);
#endif

	/* If object occludes itself */
	if( costheta < 0 )
	{
		free(occludevec);
		free(normal);
		return -1;
	}

	/* If object is occluded by another object */

	vl_diff3( occludevec, sceneobj->hitloc, light->center );
	vl_unitvec3( occludevec, occludevec );
	closestobj = find_closest_obj( lst, light->center, occludevec, NULL );
	if( closestobj != hitobj )
	{

#ifdef DBG_DIFFUSE
		vl_vecprn1("hit object occluded by ", &closestobj->objid);
#endif
		free(occludevec);
		free(normal);
		return -1;
	}

	double *diffuse = malloc( sizeof(double)*3 );
	sceneobj->getdiff( hitobj, diffuse );
	ivec[0] += diffuse[0]*(light->emissivity[0])*costheta
		/ dist_from_hit_to_light;
	ivec[1] += diffuse[1]*(light->emissivity[1])*costheta
		/ dist_from_hit_to_light;
	ivec[2] += diffuse[2]*(light->emissivity[2])*costheta
		/ dist_from_hit_to_light;

	free(diffuse);
	free(occludevec);
	free(normal);
	return 0;
}

