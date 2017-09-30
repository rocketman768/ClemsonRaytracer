#include <stdlib.h>
#include "ray.h"
#include "rayhdrs.h"

proj_t *projection_init(FILE *inFP, int columns) {
   proj_t *newProjPtr;
   char buf[256];
   int count;

   /* Allocate space for projection data */
   newProjPtr = (proj_t *) malloc(sizeof(proj_t));

   /* Fetch the world x and y unit sizes */
   count = 0;
   while (count < 2) {
      if (fscanf(inFP, "%lf", &newProjPtr->win_size_world[count]) <= 0) {
         /* Flush through newline */
         if (fgets(buf, sizeof(buf), inFP) == NULL) {
            fprintf(stderr, "Error: could not read world coordiantes\n");
            return(NULL);
         }
         if (buf[0] != '#') {
            fprintf(stderr, "Error: garbage in input\n");
            return(NULL);
         }
      }
      else {
         count++;
      }
   }

   /* Set pixel column and row dimensions */
   newProjPtr->win_size_pixel[0] = columns;
   newProjPtr->win_size_pixel[1] = 
        (newProjPtr->win_size_world[1]/newProjPtr->win_size_world[0])*columns;

   newProjPtr->pix_size_world[0] = newProjPtr->win_size_world[0]/(double)newProjPtr->win_size_pixel[0];
   newProjPtr->pix_size_world[1] = newProjPtr->win_size_world[1]/(double)newProjPtr->win_size_pixel[1];

   /* Next fetch the viewpoint (x, y, z) coordinates */
   count = 0;
   while (count < 3) {
      if (fscanf(inFP, "%lf", &newProjPtr->view_point[count]) <= 0) {
         /* Flush through newline */
         if (fgets(buf, sizeof(buf), inFP) == NULL) {
            fprintf(stderr, "Error: could not read viewpoint coordinates\n");
            return(NULL);
         }
         if (buf[0] != '#') {
            fprintf(stderr, "Error: garbage in input\n");
            return(NULL);
         }
      }
      else {
         count++;
      }
   }

   return(newProjPtr);
}

void projection_dump(proj_t *projection) {
   fprintf(stderr, "Projection Dump:\n");
   fprintf(stdout, "   Pixel size: %6d %6d\n",
          projection->win_size_pixel[0],
          projection->win_size_pixel[1]);
   fprintf(stdout, "   World size: %6.2lf %6.2lf\n",
          projection->win_size_world[0],
          projection->win_size_world[1]);
   fprintf(stdout, "   Viewpoint:  %6.2lf %6.2lf %6.2lf\n\n",
          projection->view_point[0],
          projection->view_point[1],
          projection->view_point[2]);
}

void map_pix_to_world( proj_t *projection, int x, int y, double *world )
{
   world[0] = (((double)(x+0)/
      (double)(projection->win_size_pixel[0]-1))*projection->win_size_world[0]);
   world[0] -= projection->win_size_world[0]/2.0;

   // The following segment is the way it used to be. It mapped sort of upside-down.
   //*(world+1) = (((double)(y+0)/
   //   (double)(projection->win_size_pixel[1]-1))*projection->win_size_world[1]);
   //*(world+1) -= projection->win_size_world[1]/2.0;

   world[1] = (1-y/(double)(projection->win_size_pixel[1]-1)) * projection->win_size_world[1];
   world[1] -= projection->win_size_world[1]/2.0;

   *(world+2) = 0;
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

void raytest(proj_t *projection) {
   int x;
   int y;
   double worldPix[3];
   double resultV[3];
   double unitV[3];
   double length;

   for (y=0; y<projection->win_size_pixel[1]; y++) {
      for (x=0; x<projection->win_size_pixel[0]; x++) {
          map_pix_to_world(projection, x, y, worldPix);

          vl_diff3(resultV, worldPix, projection->view_point);
          length = vl_length3(resultV);
          
          vl_unitvec3(unitV, resultV);

          fprintf(stderr, "%4d %4d %7.3lf %7.3lf %7.3lf %7.3lf %7.3lf %7.3lf\n",
                           x, y, 
                           worldPix[0], worldPix[1], 
                           length, 
                           unitV[0], unitV[1], unitV[2]); 
      }
   }
}
