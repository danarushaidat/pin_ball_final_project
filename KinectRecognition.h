#ifndef KinectRecognition_H
#define KinectRecognition_H

#define X_POSITION 0
#define Y_POSITION 1
#define R_POSITION 2

//Generael structures
#include <vector>

//OpenGL Related
#include "gl_core_3_3.h"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class KinectRecognition
{
public:
	//------------------Variables------------------//

	//-------------------Methods-------------------//
	KinectRecognition();

	void startAnalysis(vector<GLubyte> pColorData, vector<GLubyte> pDepthData);

	vector<GLfloat>* getBallGenerator();
	vector<vector<GLfloat>>* getMallets();

private:
	//------------------Variables------------------//
	// Kinect image dimensions
	GLint texWidth;		
	GLint texHeight;

	// Window dimension
	GLint winWidth;		
	GLint winHeight;

	// Ball Generator values
	vector<GLfloat>* ballGenerator;

	// Mallet values
	vector<vector<GLfloat>>* mallets;

	//-------------------Methods-------------------//
	vec2* kinectToWindow(vec2* kinectPoint);
	
};

#endif