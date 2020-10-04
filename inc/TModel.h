#ifndef TModel_h
#define TModel_h

#include "DataTypes.h"
#include "TModel.h"

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

		void transformModelX( float angle );
		void transformModelY( float angle );
		void transformModelZ( float angle );
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
