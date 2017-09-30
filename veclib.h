/* Prototype statements for vector library */

#define vl_dot3( v1, v2 ) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

/* Philip Lee's cross product */
void vl_cross3( double *resultv, double *v1, double *v2 );

/* Transpose 3x3 matrix */
void vl_xpose3( double trans[][3], double orig[][3] );

/* Multiply two 3x3 matrices */
void vl_matmul3( double result[][3], double m1[][3], double m2[][3] );

/* Project vector onto plane */
void vl_project3( double *resultv, double *n, double *v );

void vl_print3( double *vec );

/* Multiply a matrix by a vector */
void vl_xform3( double *resultv, double y[3][3], double x[3] );

/* Vector Length */
double vl_length3(double *v1);

/* Scale Vector  (resultv = factor * v) */
void vl_scale3(
       double *resultv,         /* Result vector  */
       double *v,               /* Input vector   */
       double factor            /* Scaling factor */
);

/* Unitize vector  (resultv = v / ||v|| ) */
void vl_unitvec3(
       double *resultv,         /* Unitized result vector  */
       double *v                /* Input vector            */
);

/*
// Dot Product
double vl_dot3(
       double *v1,              // Input vector 1
       double *v2               // Input vector 2
);
*/

/* Vector Difference (resultv = v1 - v2) */
void vl_diff3(
       double *resultv,         /* Result vector  */
       double *v1,              /* First vector   */
       double *v2               /* Second vector  */
);
     
/* Vector Addition (resultv = v1 + v2) */
void vl_sum3(
       double *resultv,         /* Result vector  */
       double *v1,              /* First vector   */
       double *v2               /* Second vector  */
);

int vl_get1(FILE *fp, double *v1);
int vl_get2(FILE *fp, double *v2);
int vl_get3(FILE *fp, double *v3);
int vl_getN(FILE *fp, double *v, int count);
