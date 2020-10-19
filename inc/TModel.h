#ifndef TModel_h
#define TModel_h

#include "DataTypes.h"
#include "TModel.h"
#include "Math3D.h"

class TModel 
{  

public:  
		TModel();		
		~TModel();

		Faces* getFaces() { return faces; } ;
		Vertex3* getVertices() { return vertices; };
		Normal3* getNormals() { return normals; };
		int getNumberVertices() { return numberVertices; };		
		int getNumberFaces() { return numberFaces; };

		void ImportModel(TModel3D &modelObject);

		void transformModel( float angle, vec3_t axis, vec3_t position = vec3(0.0f,0.0f,0.0f), float scale = 1.0f );
		void scaleModel();

private:
		Vertex3* vertices;
		Normal3* normals;
		Vertex3* verticesIn;
		Normal3* normalsIn;
		Faces* faces;
		int numberVertices;
		int numberFaces;
		float zoomFactor;
};

#endif
