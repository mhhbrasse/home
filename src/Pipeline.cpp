///////////////////////////////////////////////////////////////////////////////
// Pipeline.cpp
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include "TRender.h"
#include "TModel.h"
#include "TDisplay.h"

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
	{3, "../Models/bol.obj", 1368,2808,+1,-1.0f,-1,true,true},
	{-1, "", 0, 0, 0, 0.0f, 0, true, true }
};

int getNumberObjects(TModel3D objectData[])
{
	int n=0;
	while (objectData[n].objectID >= 0) n++;
	return n;
}

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
	// override for this demo, input value not used
	ID = 0;
	if (ID<0) 
	{
		printf("Usage: %s [objectID], where optional argument is the objectID [0..%d]\n", argv[0], numberModels-1);
		return -1;
	}
	printf("Selected ID is %d, model: %s\n", ID, objectData[ID].modelName);

	// Create Display
	TDisplay myDisplay( displayWidth, displayHeight );
	
	printf("Import Model 1\n");
	// Load the Model from disk (Vehicle)
	TModel myModel1;
	myModel1.ImportModel( objectData[2] );
	
	printf("Import Model 2\n");
	// Load the Model from disk (Sphere)
	TModel myModel2;
	myModel2.ImportModel( objectData[3] );
	
	// Create and Initalize the Renderer 
	TRender myRenderer( frameWidth, frameHeight );
	myRenderer.setCamera( vec3(0,0,2.0), vec3(0,0,0), vec3(0,1,0), usePerspective, useLogging );
	myRenderer.setLightNormal( vec3(0.0,0.0,1.0) );

	// properties and positions used in animation and visualisation
	float orbitRadius  = 2.0f;
	float sphereScale  = 2.5f;
	float vehicleScale = 0.7f;
	float angleSphere = 0.0f;
	float angleVehicle = 90.0f;
	vec3_t positionSphere = vec3(0.0f, 0.0f, -5.0f);
	float vehicleX =  orbitRadius * (float) cos((angleVehicle * M_PI / 180.0)) + positionSphere.x;
	float vehicleY =  positionSphere.y;
	float vehicleZ =  orbitRadius * (float) sin((angleVehicle * M_PI / 180.0)) + positionSphere.z;
	vec3_t positionVehicle = vec3(vehicleX, vehicleY, vehicleZ);
	
	// Render the Model through successive animation steps
	for (int angle=0; angle<=360; angle++)
	{
		positionVehicle.x = orbitRadius * (float) cos((angleVehicle * M_PI / 180.0)) + positionSphere.x;
		positionVehicle.z = orbitRadius * (float) sin((angleVehicle * M_PI / 180.0)) + positionSphere.z;
		angleVehicle += 1.0f;
		angleSphere += 0.2f;
		//
		//myModel1.transformModel(  (float) -angle, Y_AXIS, positionVehicle, vehicleScale  );
		myModel1.transformModel(   (float) -angle, 0.0f, 0.0f, positionVehicle, vehicleScale  );
		myModel2.transformModel( 90.0f, (float) 23.4f, (float) -angleSphere, positionSphere, sphereScale );
		//
		myRenderer.swapBuffers();
		myRenderer.clearBuffers();
		myRenderer.renderModel( myModel1 );
		myRenderer.renderModel( myModel2 );
		myRenderer.display( myDisplay );
		//myRenderer.saveScene();
	}
	// Render the Model through successive animation steps
	for (int angle=0; angle<=360; angle+=2)
	{
		positionVehicle.y = orbitRadius * (float) cos((angleVehicle * M_PI / 180.0)) + positionSphere.y;
		positionVehicle.z = orbitRadius * (float) sin((angleVehicle * M_PI / 180.0)) + positionSphere.z;
		angleVehicle -= 1.0f;
		angleSphere += 0.2f;
		//
		//myModel1.transformModel( (float) angle, X_AXIS, positionVehicle, vehicleScale  );
		myModel1.transformModel( 90.0f, (float) -angle, -90.0f, positionVehicle, vehicleScale  );
		myModel2.transformModel( 90.0f, (float) 23.4f, (float) -angleSphere, positionSphere, sphereScale );
		//
		myRenderer.swapBuffers();		
		myRenderer.clearBuffers();
		myRenderer.renderModel( myModel1 );
		myRenderer.renderModel( myModel2 );
		myRenderer.display( myDisplay );
		//myRenderer.saveScene();
	}
	// Render the Model through successive animation steps
	for (int angle=0; angle<=360; angle+=2)
	{
		positionVehicle.x = orbitRadius * (float) cos((angleVehicle * M_PI / 180.0)) + positionSphere.x;
		positionVehicle.z = orbitRadius * (float) sin((angleVehicle * M_PI / 180.0)) + positionSphere.z;
		angleVehicle += 1.0f;
		angleSphere += 0.2f;
		//
		myModel1.transformModel( (float) -angle, (float) -angle, 0.0f, positionVehicle, vehicleScale );
		myModel2.transformModel( 90.0f, (float) 23.4f, (float) -angleSphere, positionSphere, sphereScale );
		//
		myRenderer.swapBuffers();
		myRenderer.clearBuffers();
		myRenderer.renderModel( myModel1 );
		myRenderer.renderModel( myModel2 );
		myRenderer.display( myDisplay );
		//myRenderer.saveScene();
	}
	// Write last rendered scene to disk
	myRenderer.saveSceneJPG();
	return 0;
}
