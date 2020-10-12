#pragma warning(disable:4996)

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <Windows.h>
#include "DataTypes.h"
#include "TRender.h"
#include "TModel.h"
#include "Math3D.h"
#include <stdlib.h> // calloc

#define FLOAT_MIN (-100000.0f)
#define SQR(x) ((x)*(x))

TRender::TRender(int width, int height)
{
	// setup buffer 0
	gBuffer0 = (COLORREF*) calloc(width*height, sizeof(COLORREF));
	zBuffer0 = (float*) calloc(width*height, sizeof(float));
	for (int i=0; i<width*height; i++) zBuffer0[i]=FLOAT_MIN;
	
	// setup buffer 1
	gBuffer1 = (COLORREF*) calloc(width*height, sizeof(COLORREF));
	zBuffer1 = (float*) calloc(width*height, sizeof(float));
	for (int i=0; i<width*height; i++) zBuffer1[i]=FLOAT_MIN;

	// initialize to use buffer 0
	gBuffer = gBuffer0;
	zBuffer = zBuffer0;

	// store frame dimensions
	frameWidth = width;
	frameHeight = height;

	// set default CameraView matrix
	vec3_t from = vec3(0,0,10);
	vec3_t to = vec3(0,0,0);
	vec3_t up = vec3(0,1,0);
	setCamera(from,to,up,false,false);
}

TRender::~TRender()
{
	if (gBuffer) free(gBuffer);
	if (zBuffer) free(zBuffer);
	gBuffer = NULL;
	zBuffer = NULL;
}

void TRender::plot(int x, int y, int r, int g, int b) 
{
	//bool result = false;
	if (0<=x && x<frameWidth) 
	{
		if (0<=y && y<frameHeight) 
		{
			unsigned char ucolor_r = (unsigned char ) r;
			unsigned char ucolor_g = (unsigned char ) g;
			unsigned char ucolor_b = (unsigned char ) b;
			gBuffer[(frameHeight-y-1)*frameWidth + x] = ucolor_r<<16 | ucolor_g<<8 | ucolor_b; 
			//result = true;
		}
	}	
	//return result;
}


void TRender::clearBuffers()
{
	for (int i=0; i<frameWidth*frameHeight; i++) 
	{
			gBuffer[i]=DWORD(0);
			zBuffer[i]=FLOAT_MIN;
	}
}

void TRender::swapBuffers()
{
	if (gBuffer == gBuffer0) { gBuffer = gBuffer1; } else { gBuffer = gBuffer0; }
	if (zBuffer == zBuffer0) { zBuffer = zBuffer1; } else { zBuffer = zBuffer0; }
}


void TRender::transformModelX( TModel &model, float angle )
{
	model.transformModelX( angle );
	return;
}

void TRender::transformModelY( TModel &model, float angle )
{
	model.transformModelY( angle );
	return;
}

void TRender::transformModelZ( TModel &model, float angle )
{
	model.transformModelZ( angle );
	return;
}



void TRender::renderModel(TModel& model, int px, int py, int qx, int qy)
{
	int numberFaces = model.getNumberFaces();
	int numberVertices = model.getNumberVertices();
	Faces* faces = model.getFaces();
	Vertex3* vertices = model.getVertices();
	Normal3* normals = model.getNormals() ;
	if (px<0 || py<0 || qx<0 || qy<0)
	{
		px = 0;
		py = 0;
		qx = frameWidth;
		qy = frameHeight;
	}
	renderModel( numberFaces,  numberVertices, faces, vertices, normals, px, py, qx, qy);
}

void TRender::setCamera(vec3_t from, vec3_t to, vec3_t up, bool usePerspective, bool info)
{
	mat4_t perspectiveMatrix=m4_identity();
	mat4_t transleer = m4_translation(vec3(0.0,0.0,0.0));
	mCameraView = m4_look_at(from,to,up);
	mCameraDistance = (float) (sqrt(SQR(from.x-to.x)+SQR(from.y-to.y)+SQR(from.z-to.z)));
	if (usePerspective && info)
	{
		printf("[Info][TRender::setCamera]: automatic Field Of View is %.2f degrees\n", atan(2.0f/mCameraDistance)*180.0/M_PI);
	}
	// set auto perspective transform 
	if (usePerspective)
	{
		perspectiveMatrix = m4_perspective((float) (atanf(2.0f/mCameraDistance)*180.0f/M_PI), 1.0f, mCameraDistance-1.0f, mCameraDistance+1.0f);
		// for perspective projection, move all z-values from near/far of [1..-1] to [0..-2], aligned with ortho-projection (see scanline()).
		transleer = m4_translation(vec3(0.0,0.0,-1.0));
	}
	mPerspectiveCameraView = m4_mul(transleer,m4_mul(perspectiveMatrix,mCameraView));
	// due to perspective transform definition in Math3D.h for near/far plane, use negated z
	if (usePerspective)
	{
		mPerspectiveCameraView.m02 = -mPerspectiveCameraView.m02;
		mPerspectiveCameraView.m12 = -mPerspectiveCameraView.m12;
		mPerspectiveCameraView.m22 = -mPerspectiveCameraView.m22;
		//mPerspectiveCameraView.m32 = 1.0f;
	}
	
}

void TRender::renderModel(int numberFaces, int numberVertices, Faces* faces, Vertex3* verticesIn, Normal3* normalsIn, int px, int py, int qx, int qy)
{
	int i, color1,color2,color3;
	//
	Vertex3 *vertices = (Vertex3*) calloc(numberVertices, sizeof(Vertex3));
	Normal3 *normals  = (Normal3*) calloc(numberVertices, sizeof(Normal3));
	Normal3 aLight;
	// default light 
	Normal3 aLightWorld;
	aLightWorld.nx = 0.0f;
	aLightWorld.ny = 0.0f;
	aLightWorld.nz = 1.0f;
	// Convert Vertices and Normals to cameraView
	for (i=0; i<numberVertices; i++)
	{
		Vertex3 p0 = verticesIn[i];
		vec3_t pp = vec3(p0.x,p0.y,p0.z);
		vec3_t rr = m4_mul_pos(mPerspectiveCameraView, pp);
		vertices[i].x = rr.x; vertices[i].y = rr.y; vertices[i].z=rr.z; 
		//printf("z is %f\n", rr.z);

		Normal3 n0 = normalsIn[i];
		vec3_t nn = vec3(n0.nx,n0.ny,n0.nz);
		rr = m4_mul_dir(mCameraView, nn);
		normals[i].nx = rr.x; normals[i].ny = rr.y; normals[i].nz=rr.z;
	}
	// Convert aLightWorld to aLight(Camera)
	vec3_t lightWorld = vec3(aLightWorld.nx,aLightWorld.ny,aLightWorld.nz);
	vec3_t lightCamera = m4_mul_dir(mCameraView, lightWorld);
	aLight.nx = lightCamera.x; aLight.ny = lightCamera.y; aLight.nz = lightCamera.z; 
	//
	for (i=0; i<numberFaces; i++)
	{
		float light;
		light = aLight.nx*normals[faces[i].v0].nx + aLight.ny*normals[faces[i].v0].ny + aLight.nz*normals[faces[i].v0].nz;
		if (light<0.0f) light=0.0f;
		if (light>1.0f) light=1.0f;				
		//color1 = (int) floor(0.0f+light*(255.0f-0.0f));
		color1 = (int) floor(10.0f+light*(255.0f-10.0f));

		light = aLight.nx*normals[faces[i].v1].nx + aLight.ny*normals[faces[i].v1].ny + aLight.nz*normals[faces[i].v1].nz;
		if (light<0.0f) light=0.0f;
		if (light>1.0f) light=1.0f;				
		//color2 = (int) floor(0.0f+light*(255.0f-0.0f));
		color2 = (int) floor(10.0f+light*(255.0f-10.0f));

		light = aLight.nx*normals[faces[i].v2].nx + aLight.ny*normals[faces[i].v2].ny + aLight.nz*normals[faces[i].v2].nz;
		if (light<0.0f) light=0.0f;
		if (light>1.0f) light=1.0f;				
		//color3 = (int) floor(0.0f+light*(255.0f-0.0f));
		color3 = (int) floor(10.0f+light*(255.0f-10.0f));
						
		if (color1>0 || color2>0 || color3>0)
		{
			//color = floor(0.0f+light*(255.0f-0.0f));
			RenderTriangle(
				vertices[faces[i].v0], vertices[faces[i].v1], vertices[faces[i].v2],
				normals[faces[i].v0], normals[faces[i].v1], normals[faces[i].v2],
				//normals[i], normals[i], normals[i],
				color1,color2,color3, faces[i].k0, faces[i].k1, faces[i].k2,
				px, py, qx, qy);
		}
	}	
	//
	if (vertices!=NULL) free(vertices);
	if (normals!=NULL) free(normals);
}

				
void TRender::saveScene()
{
	char filename[64];
	char rgb_r;
	char rgb_g;
	char rgb_b;

	FILE* f;	
	sprintf(filename, "scene_%d_%d.ppm", frameWidth, frameHeight);
	if ((f=fopen(filename, "w"))!=NULL) 	
	{
		fprintf(f, "P6 %d %d 255 ", frameWidth, frameHeight);
		fclose(f);
		if ((f=fopen(filename, "ab"))!=NULL) 
		{
			for (int j=0;j<frameHeight;j++)
			{
				for (int i=0; i<frameWidth; i++)
				{
					COLORREF v = (COLORREF) (gBuffer[j*frameWidth+i]);
					rgb_r = GetRValue(v);
					rgb_g = GetGValue(v);
					rgb_b = GetBValue(v);
					fwrite((char*)&rgb_b, 1, 1, f);
					fwrite((char*)&rgb_g, 1, 1, f);
					fwrite((char*)&rgb_r, 1, 1, f);
				}
			}
			fclose(f);
		}
	}
}


void TRender::scanline(int y, int x0, int x1, float z0, float z1, Normal3 n0, Normal3 n1,  int color1, int color2, int k0, int k1, int k2,
					int px, int py, int qx, int qy)
{
	int i;
	Normal3 ni;
	float light;
	float eyex = 0.0f; 
	float eyey = 0.0f; 
	float eyez = 1.0f;

	// fill scanline (x0,y) .. (x1,y) /\ x0!=x1		
	if (x1 == x0) return;

	for (i=x0; i<x1; i++)
	{
		float len;
		float factor = ((float) (i-x0) / (float) (x1-x0));
		float z = (x1>x0 ? z0 + (z1-z0)*factor : z0);
		float color = (x1>x0 ? color1 + (color2-color1)*factor : color1);
		ni.nx = n0.nx + ((n1.nx-n0.nx)) * factor;
		ni.ny = n0.ny + ((n1.ny-n0.ny)) * factor;
		ni.nz = n0.nz + ((n1.nz-n0.nz)) * factor;
		len = sqrt(ni.nx*ni.nx + ni.ny*ni.ny + ni.nz*ni.nz);
		light = 0.0;
		if (len > 0.0)
		{
			light = eyex*ni.nx/len + eyey*ni.ny/len + eyez*ni.nz/len;
			if (light>1.0f) light=1.0f;
		}
		
		//printf("px, py, qx, qy = %d %d %d %d\n", px,py,qx,qy);
		if (px<=i && i<qx && py<=y && y<=qy && z<=0.0) 
		{
			// within clip area
			if (light > 0.0f)
			{			
				if (z>zBuffer[ (frameHeight-y-1)*frameWidth + i ]) 
				{
					if (color<0.0) color = 0.0f;
					if (color>255.0) color = 255.0f;
					unsigned char ucolor_r = (unsigned char) (color*k0/255.0f);
					unsigned char ucolor_g = (unsigned char) (color*k1/255.0f);
					unsigned char ucolor_b = (unsigned char) (color*k2/255.0f);
				
					zBuffer[ (frameHeight-y-1)*frameWidth + i ] = z;
					// changed code to be compatible with Windows framebuffer
					//gBuffer[(frameHeight-y-1)*frameWidth*3 + i*3] = ucolor;
					//gBuffer[(frameHeight-y-1)*frameWidth*3 + i*3 + 1] = ucolor;
					//gBuffer[(frameHeight-y-1)*frameWidth*3 + i*3 + 2] = ucolor;
					//gBuffer[(frameHeight-y-1)*frameWidth + i] = RGB(ucolor_b, ucolor_g, ucolor_r);
					gBuffer[(frameHeight-y-1)*frameWidth + i] = ucolor_r<<16 | ucolor_g<<8 | ucolor_b; 
				}
			}
		}
	}
}

void TRender::RenderTriangle(Vertex3 v0, Vertex3 v1, Vertex3 v2, Normal3 n0, Normal3 n1, Normal3 n2, int color0, int color1, int color2, int k0, int k1, int k2,
							 int px, int py, int qx, int qy)
{
	int v0_x,v0_y;
	int v1_x,v1_y;
	int v2_x,v2_y;
	int dx01,dx02;
	int dy01,dy02;
	int ys;
	int e01,e02;
	int x01,x02;
	float z01=0.0f,z02=0.0f;
	Normal3 n01;
	Normal3 n02;
	int color01;
	int color02;


	// sort vertices of face (in non-screen coordinates :()
	if (v1.y > v2.y) { Vertex3 tmp = v1; v1 = v2; v2 = tmp; Normal3 tmp1 = n1; n1 = n2; n2 = tmp1; int tmp2 = color1; color1 = color2; color2 = tmp2; } 
	if (v0.y > v1.y) { Vertex3 tmp = v0; v0 = v1; v1 = tmp; Normal3 tmp1 = n0; n0 = n1; n1 = tmp1; int tmp2 = color0; color0 = color1; color1 = tmp2; } 
	if (v1.y > v2.y) { Vertex3 tmp = v1; v1 = v2; v2 = tmp; Normal3 tmp1 = n1; n1 = n2; n2 = tmp1; int tmp2 = color1; color1 = color2; color2 = tmp2; } 
	// { v0_y <= v1_y <= v2_y }

	// scale/translate to screen coordinates	
	v0_x = toScreen(v0.x);
	v0_y = toScreen(v0.y);	
	v1_x = toScreen(v1.x);
	v1_y = toScreen(v1.y);	
	v2_x = toScreen(v2.x);
	v2_y = toScreen(v2.y);
	// { v0_y <= v1_y <= v2_y }

	if ( v2_y < py || v0_y >qy) return; // added 10-10-2020
	
	// { v0_y <= v1_y <= v2_y }
	ys = v0_y;
	e01 = 0;  
	e02 = 0;
	x01 = v0_x;
	x02 = v0_x;
	dx01 = v1_x - v0_x;
	dy01 = v1_y - v0_y;
	dx02 = v2_x - v0_x;	
	dy02 = v2_y - v0_y;
	z01 = v0.z;
	z02 = v0.z;
	n01 = n0;
	n02 = n0;
	color01 = color0;
	color02 = color0;
	while (ys<v1_y) {
		float factor01;
		float factor02;

		// render (x,y)::v0_y <= y < v1_y for line (0,1)
		// render (x,y)::v0_y <= y < v1_y for line (0,2)
		
		if (x01<=x02) scanline(ys,x01,x02,z01,z02,n01,n02,color01,color02,k0,k1,k2,px,py,qx,qy);
		else if (x01>x02) scanline(ys,x02,x01,z02,z01,n02,n01,color02,color01,k0,k1,k2,px,py,qx,qy);
		//e01 = e01 - dx01;
		//e02 = e02 - dx02;
		//while (e01 + dy01 <= 0) { e01 += dy01; x01++; }
		//while (e01 > 0) { e01 -= dy01; x01--; }
		//while (e02 + dy02 <= 0) { e02 += dy02; x02++; }
		//while (e02 > 0) { e02 -= dy02; x02--; }						
		e01 -= 2*dx01;
		e02 -= 2*dx02;
		while (e01 < -dy01) { e01 += 2*dy01; x01++; }
		while (e01 >= dy01) { e01 -= 2*dy01; x01--; }
		while (e02 < -dy02) { e02 += 2*dy02; x02++; }
		while (e02 >= dy02) { e02 -= 2*dy02; x02--; }

		factor01 = (float) (ys+1-v0_y) / (float) (v1_y-v0_y);
		factor02 = (float) (ys+1-v0_y) / (float) (v2_y-v0_y);

		z01 = (float) v0.z + ((float) (v1.z-v0.z)) * factor01;
		z02 = (float) v0.z + ((float) (v2.z-v0.z)) * factor02;		

		n01.nx = (float) n0.nx + ((float) (n1.nx-n0.nx)) * factor01;
		n01.ny = (float) n0.ny + ((float) (n1.ny-n0.ny)) * factor01;
		n01.nz = (float) n0.nz + ((float) (n1.nz-n0.nz)) * factor01;
		{
			float len;
			len = sqrt(n01.nx*n01.nx + n01.ny*n01.ny + n01.nz*n01.nz);
			if (len<=0.0) len=1.0;
			n01.nx /= len; n01.ny /= len; n01.nz /= len;
		}

		n02.nx = (float) n0.nx + ((float) (n2.nx-n0.nx)) * factor02;
		n02.ny = (float) n0.ny + ((float) (n2.ny-n0.ny)) * factor02;
		n02.nz = (float) n0.nz + ((float) (n2.nz-n0.nz)) * factor02;
		{
			float len;
			len = sqrt(n02.nx*n02.nx + n02.ny*n02.ny + n02.nz*n02.nz);
			if (len<=0.0) len=1.0;
			n02.nx /= len; n02.ny /= len; n02.nz /= len;
		}

		color01 = (int) ((float) color0 + ((float) (color1-color0)) * factor01);
		color02 = (int) ((float) color0 + ((float) (color2-color0)) * factor02);

		ys++;
	}
	//render (x,y)::v1_y <= y < v2_y 
	dx01 = v2_x - v1_x;
	dy01 = v2_y - v1_y;
	x01 = v1_x;
	e01 = 0;
	z01 = v1.z;
	n01 = n1;
	color01 = color1;
	while (ys<v2_y) {
		float factor01;
		float factor02;
		// render (x,y)::v1_y <= y < v2_y for line (1,2)
		// render (x,y)::v1_y <= y < v2_y for line (0,2)
		
		if (x01<=x02) scanline(ys,x01,x02,z01,z02,n01,n02,color01,color02,k0,k1,k2,px,py,qx,qy);
		else if (x01>x02) scanline(ys,x02,x01,z02,z01,n02,n01,color02,color01,k0,k1,k2,px,py,qx,qy);
		//e01 = e01 - dx01;
		//e02 = e02 - dx02;
		//while (e01 + dy01 <= 0) { e01 += dy01; x01++; }
		//while (e01 > 0) { e01 -= dy01; x01--; }
		//while (e02 + dy02 <= 0) { e02 += dy02; x02++; }
		//while (e02 > 0) { e02 -= dy02; x02--; }
		e01 -= 2*dx01;
		e02 -= 2*dx02;		
		while (e01 < -dy01) { e01 += 2*dy01; x01++; }
		while (e01 >= dy01) { e01 -= 2*dy01; x01--; }
		while (e02 < -dy02) { e02 += 2*dy02; x02++; }
		while (e02 >= dy02) { e02 -= 2*dy02; x02--; }

		factor01 = (float) (ys+1-v1_y) / (float) (v2_y-v1_y);
		factor02 = (float) (ys+1-v0_y) / (float) (v2_y-v0_y);

		z01 = (float) v1.z + ((float) (v2.z-v1.z)) * factor01;
		z02 = (float) v0.z + ((float) (v2.z-v0.z)) * factor02;

		n01.nx = (float) n1.nx + ((float) (n2.nx-n1.nx)) * factor01;
		n01.ny = (float) n1.ny + ((float) (n2.ny-n1.ny)) * factor01;
		n01.nz = (float) n1.nz + ((float) (n2.nz-n1.nz)) * factor01;
		{
			float len;
			len = sqrt(n01.nx*n01.nx + n01.ny*n01.ny + n01.nz*n01.nz);
			if (len<=0.0) len=1.0;
			n01.nx /= len; n01.ny /= len; n01.nz /= len;
		}
		n02.nx = (float) n0.nx + ((float) (n2.nx-n0.nx)) * factor02;
		n02.ny = (float) n0.ny + ((float) (n2.ny-n0.ny)) * factor02;
		n02.nz = (float) n0.nz + ((float) (n2.nz-n0.nz)) * factor02;
		{
			float len;
			len = sqrt(n02.nx*n02.nx + n02.ny*n02.ny + n02.nz*n02.nz);
			if (len<=0.0) len=1.0;
			n02.nx /= len; n02.ny /= len; n02.nz /= len;
		}
		color01 = (int) ((float) color1 + ((float) (color2-color1)) * factor01);
		color02 = (int) ((float) color0 + ((float) (color2-color0)) * factor02);

		ys++;
	}

	if (v1_y == v2_y)
	{
		if (v0_y == v1_y)
		{
			x01 = v0_x;
			x02 = v1_x;
			z01 = (float) v0.z;
			z02 = (float) v1.z;
			n01 = n0;
			n02 = n1;
			color01 = color0;
			color02 = color1;

			if (x01<=x02) scanline(ys,x01,x02,z01,z02,n01,n02,color01,color02,k0,k1,k2,px,py,qx,qy);
			else if (x01>x02) scanline(ys,x02,x01,z02,z01,n02,n01,color02,color01,k0,k1,k2,px,py,qx,qy);
		
			x01 = v1_x;
			x02 = v2_x;
			z01 = (float) v1.z;
			z02 = (float) v2.z;
			n01 = n1;
			n02 = n2;
			color01 = color1;
			color02 = color2;


			if (x01<=x02) scanline(ys,x01,x02,z01,z02,n01,n02,color01,color02,k0,k1,k2,px,py,qx,qy);
			else if (x01>x02) scanline(ys,x02,x01,z02,z01,n02,n01,color02,color01,k0,k1,k2,px,py,qx,qy);
		
		}
		else
		{
			z01 = (float) v1.z;
			z02 = (float) v2.z;
			n01 = n1;
			n02 = n2;
			color01 = color1;
			color02 = color2;


			if (x01<=x02) scanline(ys,x01,x02,z01,z02,n01,n02,color01,color02,k0,k1,k2,px,py,qx,qy);
			else if (x01>x02) scanline(ys,x02,x01,z02,z01,n02,n01,color02,color01,k0,k1,k2,px,py,qx,qy);
		
		}
	}	
}

#define MIN(a,b) ((b)>(a) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

int TRender::toScreen(float p)
{
		int f = (int) (floor((p+1.0f)*(MIN(frameWidth,frameHeight)/2.0f-0.5f)));
		f = MIN(MAX(f,0),MIN(frameWidth,frameHeight)-1);
		return f;
}
