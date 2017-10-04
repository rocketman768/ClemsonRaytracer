#include "veclib.h"

/** parsefile.c **/
void parsefile_poundcomments( FILE *fp, double *vals, unsigned int num_elts );

/** light.c **/
obj_t *lightobj_init( FILE *fp, char *objClass );
void lightobj_dump( FILE *out, obj_t *obj );

/** illuminate.c **/
void diffuse_illumination(
        model_t *model,
        obj_t   *hitobj,
        double  *intensity);
int process_light(
        list_t  *lst,
        obj_t   *hitobj,
        obj_t   *lobj,
        double  *ivec);

/** ellipsoid.c **/
obj_t *ellipsoid_init( FILE *fp, char *objClass );
double ellipsoid_hits( double *base, double *dir, obj_t *obj );
void ellipsoid_get_amb( obj_t *obj, double *ambient );
void ellipsoid_get_diff( obj_t *obj, double *diff );
void ellipsoid_get_spec( obj_t *obj, double *spec );
void ellipsoid_dump( FILE *out, obj_t *object );

/** fplane.c **/
obj_t *fplane_init( FILE *fp, char *objClass );
double fplane_hits( double *base, double *dir, obj_t *obj );
void fplane_dump( FILE *out, obj_t *obj );

/** fplane_tex.c **/
obj_t *fplanetex_init( FILE *fp, char *objClass );
void fplanetex_dump( FILE *out, obj_t *obj );
void fplanetex_getamb( obj_t *obj, double *ambient );

/** projection.c **/
void map_pix_to_world (proj_t *projection, int x, int y, double *world);
void map_lens_pix_to_world (proj_t *projection, int x, int y, double *world);
void thin_lens_model(
                     proj_t *projection,
                     double *lensCenter,
                     double *centerRayDirection,
                     double *lensPosition,
                     double *outgoingRayDirection);
proj_t *projection_init(FILE *inFP, int columns);
void golden_scatter( proj_t *proj, double *input, double *p1, double *p2 );
void rand_scatter( proj_t *proj, double *input, double *output );
void projection_dump(proj_t *proj);
void projection_test( FILE   *out, proj_t *proj);

/** image.c **/
void make_image( FILE *outFP, model_t *model );
void make_pixel( model_t *model, int x, int y, unsigned char *pixval );

/** list.c **/
struct list_type *list_init(void);
void list_add(struct list_type *list, struct obj_type *obj);

/** model.c **/
int model_init( FILE *fp, model_t *model);
void model_dump( FILE *out, model_t *model);

/** ray.c **/
void ray_trace( model_t *model,
        double base[3],
        double dir[3],
        double intensity[3],
        double total_dist,
        obj_t *lasthit );

obj_t *find_closest_obj(
        list_t *scene,
        double base[],
        double unitDir[],
        void *lasthit);

/** sphere.c **/
obj_t *sphere_init(FILE *fp, char *objclass);
void sphere_dump(FILE *out, obj_t *objPtr);
void sphere_get_amb( obj_t *obj, double *ambient );
void sphere_get_diff( obj_t *obj, double *diff );
void sphere_get_spec( obj_t *obj, double *spec );
double sphere_hits(double base[], double unitDirection[], obj_t *objPtr);

/** sphere_pixmap.c **/
void sphere_pixmap_get_amb( obj_t *obj, double *ambient );
void sphere_pixmap_dump( FILE *out, obj_t *obj );
obj_t *sphere_pixmap_init( FILE *fp, char *objClass );

/** plane.c **/
obj_t *plane_init(FILE *fp, char *objclass);
void plane_dump(FILE *out, obj_t *objPtr);
double plane_hits(double base[], double unitDirection[], obj_t *objPtr);
void plane_get_amb( obj_t *obj, double *ambient );
void plane_get_diff( obj_t *obj, double *diff );
void plane_get_spec( obj_t *obj, double *spec );

/** material.c **/
int material_init(FILE *fp, sceneobj_t *scene_objPtr);
void material_dump(FILE *out, sceneobj_t *materialPtr);

/** sceneobj.c **/
obj_t *sceneobj_init(FILE *fp, char *objclass);
void sceneobj_dump(FILE *out, obj_t *obj);


/** object.c **/
obj_t *object_init(FILE *fp, char *objclass, int objcat);

