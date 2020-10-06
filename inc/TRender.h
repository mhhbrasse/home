#ifndef TRender_h
#define TRender_h

#include <Windows.h>
#include "DataTypes.h"
#include "TModel.h"
#include "TDisplay.h"


class TRender
{  

public:  
		TRender(int width, int height);
		~TRender();
		void renderModel(TModel& model);
		void renderModel(int numberFaces, int numberVertices, Faces* faces, Vertex3* vertices, Normal3* normals);
		void display( TDisplay aDisplay) {	aDisplay.display( getBuffer(), frameWidth, frameHeight ); }
		void saveScene();
		void clearBuffers(); // clear current active color buffer and Z buffer
		void swapBuffers();  // swap active color bufers and Z buffers 
		COLORREF* getBuffer() { return gBuffer; } 
		COLORREF getBuffer(int i, int j) { return gBuffer[j*frameWidth+i]; } 
			
		void transformModelX( TModel &model, float angle );
		void transformModelY( TModel &model, float angle );
		void transformModelZ( TModel &model, float angle );

		void plot(int x, int y, int r, int g, int b);
		void drawCircle( int x0,int y0,int radius, int r,int g, int b );
		void drawLine(int xb, int yb, int xe, int ye,int r,int g, int b );

private:
		void scanline(int y, int x0, int x1, float z0, float z1, Normal3 n0, Normal3 n1,  int color1, int color2, int k0, int k1, int k2);
		void RenderTriangle(Vertex3 v0, Vertex3 v1, Vertex3 v2, Normal3 n0, Normal3 n1, Normal3 n2, int color0, int color1, int color2, int k0, int k1, int k2);
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
};  

#endif