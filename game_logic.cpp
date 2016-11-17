#include "game_logic.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

// game logic constructor ------------------------------------------------------
Game_Logic::Game_Logic()
{
	game_started = false;
	number_of_players = 3;
	player_area = glm::vec2(1400, 1050);
	Player_Status P;
	for (int i = 0; i <number_of_players; i++)
	{
		P.is_alive = true;
		P.lives = 10;
		P.player_no = i + 1;
		P.wall = i + 1;   //1)east wall     2)south  wall 		3)west wall  the last wall is 4) north but no player is using it 
		player_status.push_back(P);
	}
	max_number_of_balls = 10;
	last_ball_number = 0;
}
//game logic start ------------------------------------------------------------------------
void Game_Logic::start()
{
	game_started = true;
	init();
}
//game logic init ------------------------------------------------------------------------
void  Game_Logic::init()
{
	for (int i = 0; i < max_number_of_balls;i++)
	create_a_ball();
}
//game logic check collision------------------------------------------------------------
bool Game_Logic::check_ball_collision(int i, int j)
{
	glm::vec2 difference= balls[i].pos - balls[j].pos;
	float distance = glm::dot(difference, difference);
	float radius_sum = balls[i].radius + balls[j].radius; 
	float radius_square = radius_sum*radius_sum;
	if (distance <= radius_square)
		return true;
	else 
		return false;
}
//game logic check update-------------------------------------------------------------
void Game_Logic::update()
{
	for (int i = 0; i < balls.size(); i++)
	{
		if ((balls[i].velocity.x == 0.0) && (balls[i].velocity.y == 0))
		{		balls[i].velocity = glm::vec2(1.0, 1.0);
				}
		balls[i].pos += balls[i].velocity;
	}
	std::vector<Collided_Objects>  collided_obj = check_collision();
	resolve_collision(collided_obj);
	if (balls.size() < max_number_of_balls)
		create_a_ball();
}
std::vector<Collided_Objects> Game_Logic::check_collision()
{
	std::vector<Collided_Objects> collided_objects; 
	Collided_Objects co;
	for (int i = 0; i < balls.size(); i++)
	{
		for (int j = i + 1; j < balls.size(); j++)
		{
			if (check_ball_collision(i, j))
			{
				co.ball_number = balls[i].ball_number; co.object_type = BALL; co.object_number = balls[j].ball_number;
				collided_objects.push_back(co);
			}
		}
	}
	//for testing collision with the four walls 
	for (int i = 0; i < balls.size(); i++)
	{		
		if (balls[i].pos.x + balls[i].radius >=player_area.x )
		{
			co.ball_number = i; co.object_type = WALL;
			co.object_number = 1;
			collided_objects.push_back(co);
		}
		else if (balls[i].pos.x - balls[i].radius <= 0.0)
		{
			co.ball_number = i; co.object_type = WALL;
			co.object_number = 3;
			collided_objects.push_back(co);
		}
		else if ((balls[i].pos.y + balls[i].radius >= player_area.y))
		{
			co.ball_number = i; co.object_type = WALL;
			co.object_number = 4;
			collided_objects.push_back(co);
		}
		else if ((balls[i].pos.y - balls[i].radius <= 0.0))
		{
			co.ball_number = i; co.object_type = WALL;
			co.object_number = 2;
			collided_objects.push_back(co);
		}
	}
	cout << endl << "collided_objects are " << endl;
	for (int i = 0; i < collided_objects.size(); i++)
		cout << "ball #" << collided_objects[i].ball_number << "   collided with object    " << collided_objects[i].object_type << "   with #  " << collided_objects[i].object_number << endl << endl;
		return collided_objects;
}
//game logic resolve collision------------------------------------------------------------
void Game_Logic::resolve_collision(std::vector<Collided_Objects>)
{


}
//game logic set mallet------------------------------------------------------------------
void Game_Logic::set_mallet(std::vector<glm::vec2> mallets)
{
	mallet_pos = mallets;
}
//game logic set random ball generator-----------------------------------------------------

void Game_Logic::set_random_ball_genereator(glm::vec2 RBGP) 
{
	random_ball_gen_pos = RBGP;
}
//game logic display values  ------------------------------------------------------------------
void Game_Logic::display_values()
{
	cout << " the game status is " << game_started << endl;
	cout << "the number of players" << number_of_players<<endl;
	cout << "balls in the system" << endl;
	for (int i = 0; i < balls.size(); i++)
	{ 
		cout<<"ball_number "<<balls[i].ball_number << "with the pos"<<balls[i].pos.x << "  "<< balls[i].pos.y<<endl;
	}
	cout << endl << endl;
}
//game logic display values  ------------------------------------------------------------------
void Game_Logic::create_a_ball()
{
	Ball ball; 
	srand(time(NULL));
	ball.pos = glm::vec2(rand() % 1400, rand() % 1050);
	ball.velocity = glm::vec2((int)(rand() % 3 -1), (int)(rand() % 3 -1));
	ball.radius = 100;
	last_ball_number++;
	ball.ball_number = last_ball_number;
	balls.push_back(ball);
	
}