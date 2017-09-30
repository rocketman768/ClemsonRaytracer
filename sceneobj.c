#include <stdio.h>
#include <stdlib.h>
#include "ray.h"

obj_t *sceneobj_init( FILE *fp, char *objClass )
{
	sceneobj_t *thisobj = malloc( sizeof(sceneobj_t) );

	material_init( fp, thisobj );

	obj_t *ret = object_init( fp, objClass, SCENEOBJ );

	ret->catpriv = thisobj;

	return ret;
}

void sceneobj_dump( FILE *out, obj_t *obj )
{
	sceneobj_t *sceneOBJ = obj->catpriv;

	fprintf(out, "\nObjClass = %s", obj->objclass);
	material_dump( out, sceneOBJ );
}

