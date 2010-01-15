#include <iostream>
using namespace std;
#include "SDL.h"
#include "GraphicsLoader.h"
#include "Game Data Handler.h"

static const int SCREEN_WIDTH = 860;
static const int SCREEN_HEIGHT = 640;
static const int SCREEN_BPP = 32;

int main(int argc, char ** argv)
{
	int bob = 300;
	cout << bob << "\n";
	bob = !bob;
	cout << bob << "\n";
	bob = !bob;
	cout << bob << "\n";
	if(SDL_Init( SDL_INIT_EVERYTHING) == -1)
	{
		return 1;
	}
	Uint32 then = SDL_GetTicks(), now;
	int msPassed = 0, updateTimer = 0, drawTimer = 0;
	while(IH::Instance()->isGameRunning())
	{
		//handle game timers
		now = SDL_GetTicks();
		msPassed = now - then;
		then = now;
		updateTimer += msPassed;
		drawTimer += msPassed;

		string name;
		getline(cin, name, '#');
		//input
		while(SDL_PollEvent(&IH::Instance()->event))
		{
			IH::Instance()->handlePrimaryInput();
		}		

		//logic
		if(updateTimer > 16)
		{
			updateTimer = 0;
			IH::Instance()->update(msPassed);
		}

		//draw
		if(drawTimer > 33)
		{
			drawTimer = 0;
			IH::Instance()->drawAll();
		}
	}
	SDL_Quit();
	return 0;
}