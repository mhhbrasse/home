///////////////////////////////////////////////////////////////////////////////
// Pipeline.cpp
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include "TRender.h"
#include "TModel.h"
#include "TDisplay.h"


//const int   FRAME_WIDTH    = 4096;
//const int   FRAME_HEIGHT   = 2048;

const int   FRAME_WIDTH    = 1024;
const int   FRAME_HEIGHT   = 1024;

const int   DISPLAY_WIDTH    = 512;
const int   DISPLAY_HEIGHT   = 512;

// see DataTypes.h 
TModel3D objectData[] = 
{ 
	{0, "../Models/airplane.off", 7446, 3745, +1, -41.20f, 0, false, true },
	{1, "../Models/teapot.off", 6320, 3644, +1, -6.43f, 0, false, false },
	{2 ,"../Models/iss.small.obj", 6643, 3309 , +1, -3.0f, -1, false, false },
	{3, "../Models/CSHP_DV_130_01_______00200.obj", 104192, 52098, +1, -5.06f, -1, true, true },
	{4, "../Models/17243_Voyager_1_okt2020.obj", 46244, 23212, +1, -10.0f, -1, true, false},
	//{5, "../Models/alfa147.off", 97214, 53012, +1, -164.16f, 0, false, false },
	//{6, "../Models/voyager2_out.obj", 110114, 55446, +1, -320.0f, -1, false, true },
	//{7, "../Models/New_Horizons.obj", (260871-99985), 99985, +1, -6.75f, -1, true, true }, 
	//{8, "../Models/ISS_2016.out.obj", 2338906, 1322565, +1, -4268.0f, -1, false, true },	
	{-1, "", 0, 0, 0, 0.0f, 0, true, true }
};

int getNumberObjects(TModel3D objectData[])
{
	int n=0;
	while (objectData[n].objectID >= 0) n++;
	return n;
}

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int ID = -1;
	int frameWidth = FRAME_WIDTH;
	int frameHeight = FRAME_HEIGHT;
	int displayWidth = DISPLAY_WIDTH;
	int displayHeight = DISPLAY_HEIGHT;
	int numberModels = getNumberObjects(objectData);
	bool usePerspective = true;
	bool useLogging = true;

	// IO console interfacing
	if (argc>1) 
	{
		int anID = atoi(argv[1]);
		if (anID>=0 && anID<numberModels) ID = anID;
	}
	if (ID<0) 
	{
		printf("Usage: %s [objectID], where optional argument is the objectID [0..%d]\n", argv[0], numberModels-1);
		return -1;
	}
	printf("Selected ID is %d, model: %s\n", ID, objectData[ID].modelName);

	// Create Display
	TDisplay myDisplay( displayWidth, displayHeight );
	
	// Load the Model from disk
	TModel myModel;
	myModel.ImportModel( objectData[ID] );
	
	// Create and Initalize the Renderer 
	TRender myRenderer( frameWidth, frameHeight );
	myRenderer.setCamera(vec3(0,0,2), vec3(0,0,0), vec3(0,1,0), usePerspective, useLogging);
	myRenderer.setLightNormal(vec3(0.0,0.0,1.0));

	// Render the Model through successive animation steps
	for (int angle=0; angle<=360; angle++)
	{
		myRenderer.swapBuffers();		
		myRenderer.clearBuffers();
		myRenderer.transformModelX( myModel, (float) angle );
		myRenderer.renderModel( myModel );
		//myRenderer.renderModel( myModel, 0, frameWidth/2, frameWidth, frameHeight );
		//myRenderer.renderModel( myModel, 0, 0, frameWidth, frameHeight/2 );
		myRenderer.display( myDisplay );
	}
	// Render the Model through successive animation steps
	for (int angle=0; angle<=360; angle++)
	{
		myRenderer.swapBuffers();		
		myRenderer.clearBuffers();
		myRenderer.transformModelY( myModel, (float) angle );
		myRenderer.renderModel( myModel );
		//myRenderer.renderModel( myModel, 0, frameWidth/2, frameWidth, frameHeight );
		//myRenderer.renderModel( myModel, 0, 0, frameWidth, frameHeight/2 );
		myRenderer.display( myDisplay );
	}
	// Render the Model through successive animation steps
	for (int angle=0; angle<=360; angle++)
	{
		myRenderer.swapBuffers();
		myRenderer.clearBuffers();
		myRenderer.transformModelZ( myModel, (float) angle );
		myRenderer.renderModel( myModel );
		//myRenderer.renderModel( myModel, 0, frameWidth/2, frameWidth, frameHeight );
		//myRenderer.renderModel( myModel, 0, 0, frameWidth, frameHeight/2 );
		myRenderer.display( myDisplay );		
	}
	// Write last rendered scene to disk
	myRenderer.saveScene();
	return 0;
}
