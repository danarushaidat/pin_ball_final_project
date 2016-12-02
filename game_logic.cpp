#include "game_logic.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
  
///							wall3 ( player3) 
//     ------------------------------------------------------------
//     --														---
//     --														---
//     --														---
//wall4--														---  wall2(player2)
// no  --														---
//play --														---
// -er --														---
//     --														---
//     --														---
//     ------------------------------------------------------------
//							wall1(player1)

// game logic constructor ------------------------------------------------------
Game_Logic::Game_Logic()
{
	srand(time(NULL));
	game_started = false;
	uniform_ball_radius = 100;
	number_of_players = 3;
	player_area = glm::vec2(1400, 1050);
	for (int i = 0; i <number_of_players; i++)

	{
		Player_Status P;
		P.is_alive = true;
		P.lives = 10;
		P.player_no = i + 1;
		P.wall = i + 1;   
		player_status.push_back(P);
	}
	max_number_of_balls =4 ;
	last_ball_number = 0;
	random_ball_generator_radius = uniform_ball_radius;
	set_random_ball_genereator(glm::vec2(700,525));
	goal_boundary.push_back (glm::vec2(550, 850));
	goal_boundary.push_back(glm::vec2(375, 675));
	goal_boundary.push_back(glm::vec2(550, 850));
	ball_speed = 30.0;
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
	create_a_ball();
}
std::vector<glm::vec2> Game_Logic::get_goal_boundary()
{
	return goal_boundary;
}
//game logic check collision------------------------------------------------------------
bool Game_Logic::check_ball_collision(Ball b1, Ball b2)
{
	glm::vec2 difference= b1.pos - b2.pos;
	float distance = glm::dot(difference, difference);
	float radius_sum = b1.radius + b2.radius; 
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
		//to update very slow balls 
		if (glm::length(balls[i].velocity)<ball_speed*0.5)
			balls[i].velocity = glm::vec2(1.5, 1.5)*balls[i].velocity;
		balls[i].pos += balls[i].velocity;
		cout << "ball number " <<balls[i].ball_number<< " with pos " << balls[i].pos.x << "  " << balls[i].pos.y << "   and velocity   " << balls[i].velocity.x << "  " << balls[i].velocity.y << endl;
		
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
			if (check_ball_collision(balls[i],balls[j]))
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
			co.ball_number = balls[i].ball_number; co.object_type = WALL;
			co.object_number = 2;
			collided_objects.push_back(co);
		}
		else if (balls[i].pos.x - balls[i].radius <= 0.0)
		{
			co.ball_number = balls[i].ball_number; co.object_type = WALL;
			co.object_number = 4;
			collided_objects.push_back(co);
		}
		else if ((balls[i].pos.y + balls[i].radius >= player_area.y))
		{
			co.ball_number = balls[i].ball_number; co.object_type = WALL;
			co.object_number = 3;
			collided_objects.push_back(co);
		}
		else if ((balls[i].pos.y - balls[i].radius <= 0.0))
		{
			co.ball_number = balls[i].ball_number; co.object_type = WALL;
			co.object_number = 1;
			collided_objects.push_back(co);
		}
	}
	//testing collision with the mallet 
	for (int i = 0; i < balls.size(); i++)
	{
		for (int j = 0; j < mallets_pos.size(); j++)
		{
			glm::vec2 difference = balls[i].pos - mallets_pos[j];
			float distance = glm::dot(difference, difference);
			float radius_sum = balls[i].radius + balls[i].radius;  /// I'm supposing the mallet has the same radius like the ball 
			float radius_square = radius_sum*radius_sum;
			if (distance <= radius_square)
			{
				co.ball_number = balls[i].ball_number; co.object_type = MALLET;
				co.object_number = j+1;
				collided_objects.push_back(co);
			}
					
		}
	}
	for (int i = 0; i < balls.size(); i++)
	{
		Ball b;  b.radius = random_ball_generator_radius; b.pos = random_ball_gen_pos;
		if (check_ball_collision(balls[i], b))
		{
			co.ball_number = balls[i].ball_number; co.object_type = RANDOM_BALL_GENERATOR;
			co.object_number = 1;
			collided_objects.push_back(co);
		}
	}
	cout << endl << "collided_objects are " << endl;
	for (int i = 0; i < collided_objects.size(); i++)
	cout << "ball #" << collided_objects[i].ball_number << "   collided with object    " << collided_objects[i].object_type << "   with #  " << collided_objects[i].object_number << endl << endl;
	return collided_objects;
}
//game logic resolve collision------------------------------------------------------------
void Game_Logic::resolve_collision(std::vector<Collided_Objects> collided_object )
{
	for (int i = 0; i < collided_object.size(); i++)
	{
		if (collided_object[i].object_type == BALL)
			resolve_ball_ball_collision(collided_object[i]);
		else if (collided_object[i].object_type == MALLET)
			resolve_ball_mallet_collision(collided_object[i]);
		else if (collided_object[i].object_type ==WALL)
			resolve_ball_wall_collision (collided_object[i]);
		else if(collided_object[i].object_type == RANDOM_BALL_GENERATOR)
			resolve_ball_generator_collision(collided_object[i]);
	}

	
	for (int i = 0; i < number_of_players; i++)
	{
		cout << "player # " << player_status[i].player_no << "is alive " << player_status[i].is_alive << "is alive " << "has lives = " << player_status[i].lives << endl << endl;
	}
}
//game logic set mallet------------------------------------------------------------------

void Game_Logic::set_mallet(std::vector<glm::vec2> mallets)
{
	mallets_pos = mallets;
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
		cout<<"ball_number "<<balls[i].ball_number << "with the pos"<<balls[i].pos.x << "  "<< balls[i].pos.y<< "    and velocity   "<<balls[i].velocity.x<<"   " <<balls[i].velocity.y<<endl;
	}
	cout << endl << endl;
}

glm::vec2 Game_Logic::get_random_ball_genreator()
{
	return random_ball_gen_pos;

}
//game logic create a ball-----------------------------------------------------------------
void Game_Logic::create_a_ball()
{
	bool there_is_collision = false;
	if (waiting_balls.size() != 0)
	{
		for (int i = 0; i < waiting_balls.size(); i++)
		{
			there_is_collision = false;
			for (int j = 0; j < balls.size(); j++)
			{
				if (check_ball_collision(waiting_balls[i], balls[j]))
				{
					there_is_collision = true; break;
				}
			}
			if (!there_is_collision)
			{
				balls.push_back(waiting_balls[i]);
				waiting_balls.erase(waiting_balls.begin() + i);
			}
		}
	}
	else
	{
		bool there_is_collision = false;
		Ball ball;
		//ball.pos = glm::vec2(rand() %int( player_area.x), rand() % int(player_area.y));
		ball.radius = uniform_ball_radius;
		last_ball_number++;
		ball.ball_number = last_ball_number;
		float angle = rand() % 360;
		float radian_angle = angle *3.14 / 180.0;
		ball.velocity = glm::vec2(glm::sin(radian_angle) *ball_speed, glm::cos(radian_angle)* ball_speed);
		glm::vec2 vol_n = glm::normalize(ball.velocity);
		ball.pos = random_ball_gen_pos + glm::vec2(2.0*ball.radius, 2.0*ball.radius) *vol_n;
		for (int i = 0; i < balls.size(); i++)
		{
			if (check_ball_collision(ball, balls[i]))
			{
				there_is_collision = true;
				break;
			}
		}
		if (there_is_collision)
			waiting_balls.push_back(ball);
		else
			balls.push_back(ball);
	}//end of else 
	
}
std::vector<Ball>  Game_Logic::get_balls()
{
	return balls;
	
}
bool Game_Logic::get_game_started()
{
	return  game_started;
}
void Game_Logic::resolve_ball_ball_collision(Collided_Objects ball_collision)
{ 
	int z = find_ball_index(ball_collision.ball_number);
	int w = find_ball_index(ball_collision.object_number);
	float m1, m2, x1, x2, kissing_test1,kissing_test2;
	glm::vec2 x = balls[z].pos - balls[w].pos;
	x=glm::normalize(x);
	kissing_test1 = glm::dot(balls[w].velocity, x);
	glm::vec2  v1= balls[z].velocity;
	 x1 = glm::dot(x,v1);
	 glm::vec2 v1x = x * x1;
	glm::vec2 v1y = v1 - v1x;
	x = x*glm::vec2(-1,-1);
	kissing_test2= glm::dot(balls[z].velocity, x);
	glm::vec2 v2 = balls[w].velocity;
	x2 = glm::dot(x,v2);
	glm::vec2 v2x = x * x2;
	glm:: vec2 v2y = v2 - v2x;
	if (!(kissing_test1 <= 0 && kissing_test2 <= 0))
	{
		balls[z].velocity = glm::vec2(v2x + v1y);
		balls[w].velocity = glm::vec2(v1x + v2y);
	}

	

	//https://github.com/Studiofreya/code-samples/blob/master/opengl/collisiondetect/collisiondetect.cpp		
	
	}

void Game_Logic::resolve_ball_mallet_collision(Collided_Objects mallet_collision)
{
}
int Game_Logic::find_ball_index(int ball_number) {

	for (int i = 0; i < balls.size(); i++)
	{
		if (balls[i].ball_number == ball_number)
			return i; 
	}
	return -1;
}
bool Game_Logic::ball_in_goal(Collided_Objects collided_obj)
{
	int z = find_ball_index(collided_obj.ball_number);
	if (z != -1)
	{
		if (collided_obj.object_number == 1)
		{
			if ((balls[z].pos.x >= goal_boundary[0].x) && (balls[z].pos.x <= goal_boundary[0].y))
				return true;
		}
		else if (collided_obj.object_number == 2)
		{
			if ((balls[z].pos.y >= goal_boundary[1].x) && (balls[z].pos.y <= goal_boundary[1].y))
				return true;
		}
		else if (collided_obj.object_number == 3)
		{
			if ((balls[z].pos.x >= goal_boundary[2].x) && (balls[z].pos.x <= goal_boundary[2].y))
				return true;

		}
	}
	return false;
}

void Game_Logic::resolve_ball_wall_collision(Collided_Objects wall_collision)
{
	int z = find_ball_index(wall_collision.ball_number);
	if ((wall_collision.object_type == WALL) && (wall_collision.object_number == 4))
	{
		balls[z].velocity.x = -balls[z].velocity.x;
		balls[z].pos.x = balls[z].radius + 3;

	}
	else if ((wall_collision.object_type == WALL) && (wall_collision.object_number != 4))
	{

		if ((player_status[wall_collision.object_number-1].is_alive) && (ball_in_goal(wall_collision)))
		{	
			player_status[wall_collision.object_number-1].lives--;
			balls.erase(balls.begin() + z);
			if (player_status[wall_collision.object_number - 1].lives == 0)
				player_status[wall_collision.object_number - 1].is_alive = false;
			if (!player_status[0].is_alive && !player_status[0].is_alive && !player_status[0].is_alive)
			{
				game_started = false;
				exit(0);
			}
		}
		else if ((player_status[wall_collision.object_number - 1].is_alive) && (!(ball_in_goal(wall_collision)))
			|| (!(player_status[wall_collision.object_number - 1].is_alive)))
		{
			switch (wall_collision.object_number)
			{
			case 1: balls[z].velocity.y = -balls[z].velocity.y; balls[z].pos.y = balls[z].radius + 3; break;
			case 2: balls[z].velocity.x = -balls[z].velocity.x; balls[z].pos.x = player_area.x - balls[z].radius - 3; break;
			case 3: balls[z].velocity.y = -balls[z].velocity.y; balls[z].pos.y = player_area.y - balls[z].radius - 3; break;
			default: cout << "error no wall with this number ";
			}
		}
		
	}
}

void Game_Logic::resolve_ball_generator_collision(Collided_Objects random_ball_collision)
{
	int z = find_ball_index(random_ball_collision.ball_number);
	glm::vec2 N = balls[z].pos - random_ball_gen_pos;
	N = glm::normalize(N);
	glm::vec2 d = balls[z].velocity;
	glm::vec2 T = glm::dot(d, N)*N;
	balls[z].velocity = d - T - T;
	}