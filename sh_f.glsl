#version 330

smooth in vec2 fragUV;	// Interpolated texture coordinate 0.0-1.0
smooth in vec2 position; // Interpolated projection coordinate -1.0-1.0

uniform sampler2D tex;	// Texture sampler
uniform int viewmode;	// Color or depth viewing

out vec3 outCol;	// Final pixel color

float screenWidth = 1400;
float screenHeight = 1050;
float kinectWidth = 640;
float kinectHeight = 480;


float screenOffsetX = 1.0f/screenWidth;
float screenOffsetY = 1.0f/screenHeight;
float kinectOffsetX = 1.0f/kinectWidth;
float kinectOffsetY = 1.0f/kinectHeight;

vec2 translatePoint(vec2 worldScale)
{
	float myX = ((2.0f*float(worldScale.x))/screenWidth)-1.0f;
	float myY = ((2.0f*float(worldScale.y))/screenHeight)-1.0f;
	vec2 openGLPoint = vec2(myX,myY);
	
	return openGLPoint;
}

void drawPoint(vec2 myPoint)
{
	if(position.x < myPoint.x+screenOffsetX && position.x > myPoint.x-screenOffsetX)
	{
		if(position.y < myPoint.y+screenOffsetY && position.y > myPoint.y-screenOffsetY)
		{
			outCol = vec3(1.0,1.0,0.0);
		}
	}
}

void main()
{
	// Sample texture as output color
	if (viewmode == 0)
		outCol = texture2D(tex, fragUV).rgb;
	else
		outCol = vec3(1.0) - texture2D(tex, fragUV).rrr;

	if(position.x < screenOffsetX && position.x > -screenOffsetX)
	{
		outCol = vec3(0.0,1.0,1.0);
	}
	if(position.y < screenOffsetY && position.y > -screenOffsetY)
	{
		outCol = vec3(0.0,1.0,1.0);
	}

	if(fragUV.x < 0.5+kinectOffsetX && fragUV.x > 0.5-kinectOffsetX)
	{
		outCol = vec3(1.0,0.0,1.0);
	}
	if(fragUV.y < 0.5+kinectOffsetY && fragUV.y > 0.5-kinectOffsetY)
	{
		outCol = vec3(1.0,0.0,1.0);
	}

	vec2 worldPoint = vec2(350,263);

	vec2 openGLPoint = translatePoint(worldPoint);

	drawPoint(openGLPoint);
}