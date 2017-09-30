#include <stdio.h>
#include "veclib.h"
#include <math.h>

/* Philip Lee's cross product */
void vl_cross3( double *resultv, double *v1, double *v2 )
{
	resultv[0] = v1[1]*v2[2]-v1[2]*v2[1];
	resultv[1] = v1[2]*v2[0]-v1[0]*v2[2];
	resultv[2] = v1[0]*v2[1]-v1[1]*v2[0];
}

void vl_xpose3( double trans[][3], double orig[][3] )
{
	double ans[3][3];
	int count, count2;

	for( count=0; count < 3; count++ )
	{
		for( count2=0; count2 < 3; count2++ )
		{
			ans[count][count2] = orig[count2][count];
		}
	}

	for( count=0; count < 3; count++ )
	{
		for( count2=0; count2 < 3; count2++ )
		{
			trans[count][count2] = ans[count][count2];
		}
	}

}

void vl_matmul3( double result[][3], double m1[][3], double m2[][3] )
{
	double ans[3][3], trans[3][3];
	int count, count2;

	vl_xpose3( trans, m2 );

	for( count=0; count < 3; count++ )
	{
		for( count2=0; count2 < 3; count2++ )
		{
			ans[count][count2] = vl_dot3( m1[count], trans[count2] );
		}
	}

	for( count=0; count < 3; count++ )
	{
		for( count2=0; count2 < 3; count2++ )
		{
			result[count][count2] = ans[count][count2];
		}
	}
}

void vl_project3( double *resultv, double *n, double *v )
{
	/** Projection of V onto plane with normal N **/

	/* resultv = v - (n . v)*n */
	vl_scale3( resultv, n, vl_dot3(n,v) );
	vl_diff3( resultv, v, resultv );
}

void vl_print3( double *vec )
{
	fprintf(stderr, "%.2lf %.2lf %.2lf", vec[0], vec[1], vec[2] );
}

void vl_xform3( double *resultv, double y[3][3], double x[3] )
{
	int count;
	for( count=0; count < 3; count++ )
	{
		resultv[count] = y[count][0]*x[0] +
			y[count][1]*x[1] +
			y[count][2]*x[2];
	}
}

/* Vector Length */
double vl_length3(double *v1)
{
    return sqrt(*v1 * *v1 + *(v1+1) * *(v1+1) + *(v1+2) * *(v1+2));
}

/* Scale Vector  (resultv = factor * v) */
void vl_scale3(
       double *resultv,         /* Result vector  */
       double *v,               /* Input vector   */
       double factor            /* Scaling factor */
)
{
    *resultv = *v * factor;
    *(resultv+1) = *(v+1) * factor;
    *(resultv+2) = *(v+2) * factor;
}

/* Unitize vector  (resultv = v / ||v|| ) */
void vl_unitvec3(
       double *resultv,         /* Unitized result vector  */
       double *v                /* Input vector            */
)
{
    double length = sqrt(*v * *v + *(v+1) * *(v+1) + *(v+2) * *(v+2));
    *resultv = *v / length;
    *(resultv+1) = *(v+1) / length;
    *(resultv+2) = *(v+2) / length;
}


/** This function was taking up a lot of time, so I replaced it with a define **/
/*
// Dot Product 
double vl_dot3(
       double *v1,              // Input vector 1
       double *v2               // Input vector 2
)
{
    return( v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] );
    //return (*v1 * *v2 + *(v1+1) * *(v2+1) + *(v1+2) * *(v2+2));
}
*/


/* Vector Difference (resultv = v1 - v2) */
void vl_diff3(
       double *resultv,         /* Result vector  */
       double *v1,              /* First vector   */
       double *v2               /* Second vector  */
)
{
    *resultv = *v1 - *v2;
    *(resultv+1) = *(v1+1) - *(v2+1);
    *(resultv+2) = *(v1+2) - *(v2+2);
}
     
/* Vector Addition (resultv = v1 + v2) */
void vl_sum3(
       double *resultv,         /* Result vector  */
       double *v1,              /* First vector   */
       double *v2               /* Second vector  */
)
{
    *resultv = *v1 + *v2;
    *(resultv+1) = *(v1+1) + *(v2+1);
    *(resultv+2) = *(v1+2) + *(v2+2);
}

int vl_get1(FILE *fp, double *v1) {
   return(vl_getN(fp, v1, 1));
}

int vl_get2(FILE *fp, double *v2) {
   return(vl_getN(fp, v2, 2));
}

int vl_get3(FILE *fp, double *v3) {
   return(vl_getN(fp, v3, 3));
}

int vl_getN(FILE *fp, double *v, int count) {
    int num=0;
    char buf[256];
    while (num < count) {
       if (fscanf(fp, "%lf", v) != 1) {
          if (fgets(buf, sizeof(buf), fp) == NULL) {
             fprintf(stderr, "End of file\n");
             return(-1);
          }
          if (buf[0] != '#') {
             fprintf(stderr, "Garbage in input\n");
             return(-1);
          }
       }
       else {
          num++;
          v++;
       }
    }
    return(count);
}
