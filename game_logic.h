#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
/*
game logic will receive the positions of the objects in the real environemt recieved by the kinect (mallet pos , and random ball generatormpos) those are input from the kinect recognizer
start will start the game and call initialize for creating the players 
for each step we will :
1) move the balls 
2) check for collision
3) resolve collision 
4) update player status 
each player will have a set of lives and once he loses those lifes the player is out 

my walls should be numbered as 

1)east
2)south
3)west
 4) north

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

*/
struct Player_Status
{
	bool is_alive; // if he is alive its true 
	int lives;
	int player_no; // which can be 1 to 3 
	int wall;   // which is the wall he is standing on 1)east 	2)south 	3)west    4) north 
	};

struct Ball
{
	glm::vec2  pos; 
	glm::vec2 velocity;
	float radius; 
	int ball_number; 
	};
enum Object_Type { WALL, BALL, MALLET, RANDOM_BALL_GENERATOR};
struct Collided_Objects
{
	int ball_number; 
	Object_Type object_type; 
	int object_number; 
};
class Game_Logic
{ private:
	bool game_started;
	int number_of_players;
    int max_number_of_balls;
	int last_ball_number;
	glm::vec2 random_ball_gen_pos;
	std::vector<glm::vec2> goal_boundary;
	std::vector <Player_Status> player_status; //player status, player live ,player number, player name, wall no
	std::vector<Ball>balls; //vector that contains all the balls currently active with their positions ,velocities 
	std::vector<Ball>waiting_balls;
	std::vector <glm::vec2>mallets_pos; //the positions of the mallets received from the kinect 
	glm::vec2 player_area; // where the actual game will be conducted 
	float ball_speed;
	void create_a_ball();
	bool check_ball_collision(Ball b1, Ball b2);
	void	resolve_ball_ball_collision(Collided_Objects);
	void resolve_ball_mallet_collision(Collided_Objects);
	void resolve_ball_wall_collision (Collided_Objects);
	void resolve_ball_generator_collision(Collided_Objects);
	int find_ball_index(int ball_number);
	bool ball_in_goal(Collided_Objects );
	int uniform_ball_radius;
	float  random_ball_generator_radius;
	public:

		Game_Logic();
		void init();
		void update();
		std::vector<Collided_Objects> check_collision();  
		void resolve_collision(std::vector<Collided_Objects>);
		void start();
		void set_mallet(std::vector<glm::vec2> mallets);
		void set_random_ball_genereator(glm::vec2);
		void display_values();
		std::vector<Ball> get_balls();
		std::vector<glm::vec2> get_goal_boundary();
		glm::vec2  get_random_ball_genreator();
		bool get_game_started();
	};