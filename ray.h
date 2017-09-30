/* ray.h */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

/* Category types */
#define LIGHTCAT      0
#define SCENEOBJ      1

/* light object */
typedef struct lightobj
{
	void *priv;
	double emissivity[3];
	double center[3];
} lightobj_t;

/** Projection data (window size and view point coordinates) **/
typedef struct projection_type
{
   int    win_size_pixel[2];    /* Projection screen size in pix     */
   double win_size_world[2];    /* Screen size in world coords       */
   double view_point[3];        /* Viewpt Loc in world coords        */
   double pix_size_world[2];	/* Size of one pixel in world coords */
}  proj_t;

/** Material structure **/
typedef struct material_type
{
   double  ambient[3];    /* Reflectivity for materials  */
   double  diffuse[3];
   double  specular[3];
}  material_t;

/** Generic object type **/
typedef struct obj_type
{
   struct  obj_type  *next;   /* Next object in list                   */
   int     objid;             /* Numeric serial # for debug            */
   char    *objclass;         /* Object type name (e.g. "plane")       */
   int     objcat;            /* Category code (e.g. 1 -> sceneobject) */
  
   void    *catpriv;          /* Private category-dependent data       */

} obj_t;



/** Scene object category **/

typedef struct sceneobj
{
   /* Pointer to type dependent data */
   void    *priv;        /* Private type-dependent data            */

/* Reflectivity for reflective objects */
   material_t material;

/* Optional plugins for retrieval of reflectivity */
/* useful for the ever-popular tiled floor        */
   void    (*getamb)(obj_t *, double *);
   void    (*getdiff)(obj_t *, double *);
   void    (*getspec)(obj_t *, double *);

/* Hits function. */
   double  (*hits)(double *base, double *dir, struct obj_type *);

/* "Hit" information for object */
   double  hitloc[3];    /* Last hit point                         */
   double  normal[3];    /* Normal at hit point                    */
   double  distance;     /* Distance from "base" to last hit point */
} sceneobj_t;


typedef struct sphere_type
{
   double  center[3];
   double  radius;
   void    *priv;
}  sphere_t;

typedef struct plane_type
{
   double  normal[3];
   double  point[3];
   void    *priv;         /* Data for specialized types  */
}  plane_t;

typedef struct ellipsoid_type
{
	double center[3];
	double a;
	double b;
	double c;

	void *priv;
} ellipsoid_t;

typedef struct
{
        double brightness_multiplier;
        double forward_vec[3];
        double side_vec[3];
        double top_vec[3];

        int pixcols;
        int pixrows;
        unsigned char *data;

        void *priv;
} sphere_pixmap_t;

typedef struct
{
	double brightness_multiplier;
	double size[2];

	int pixcols, pixrows;
	unsigned char *data;

	void *priv;
} fplanetex_t;

typedef struct
{
	double orient[3];
	double size[2];
	double rotmat[3][3];
	double lasthit[2];

	void *priv;
} fplane_t;

typedef struct list_type
{
   obj_t   *head;
} list_t;

typedef struct model_type
{
   proj_t  *proj;
   list_t  *lights;
   list_t  *scene;
}  model_t;

#include "rayhdrs.h"
