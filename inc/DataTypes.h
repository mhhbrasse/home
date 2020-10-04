#ifndef RenderTypes_h
#define RenderTypes_h

struct Vertex2 { float x; float y; };
typedef struct Vertex2 Vertex2; 

struct Vertex3 { float x; float y; float z; };
typedef struct Vertex3 Vertex3; 

struct Normal3 { float nx; float ny; float nz; } ;
typedef struct Normal3 Normal3; 

struct Faces { int v0; int v1; int v2; int k0; int k1; int k2; } ;
typedef struct Faces Faces; 

struct _TModel3D 
{ 
	int objectID;		// sequential number 
	char* modelName;	// filename of model
	int numberFaces;	// #faces (must be triangles)
	int numberVertices; // #vertices
	int CCW;			// faces are constructed counterclockwise == (CCW = 1)
	float zoomFactor;	// zoom factor, negative means auto zoom
	int index;			// -1 == first vertex index = 0, 0 == first vertex index = 1,
	bool FiletypeOBJ;	// true == 'v' prefix for vertices in file
	bool verticesAreUnique; // true == vertices have unique positions
};
typedef struct _TModel3D TModel3D;

#endif
