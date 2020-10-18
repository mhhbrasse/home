// Tmodel class reads modified OBJ formatted files
// vertices: { "vertex_x vertex_y vertex_z" } +
// faces:    { "3 vertexid vertexid vertexid" | "c colorR colorG colorB" } 
// normals are computed automatically, based on counter-clockwise faces
#pragma warning(disable:4996)

#include "TModel.h"
#include "DataTypes.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "TModel.h"
#include "Math3D.h"

char buf[256]; // used for scan buffer

#define MIN(a,b) ((b)>(a) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

#define FLOAT_MAX ( 1000000.0f)
#define FLOAT_MIN (-1000000.0f)

TModel::TModel()
{
	numberVertices = 0;
	numberFaces = 0;
	faces = NULL;
	vertices = NULL;
	normals = NULL;
	verticesIn = NULL;
	normalsIn = NULL;
}

TModel::~TModel()
{
	free(vertices);
	free(normals);
	free(faces);
	free(verticesIn);
	free(normalsIn);
}

// load scene data
void TModel::ImportModel(TModel3D& modelObject)
{
	int k0=228, k1=185, k2=143; //  is initial value
	int N = modelObject.numberVertices;
	int F = modelObject.numberFaces;
	char* filename = modelObject.modelName;
	int CCW = modelObject.CCW;
	int INDEX = modelObject.index;
	bool isOBJ = modelObject.FiletypeOBJ;
	bool verticesAreUnique = modelObject.verticesAreUnique;
	float modelZoomFactor = modelObject.zoomFactor;

	int i,j;
	FILE *f;
	int *Translate = (int*) calloc(N, sizeof(int));
	
	if (vertices !=NULL) free(vertices);
	vertices = (Vertex3*) calloc (N, sizeof(Vertex3));
	
	if (verticesIn !=NULL) free(verticesIn);
	verticesIn = (Vertex3*) calloc (N, sizeof(Vertex3));
	
	if (normals !=NULL) free(normals);
	normals = (Normal3*) calloc (N, sizeof(Normal3));

	if (normalsIn !=NULL) free(normalsIn);
	normalsIn = (Normal3*) calloc (N, sizeof(Normal3));

	if (faces !=NULL) free(faces);
	faces = (Faces*) calloc (F, sizeof(Faces)); // also initialises k components

	if ((f=fopen(filename,"r"))==NULL)	
	{ 
		printf("[LoadScene] model not found on disk\n"); 
		return; 
	}

	numberVertices = N;
	numberFaces = F;

	for (i=0;i<numberVertices;i++)
	{
		float x,y,z;
		if (isOBJ)
		{
			fscanf(f,"%s %f %f %f\n",&buf[0], &x,&y,&z);
			//printf("V: %f %f %f\n", x,y,z);
		}
		else
			fscanf(f,"%f %f %f\n",&x,&y,&z);
		Translate[i]=i;	
		if (!verticesAreUnique)
		{
			j=0;		
			// avoid double vertices with equal positions, as this interferes with the per-vertex normal computation
			while (j<i)
			{
				if ((vertices[j].x==x) && (vertices[j].y==y) && (vertices[j].z==z))
				{					
					Translate[i]=j;
					j=i;
				}
				else
				{
					j++;
				}
			}
		}		
		vertices[i].x=x;
		vertices[i].y=y;
		vertices[i].z=z;
	}

	for (i=0;i<numberVertices;i++)
	{
		normals[i].nx=0.0f;
		normals[i].ny=0.0f;
		normals[i].nz=0.0f;
	}

	// read faces and compute face and vertex normals
	i = 0; 
	while (i < numberFaces)
	{
		int v0,v1,v2;
		
		float len;
		Vertex3 p0,p1,p2,a,b,c;
		Normal3 norm;

		fscanf(f,"%s %d %d %d\n",&buf[0], &v0,&v1,&v2);
		//printf("face: %d %d %d\n", v0,v1,v2);
		if (!strcmp(buf,"c"))
		{
			// color for next Faces can be set via input file (experimental)
			// format: c colorR colorG colorB
			k0 = v0; k1 = v1; k2 = v2; 
		}
		else // "3 vertixId vertexID vertexID"
		{
			faces[i].v0=Translate[v0+INDEX];
			faces[i].v1=Translate[v1+INDEX];
			faces[i].v2=Translate[v2+INDEX];

			faces[i].k0 = k0;
			faces[i].k1 = k1;
			faces[i].k2 = k2;

			p0=vertices[faces[i].v0];
			p1=vertices[faces[i].v1];
			p2=vertices[faces[i].v2];

			a.x=p1.x-p0.x;
			a.y=p1.y-p0.y;
			a.z=p1.z-p0.z;
			b.x=p2.x-p0.x;
			b.y=p2.y-p0.y;
			b.z=p2.z-p0.z;
			c.x=a.y*b.z-a.z*b.y;
			c.y=a.z*b.x-a.x*b.z;
			c.z=a.x*b.y-a.y*b.x;
			len=(float) sqrt(c.x*c.x+c.y*c.y+c.z*c.z);
			if (len<=0.000000000001f) // must be smallest
			{
				norm.nx=0.0f;
				norm.ny=0.0f;
				norm.nz=0.0f;
			}
			else
			{
				norm.nx=CCW*c.x/len;// CCW == counterclockwise (-1 or 1)
				norm.ny=CCW*c.y/len;// CCW == counterclockwise (-1 or 1)
				norm.nz=CCW*c.z/len;// CCW == counterclockwise (-1 or 1)
			}

			// add normal to vertex belonging to the face
			normals[faces[i].v0].nx += norm.nx;
			normals[faces[i].v0].ny += norm.ny;
			normals[faces[i].v0].nz += norm.nz;
			
			// add normal to vertex belonging to the face
			normals[faces[i].v1].nx += norm.nx;
			normals[faces[i].v1].ny += norm.ny;
			normals[faces[i].v1].nz += norm.nz;

			// add normal to vertex belonging to the face
			normals[faces[i].v2].nx += norm.nx;
			normals[faces[i].v2].ny += norm.ny;
			normals[faces[i].v2].nz += norm.nz;

			// only increase for real face 
			i++;
		}
	}		

	// normalise the vertex normal vectors
	// a number of normals entries will not be used, as they will not be referred to by the faces (due to Translate). 
	// these are identified by len==0, but will not be used for computations, so they can be set to 0.0
	for (i=0;i<numberVertices;i++)
	{			
		float len = (float) sqrt( normals[i].nx*normals[i].nx+normals[i].ny*normals[i].ny+normals[i].nz*normals[i].nz);			
		if (len > 0.001f)
		{
			normals[i].nx = normals[i].nx/len;
			normals[i].ny = normals[i].ny/len;
			normals[i].nz = normals[i].nz/len;	
		}			
	}
	
	fclose(f);
	free(Translate);

	// copy for future transformations on original positions from loaded Model, and initial values of normals
	// the faces are invariant
	float minx = FLOAT_MAX;
	float miny = FLOAT_MAX;
	float minz = FLOAT_MAX;
	float maxx = FLOAT_MIN;
	float maxy = FLOAT_MIN;
	float maxz = FLOAT_MIN;
	for (i=0;i<numberVertices;i++)
	{
		minx = MIN(minx,vertices[i].x);
		maxx = MAX(maxx,vertices[i].x);

		miny = MIN(miny,vertices[i].y);
		maxy = MAX(maxy,vertices[i].y);

		minz = MIN(minz,vertices[i].z);
		maxz = MAX(maxz,vertices[i].z);
	}		

	if (modelZoomFactor <= 0.0f)
	{
		zoomFactor = (MAX((maxz-minz), MAX((maxx-minx),(maxy-miny))))/1.00f;
		//printf("zoomFactor %f (Xmax-min %f, Ymax-min %f, Zmax-min %f)\n", zoomFactor, maxx-minx, maxy-miny, maxz-minz);
		
	}
	else
	{
		zoomFactor = modelZoomFactor;
	}
	//printf("[TModel][Debug] Zoomfactor is %f\n", zoomFactor);

	// scale the positions of the model, and prepare buffers
	for (i=0;i<numberVertices;i++)
	{
		vertices[i].x=vertices[i].x/zoomFactor;
		vertices[i].y=vertices[i].y/zoomFactor;
		vertices[i].z=vertices[i].z/zoomFactor;

		verticesIn[i].x=vertices[i].x;
		verticesIn[i].y=vertices[i].y;
		verticesIn[i].z=vertices[i].z;

		normalsIn[i].nx=normals[i].nx;
		normalsIn[i].ny=normals[i].ny;
		normalsIn[i].nz=normals[i].nz;
	}		
}

void TModel::scaleModel()
{
	int numberVertices = getNumberVertices();
	Vertex3* vertices = getVertices();
	Normal3* normals = getNormals();
	
	mat4_t mScaleModel = m4_scaling(vec3(1.0f/zoomFactor,1.0f/zoomFactor,1.0f/zoomFactor));
	
	for (int i=0; i<numberVertices; i++)
	{
		vec3_t v = m4_mul_pos ( mScaleModel, vec3(verticesIn[i].x, verticesIn[i].y, verticesIn[i].z));
		vertices[i].x = v.x; vertices[i].y = v.y; vertices[i].z = v.z;
	}
	return;
}

void TModel::transformModelX( float angle )
{
	int numberVertices = getNumberVertices();
	Vertex3* vertices = getVertices();
	Normal3* normals = getNormals();
	
	mat4_t mRotationX = m4_rotation_x((float) (-angle*M_PI/180.0));
	mat4_t mScaleModel = m4_scaling(vec3(1.0f,1.0f,1.0f));
	
	for (int i=0; i<numberVertices; i++)
	{
		vec3_t v = m4_mul_pos ( m4_mul(mRotationX, mScaleModel), (vec3(verticesIn[i].x, verticesIn[i].y, verticesIn[i].z)));
		vertices[i].x = v.x; vertices[i].y = v.y; vertices[i].z = v.z;
		//
		vec3_t n = m4_mul_pos (mRotationX, (vec3(normalsIn[i].nx, normalsIn[i].ny, normalsIn[i].nz)));
		normals[i].nx = n.x; normals[i].ny = n.y; normals[i].nz = n.z;
	}
	return;
}

void TModel::transformModelY( float angle )
{
	int numberVertices = getNumberVertices();
	Vertex3* vertices = getVertices();
	Normal3* normals = getNormals();
	mat4_t mRotationY = m4_rotation_y((float) (-angle*M_PI/180.0));
	mat4_t mScaleModel = m4_scaling(vec3(1.0f,1.0f,1.0f));
	
	for (int i=0; i<numberVertices; i++)
	{
		vec3_t v = m4_mul_pos (m4_mul(mRotationY, mScaleModel), (vec3(verticesIn[i].x, verticesIn[i].y, verticesIn[i].z)));
		vertices[i].x = v.x; vertices[i].y = v.y; vertices[i].z = v.z;
		//
		vec3_t n = m4_mul_pos (mRotationY, (vec3(normalsIn[i].nx, normalsIn[i].ny, normalsIn[i].nz)));
		normals[i].nx = n.x; normals[i].ny = n.y; normals[i].nz = n.z;
	}
	return;
}

void TModel::transformModelZ( float angle )
{
	int numberVertices = getNumberVertices();
	Vertex3* vertices = getVertices();
	Normal3* normals = getNormals();
	mat4_t mRotationZ = m4_rotation_z((float) (-angle*M_PI/180.0));
	mat4_t mScaleModel = m4_scaling(vec3(1.0f,1.0f,1.0f));

	for (int i=0; i<numberVertices; i++)
	{
		vec3_t v = m4_mul_pos (m4_mul(mRotationZ, mScaleModel), (vec3(verticesIn[i].x, verticesIn[i].y, verticesIn[i].z)));
		vertices[i].x = v.x; vertices[i].y = v.y; vertices[i].z = v.z;
		//
		vec3_t n = m4_mul_pos (mRotationZ, (vec3(normalsIn[i].nx, normalsIn[i].ny, normalsIn[i].nz)));
		normals[i].nx = n.x; normals[i].ny = n.y; normals[i].nz = n.z;
	}
	return;
}

