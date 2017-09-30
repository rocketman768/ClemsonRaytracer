#include <stdio.h>
#include <stdlib.h>
#include "ray.h"

int material_init( FILE *fp, sceneobj_t *sceneobj )
{
	double vals[9];

	parsefile_poundcomments( fp, vals, 9 );

	sceneobj->material.ambient[0] = vals[0];
	sceneobj->material.ambient[1] = vals[1]; 
	sceneobj->material.ambient[2] = vals[2];
	sceneobj->material.diffuse[0] = vals[3]; 
	sceneobj->material.diffuse[1] = vals[4]; 
	sceneobj->material.diffuse[2] = vals[5];
	sceneobj->material.specular[0] = vals[6]; 
	sceneobj->material.specular[1] = vals[7]; 
	sceneobj->material.specular[2] = vals[8]; 

	return 0;
}

void material_dump( FILE *out, sceneobj_t *sceneobj )
{
	fprintf( out, "\nMaterial Data -");
	fprintf( out, "\n\tAmbient: %.2lf %.2lf %.2lf", 
		sceneobj->material.ambient[0], 
		sceneobj->material.ambient[1], 
		sceneobj->material.ambient[2]);

	fprintf( out, "\n\tDiffuse: %.2lf %.2lf %.2lf",
		sceneobj->material.diffuse[0],
		sceneobj->material.diffuse[1],
		sceneobj->material.diffuse[2]);

	fprintf( out, "\n\tSpecular: %.2lf %.2lf %.2lf\n",
		sceneobj->material.specular[0],
		sceneobj->material.specular[1],
		sceneobj->material.specular[1]);
}

