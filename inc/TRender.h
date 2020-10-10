#ifndef TRender_h
#define TRender_h

#include <Windows.h>
#include "DataTypes.h"
#include "TModel.h"
#include "TDisplay.h"
#include "Math3D.h" 

class TRender
{  

public:  
		TRender(int width, int height);
		~TRender();
		void renderModel(TModel& model, int px=-1, int py=-1, int qx=-1, int qy=-1);
		void renderModel(int numberFaces, int numberVertices, Faces* faces, Vertex3* verticesIn, Normal3* normalsIn, int px, int py, int qx, int qy);
		void setCamera(vec3_t from, vec3_t to, vec3_t up);
		void display( TDisplay aDisplay) {	aDisplay.display( getBuffer(), frameWidth, frameHeight ); }
		void saveScene();
		void clearBuffers(); // clear current active color buffer and Z buffer
		void swapBuffers();  // swap active color bufers and Z buffers 
		COLORREF* getBuffer() { return gBuffer; } 
		void plot(int x, int y, int r, int g, int b);
		
		void transformModelX( TModel &model, float angle );
		void transformModelY( TModel &model, float angle );
		void transformModelZ( TModel &model, float angle );

private:
		void scanline(int y, int x0, int x1, float z0, float z1, Normal3 n0, Normal3 n1,  int color1, int color2, int k0, int k1, int k2, int px, int py, int qx, int qy);
		void RenderTriangle(Vertex3 v0, Vertex3 v1, Vertex3 v2, Normal3 n0, Normal3 n1, Normal3 n2, int color0, int color1, int color2, int k0, int k1, int k2,int px, int py, int qx, int qy);
		int toScreen(float p);

private:
		COLORREF* gBuffer;
		float* zBuffer;

		COLORREF* gBuffer0;
		COLORREF* gBuffer1;
		float* zBuffer0;
		float* zBuffer1;

		int frameHeight;
		int frameWidth;

		mat4_t mCameraView;
};  

#endif