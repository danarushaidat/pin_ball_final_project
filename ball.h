#ifndef ball_H
#define ball_H
class ball
{
private :
	float xpos; 
	float ypos; 
	float radius; 
	float xvelocity;
	float yvelocity; 

	public:
	ball(float x, float y, float radius, float xvelocity, float yvelocity);
		
    void draw();
	void update();
	bool check_collision( ball &b);
	void resolve_collision(ball &b);
	void distroy_ball();
	float getxpos();
	float getypos();
	float getradius();

};

#endif