#include "KinectRecognition.h"

KinectRecognition::KinectRecognition()
{
	texWidth = 640;		
	texHeight = 480;
	winWidth = 1400;
	winHeight = 1050;

	ballGenerator = new vector<GLfloat>();
	mallets = new vector<vector<GLfloat>>();
}

void KinectRecognition::startAnalysis(vector<GLubyte> pColorData, vector<GLubyte> pDepthData)
{
	// Basically, read from the color and depth data sop that
	// I can find the position of the generator and the mallets
	// and update them to the internal class definition of them
	// As for now, I'm creating wired values

	vec2* generatorKinectPosition = new vec2(320,240);
	vec2* generatorWinPosition = kinectToWindow(generatorKinectPosition);

	ballGenerator->push_back(generatorWinPosition->x);
	ballGenerator->push_back(generatorWinPosition->y);
	ballGenerator->push_back(5.0f);

	int numberOfPlayers = 3;
	for(int i =0;i<numberOfPlayers;i++)
	{
		/*
		vec2* playerKinectPosition = new vec2(320,240);
		vec2* playerWinPosition = kinectToWindow(generatorKinectPosition);

		vector<GLfloat>* newPlayer = new vector<GLfloat>();

		ballGenerator->push_back(generatorWinPosition->x);
		ballGenerator->push_back(generatorWinPosition->y);
		ballGenerator->push_back(5.0f);
		*/
	}
}

vector<GLfloat>* KinectRecognition::getBallGenerator()
{
	return ballGenerator;
}

vector<vector<GLfloat>>* KinectRecognition::getMallets()
{
	return mallets;
}

vec2* KinectRecognition::kinectToWindow(vec2* kinectPoint)
{
	vec2* winPoint = new vec2();

	winPoint->x = (winWidth*kinectPoint->x)/texWidth;
	winPoint->y = (winHeight*kinectPoint->y)/texHeight;

	return winPoint;
}