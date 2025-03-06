 #include <stdio.h>
#include "SDL2/SDL.h"
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

const int WIDTH = 25;//How many boxes make up the width of the screen
const int HEIGHT = 25;//Boxes in height of screen

const int BOX_WIDTH = SCREEN_WIDTH/WIDTH*5/6;
const int BOX_HEIGHT = SCREEN_HEIGHT/HEIGHT*5/6;

//The space between the boxes
const int BUFFER = SCREEN_WIDTH/WIDTH - BOX_WIDTH;

const int SNAKE_START_LENGTH = 4;

//Used to tell the makeBox function what color
//White for snake, red for food
const int SNAKE = 0;
const int FOOD = 1;

const int DELAY = 150;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

//A vector holding the coordinates of all of the squares in the snake
//First element in the vector is the front of the snake, last is the back
std::vector<std::array<int,2> > squares(SNAKE_START_LENGTH);

//The current coordinates of the food
std::array<int,2> food;

//Direction constants
const int RIGHT = 0;
const int UP = 1;
const int LEFT = 2;
const int DOWN = 3;

//The direction in which the snake is traveling
int direction = RIGHT;

//Inits SDL, window, and renderer
bool init(){
	bool success = true;
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("Could not initialize SDL. SDL Error: %s\n",SDL_GetError());
		success = false;
	}

	gWindow = SDL_CreateWindow("Snake",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH + BUFFER,SCREEN_HEIGHT + BUFFER,SDL_WINDOW_SHOWN);
	if(gWindow == NULL){
		printf("Could not create window. SDL Error: %s\n",SDL_GetError());
		success = false;
	}
	else{
		gRenderer = SDL_CreateRenderer(gWindow,-1,SDL_RENDERER_ACCELERATED);
		if(gRenderer == NULL){
			printf("Could not create renderer. SDL Error: %s\n",SDL_GetError());
			success = false;
		}
		else{
			SDL_SetRenderDrawColor(gRenderer,0x00,0x00,0x00,0xFF);
		}
	}
	return success;
}

//Sets up everything for the snake game
void snakeInit(){
	for(int i = 0;i < SNAKE_START_LENGTH;i++){
		squares[i] = {WIDTH/2-i,HEIGHT/2};
	}
}

void makeBox(int x,int y,int type){
	SDL_Rect box = {(BOX_WIDTH + BUFFER)*x+BUFFER,(BOX_HEIGHT + BUFFER)*y+BUFFER,BOX_WIDTH,BOX_HEIGHT};


	//If the box is part of the snake it is white
	//If it is food, it is red
	if(type == SNAKE){
		SDL_SetRenderDrawColor(gRenderer,0xFF,0xFF,0xFF,0xFF);
	}
	else if(type == FOOD){
		SDL_SetRenderDrawColor(gRenderer,0xFF,0x00,0x00,0xFF);
	}

	SDL_RenderFillRect(gRenderer,&box);
}

//Returns true if the array is in the vector
bool inVector(std::vector<std::array<int,2> > vect,std::array<int,2> arr){
        for(int i = 0;i < vect.size();i++){
                if(vect[i] == arr){
                        return true;
                }
        }
        return false;
}

//Moves the food to a random location that is not on the snake
void moveFood(){
        do{
                int foodX = rand() % WIDTH;
                int foodY = rand() % HEIGHT;
                food = {foodX,foodY};
        }while(inVector(squares,food));
}

//Updates the position of the snake
void updateSnake(){

	//Checks to see if the snake ate the food
        if(squares[0] == food){
                squares.push_back(squares[squares.size()]);
                moveFood();
        }

	for(int i = squares.size()-1;i > 0;i--){
		squares[i] = squares[i-1];
	}

	if(direction == UP){
		squares[0] = {squares[0][0],squares[0][1]-1};
	}
	else if(direction == RIGHT){
		squares[0] = {squares[0][0]+1,squares[0][1]};
	}
	else if(direction == DOWN){
		squares[0] = {squares[0][0],squares[0][1]+1};
	}
	else{
		squares[0] = {squares[0][0]-1,squares[0][1]};
	}
}
void drawBorders(){
	//Draws the grey borders on the edges of the screen
	SDL_SetRenderDrawColor(gRenderer,0xA0,0xA0,0xA0,0xFF);

	//Left edge border
	SDL_Rect leftBorder = {0,0,BUFFER,SCREEN_HEIGHT};
	SDL_RenderFillRect(gRenderer,&leftBorder);

	//Right edge border
	SDL_Rect rightBorder = {SCREEN_WIDTH,0,BUFFER,SCREEN_HEIGHT};
	SDL_RenderFillRect(gRenderer,&rightBorder);

	//Top edge border
	SDL_Rect topBorder = {0,0,SCREEN_WIDTH,BUFFER};
	SDL_RenderFillRect(gRenderer,&topBorder);

	//Bottom edge border
	SDL_Rect bottomBorder = {0,SCREEN_HEIGHT,SCREEN_WIDTH+BUFFER,BUFFER};
	SDL_RenderFillRect(gRenderer,&bottomBorder);
}

int main(int argc,char* args[]){
	srand(time(0));

	moveFood();

	if(!init()){
		printf("Error initializing.\n");
		return 0;
	}

	snakeInit();

	bool quit = false;
	SDL_Event e;

	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT){
				//Quits if the player presses the X
				quit = true;
			}
			else if(e.type == SDL_KEYDOWN){
				switch(e.key.keysym.sym){
					case SDLK_UP:
						if(direction != DOWN){
							direction = UP;
						}
						break;
					case SDLK_LEFT:
						if(direction != RIGHT){
							direction = LEFT;
						}
						break;
					case SDLK_DOWN:
						if(direction != UP){
							direction = DOWN;
						}
						break;
					case SDLK_RIGHT:
						if(direction != LEFT){
							direction = RIGHT;
						}
						break;
				}
			}
		}
		//Clears the screen before drawing the snake
		SDL_SetRenderDrawColor(gRenderer,0x00,0x00,0x00,0xFF);
		SDL_RenderClear(gRenderer);

		updateSnake();

		//Draws the food
		makeBox(food[0],food[1],FOOD);

		for(int i = 0;i < squares.size();i++){
			makeBox(squares[i][0],squares[i][1],SNAKE);
		}

		//Draws the borders on the edges of the screen
		drawBorders();

		//Updates the renderer
		SDL_RenderPresent(gRenderer);

		SDL_Delay(DELAY);
	}
	//Quits SDL
	SDL_Quit();
	return 0;
}
