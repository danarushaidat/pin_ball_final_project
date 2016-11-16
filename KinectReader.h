#ifndef KinectReader_H
#define KinectReader_H

//Generael structures
#include <vector>

// Kinect required headers
#include <Windows.h>
#include <Ole2.h>
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>

//OpenGL
#include "gl_core_3_3.h"

using namespace std;

class KinectReader
{
public:
	//------------------Variables------------------//

	//-------------------Methods-------------------//
	KinectReader(vector<GLubyte> pColorData, vector<GLubyte> pDepthData);

	// Kinect functions
	GLubyte* kinectGetColor();
	GLubyte* kinectGetDepth();

private :
	//------------------Variables------------------//
	// Kinect image dimensions
	GLint texWidth;		
	GLint texHeight;

	// Kinect state
	INuiSensor* kinect;
	HANDLE colorStream;
	HANDLE depthStream;

	vector<GLubyte> colorData;	// Color data from kinect
	vector<GLubyte> depthData;	// Depth data from kinect

	//-------------------Methods-------------------//
	void initKinect();
	
};

#endif