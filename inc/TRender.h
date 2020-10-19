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
		void setCamera(vec3_t from, vec3_t to, vec3_t up, bool usePerspective=false, bool info=false);
		void setLightNormal(vec3_t direction);
		void display( TDisplay aDisplay) {	aDisplay.display( getBuffer(), frameWidth, frameHeight ); }
		void saveScene();
		void saveSceneJPG();
		void clearBuffers(); // clear current active color buffer and Z buffer
		void swapBuffers();  // swap active color buffers and Z buffers 
		COLORREF* getBuffer() { return gBuffer; } 
		void plot(int x, int y, int r, int g, int b);
		
		void transformModel( TModel &model, float angle, vec3_t axis, vec3_t position = vec3(0.0f,0.0f,0.0f), float scale = 1.0f );

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
		mat4_t mPerspectiveCameraView;
		float mCameraDistance;

		Normal3 mLightDirection;

		int nrJPG;
};  

#endif