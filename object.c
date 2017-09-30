#include <stdlib.h>
#include <string.h>
#include "ray.h"


obj_t *object_init( FILE *fp, char *objClass, int objcat )
{
	obj_t *ret = (obj_t*) malloc( sizeof(obj_t) );
	static int id_count = 0;

	ret->objid = id_count++;
	ret->objclass = strdup(objClass);
	ret->objcat = objcat;

	return ret;
}

