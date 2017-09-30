#include "ray.h"

obj_t *error_init( FILE *in, char *objclass )
{
	return NULL;
}

void error_dump( FILE *out, obj_t *objPtr )
{
	fprintf( stderr, "\nUnknown object type %s\n", objPtr->objclass );
}

struct
{
	char *className;
	obj_t *(*classInit)();
	void (*classDump)();
} classes[] =
	{{ "pointlight", lightobj_init, lightobj_dump },
	 { "sphere", sphere_init, sphere_dump },
	 { "plane", plane_init, plane_dump },
	 { "ellipsoid", ellipsoid_init, ellipsoid_dump},
	 { "sphere_pixmap", sphere_pixmap_init, sphere_pixmap_dump },
	 { "fplane", fplane_init, fplane_dump },
	 { "fplanetex", fplanetex_init, fplanetex_dump },
	 { NULL, error_init, error_dump}};

int model_init(
FILE    *in,
model_t *model)
{
   char  buf[256];
   char  objclass[128];
   obj_t *new;
   int classNdx;

/* now load the objects in the scene */

   while (fscanf(in, "%128s", objclass) == 1)
   {
      if (objclass[0] == '#')
      {
         /* It's a comment -- skip the rest of the line */
         fgets(buf, sizeof(buf), in); /* consume rest of line */
      }
      else /** I.E. "if this is NOT a comment **/
      {
         /** Test to see what kinda object we got. **/

         for( classNdx=0; classes[classNdx].className != NULL; classNdx++ )
         {
            if( strcasecmp(objclass, classes[classNdx].className) == 0 )
               break;
         }

         new = classes[classNdx].classInit(in, objclass);
         if( new == NULL )
         {
            fprintf(stderr, "\nERROR: failed to load type %s.\n", objclass);
            exit(2);
         }
         else
         {
            /* See if this is a sceneobj_t or lightobj_t */
            if( new->objcat == SCENEOBJ )
               list_add(model->scene, new);
            else
               list_add(model->lights, new);
         }

      }
   }

   return(0);
}

/**/
void model_dump(
FILE   *out,
model_t *model)
{
   obj_t *obj;
   int classNdx;

   /* Dump the scene objects */
   obj = model->scene->head;
   while (obj != NULL)
   {
      for( classNdx = 0; classes[classNdx].className != NULL; classNdx++ )
      {
         if( strcasecmp( obj->objclass, classes[classNdx].className ) == 0 )
            break;
      }

      classes[classNdx].classDump( out, obj );

      obj = obj->next;
   }


   /* Dump the light objects */
   obj = model->lights->head;
   while( obj != NULL )
   {
      for( classNdx = 0; classes[classNdx].className != NULL; classNdx++ )
      {
         if( strcasecmp( obj->objclass, classes[classNdx].className ) == 0 )
            break;
      }

      classes[classNdx].classDump( out, obj );

      obj = obj->next;
   }
}
