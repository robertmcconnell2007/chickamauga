#include <iostream>
#include <string>
using namespace std;
#include "SDL.h"
#include "GraphicsLoader.h"
#include "Game Data Handler.h"
#include "SDL_ttf.h"	// true-type font library for SDL
#include "messageHandler.h"

static const int SCREEN_WIDTH = 860;
static const int SCREEN_HEIGHT = 640;
static const int SCREEN_BPP = 32;

#define mainDebug




int main(int argc, char ** argv)
{
	if(SDL_Init( SDL_INIT_EVERYTHING) == -1)
	{
		return 1;
	}
	if(TTF_Init() != 0)
	{
		printf("could not initialize True Type Fonts\n");
		return 2;
	}
#ifdef mainDebug
	fstream outFile;
	outFile.open("sendFile.txt",fstream::out);
	outFile << "Data Send File\n";
	outFile.close();
	fstream outFile2;
	outFile2.open("recvFile.txt",fstream::out);
	outFile2 << "Data Recv File\n";
	outFile2.close();
	fstream outFile3;
	outFile3.open("playingLAN states.txt", fstream::out);
	outFile3 << "Playing Lan states\n";
	outFile3.close();
#endif
	//MessageHandler::Instance()->setupHost();
	Uint32 then = SDL_GetTicks(), now;
	int msPassed = 0, updateTimer = 0, drawTimer = 0;
	cout << "I'm beginning game loop now\n";
	while(IH::Instance()->isGameRunning())
	{
		//handle game timers
		now = SDL_GetTicks();
		msPassed = now - then;
		then = now;
		updateTimer += msPassed;
		drawTimer += msPassed;

		//string name;
		//getline(cin, name, '#');
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