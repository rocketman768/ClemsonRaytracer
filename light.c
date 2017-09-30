#include <stdlib.h>
#include "ray.h"

obj_t *lightobj_init( FILE *fp, char *objClass )
{
	obj_t *ret = object_init( fp, objClass, LIGHTCAT );
	lightobj_t *thislight = malloc( sizeof(lightobj_t) );

	parsefile_poundcomments( fp, thislight->emissivity, 3 );
	parsefile_poundcomments( fp, thislight->center, 3 );

	ret->catpriv = thislight;

	return ret;
}

void lightobj_dump( FILE *out, obj_t *obj )
{
	lightobj_t *light = obj->catpriv;

	fprintf(out, "\nObjClass = %s", obj->objclass);
	fprintf(out, "\n\tEmissivity: %.2lf %.2lf %.2lf",
		light->emissivity[0],
		light->emissivity[1],
		light->emissivity[2]);
	fprintf(out, "\n\tCenter: %.2lf %.2lf %.2lf\n",
		light->center[0],
		light->center[1],
		light->center[2]);
}

