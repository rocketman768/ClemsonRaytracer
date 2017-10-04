#include <math.h>
#include <stdlib.h>
#include "ray.h"
#include "rayhdrs.h"

proj_t *projection_init(FILE *inFP, int columns) {
   proj_t *newProjPtr;
   double tmp[2];
   double fov_deg;
   double focalLength;
   double aspectRatio;
   double fNumber;

   /* Allocate space for projection data */
   newProjPtr = (proj_t *) malloc(sizeof(proj_t));

   /* Parse the input */
   parsefile_poundcomments(inFP, &aspectRatio, 1);
   parsefile_poundcomments(inFP, &fov_deg, 1);
   parsefile_poundcomments(inFP, &focalLength, 1);
   parsefile_poundcomments(inFP, &fNumber, 1);
   parsefile_poundcomments(inFP, tmp, 2);
   newProjPtr->lens_resolution_pixel[0] = tmp[0] + 0.5;
   newProjPtr->lens_resolution_pixel[1] = tmp[1] + 0.5;
   parsefile_poundcomments(inFP, &newProjPtr->focal_plane_z_world, 1);

   /* Set the focal plane */
   newProjPtr->image_plane_z_world = focalLength;

   /* Set pixel column and row dimensions */
   newProjPtr->win_size_pixel[0] = columns;
   newProjPtr->win_size_pixel[1] = (double)columns / aspectRatio;

   /* Calculate world sizes for the image plane */
   newProjPtr->win_size_world[0] = 2.0 * focalLength * tan((M_PI / 180.0) * (fov_deg / 2.f));
   newProjPtr->win_size_world[1] = newProjPtr->win_size_world[0] / aspectRatio;

   /* Calculate world size for a pixel */
   newProjPtr->pix_size_world[0] = newProjPtr->win_size_world[0]/(double)newProjPtr->win_size_pixel[0];
   newProjPtr->pix_size_world[1] = newProjPtr->win_size_world[1]/(double)newProjPtr->win_size_pixel[1];

   /* Calculate the world size of the lens */
   newProjPtr->lens_size_world[0] = focalLength / fNumber;
   newProjPtr->lens_size_world[1] = newProjPtr->lens_size_world[0];

   return(newProjPtr);
}

void projection_dump(proj_t *projection) {
   fprintf(stderr, "Projection Dump:\n");
   fprintf(stdout, "   Pixel size:  %6d %6d\n",
          projection->win_size_pixel[0],
          projection->win_size_pixel[1]);
   fprintf(stdout, "   World size:  %6.2lf %6.2lf\n",
          projection->win_size_world[0],
          projection->win_size_world[1]);
   fprintf(stdout, "   Image plane: z=%6.2lf\n",
          projection->image_plane_z_world);
   fprintf(stdout, "   Lens resolution:  %6d %6d\n",
           projection->lens_resolution_pixel[0],
           projection->lens_resolution_pixel[1]);
   fprintf(stdout, "   Lens size:  %6.2f %6.2f\n",
           projection->lens_size_world[0],
           projection->lens_size_world[1]);
   fprintf(stdout, "\n");
}

void map_pix_to_world( proj_t *projection, int x, int y, double *world )
{
   double xd = (double)x + 0.5f;
   double yd = (double)y + 0.5f;
   double xResolution = projection->win_size_pixel[0];
   double yResolution = projection->win_size_pixel[1];

   world[0] = (1.0 - xd / xResolution) * projection->win_size_world[0]
               - projection->win_size_world[0] / 2.0;
   world[1] = (yd / yResolution) * projection->win_size_world[1]
               - projection->win_size_world[1] / 2.0;
   world[2] = projection->image_plane_z_world;
}

void map_lens_pix_to_world (proj_t *projection, int x, int y, double *world)
{
    const double xResolution = projection->lens_resolution_pixel[0];
    const double yResolution = projection->lens_resolution_pixel[1];

    double xd = (double)x + 0.5f;
    double yd = (double)y + 0.5f;
    
    world[0] = xd / xResolution * projection->lens_size_world[0];
    world[1] = yd / yResolution * projection->lens_size_world[1];
    world[2] = 0.f;
}

void thin_lens_model(
                     proj_t *projection,
                     double *lensCenter,
                     double *centerRayDirection,
                     double *lensPosition,
                     double *outgoingRayDirection)
{
    double centerRayFocalPoint[3];
    // Depth of the focal plane
    // TODO: check for infinity
    //double focalDistance = 1.0 / (1.0 / focalLength - 1.0 / a);
    
    // All rays should meet at this point on the focal plane.
    // This is the ray that extends from the lens center along the direction from the
    // image plane pixel until it hits the plane of focus.
    vl_scale3(centerRayFocalPoint, centerRayDirection, projection->focal_plane_z_world / centerRayDirection[2]);
    vl_sum3(centerRayFocalPoint, centerRayFocalPoint, lensCenter);
    
    // outgoingRayDirection = norm( centerRayFocalPoint - lensPosition )
    vl_diff3(outgoingRayDirection, centerRayFocalPoint, lensPosition);
    vl_unitvec3(outgoingRayDirection, outgoingRayDirection);
}

void golden_scatter( proj_t *proj, double *input, double *p1, double *p2 )
{
	static double G = 0.61803399;

	p1[0] = input[0] - (proj->pix_size_world[0]*G)/2;
	p1[1] = input[1] + (proj->pix_size_world[1]*G)/2;
	p1[2] = 0;

	p2[0] = input[0] + (proj->pix_size_world[0]*G)/2;
	p2[1] = input[1] - (proj->pix_size_world[1]*G)/2;
	p2[2] = 0;

}

void rand_scatter( proj_t *proj, double *input, double *output )
{
	/************************************************************************
	** Generates a random spot inside the square pixel surrounding "input" **
	** where "input" is the center coords of the square                    **
	************************************************************************/
	static double randx;
	static double randy;

	randx = drand48() - 0.5;
	randy = drand48() - 0.5;

	randx *= proj->pix_size_world[0];
	randy *= proj->pix_size_world[1];

	output[0] = input[0] + randx;
	output[1] = input[1] + randy;
	output[2] = 0;
}
