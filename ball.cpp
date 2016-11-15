#include "ball.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gl_core_3_3.h"
#include <GL/freeglut.h>
#include "util.hpp"

float window_width=640;
float window_hight=480; 

//ball constructor
ball::ball(float x, float y, float r, float xvel , float yvel)
{xpos = x; ypos =y; radius=r;
xvelocity =xvel; yvelocity = yvel;
}

float ball::getxpos()
{return xpos;}

float ball::getypos()
{return ypos;}

float ball::getradius()
{return radius;}


bool ball::check_collision(ball &b)
{
	//calculate the distance between the two balls
	float x_distance=  getxpos() -b.getxpos();
	float y_distance = getypos() -b.getypos();
	float distance = (x_distance * x_distance) + ( y_distance * y_distance);

	//calculate the summation of the balls radius
	float radius_summation= getradius()  + b.getradius(); 
	float sqr_radius_summation = radius_summation * radius_summation;
	

	//compare if distance is less than the radius summation the balls are colliding
	if ( distance <= sqr_radius_summation)
		return true;



		return false; 
}
void ball::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
     glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(xpos, ypos, 0.0f));
    glutSolidSphere(radius,50,50);

}
void ball::update ()
{
xpos += xvelocity;
ypos+=yvelocity;
if (xpos<0.0) 
{
xpos=0.0;
xvelocity =-xvelocity;
}
else
if (xpos > window_width)
{
xpos= window_width ;
xvelocity=-xvelocity;
}
if ( ypos<0.0 ) 
{
ypos=0;
yvelocity= -yvelocity;
}
else if ( ypos>window_hight) 
{
ypos = window_hight;
yvelocity= - yvelocity;
}
}

void ball::resolve_collision(ball &ball)
{






}