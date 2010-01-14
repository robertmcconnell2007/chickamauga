#include <iostream>
#include "Game Data Handler.h"
#include "tools.h"
#include "rules.h"
using namespace std;


void IH::handlePrimaryInput()
{
	switch(IH::Instance()->event.type)
	{
	case SDL_QUIT:
		IH::Instance()->endGame();

		break;

	case SDL_KEYDOWN:
		switch(IH::Instance()->event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			IH::Instance()->endGame();
			break;
		case SDLK_SPACE:
			cout << "OMG\n";
			break;
		default:
			break;
		}
		break;
	}
}

void IH::update(int mspassed)
{
	if(playingMatch)
	{
	}
	else
	{
	}
}

void IH::drawAll()
{
	if(playingMatch)
	{
		
	}
	else
	{
	}
}