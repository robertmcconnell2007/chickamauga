#include "SDL.h"
#include <cmath>
#include <vector>
#include <string>
#include "MersenneTwister.h"
#include "graphicsloader.h"
using namespace std;

SDL_Surface *load_my_image( string filename ) 
{ 
	//Temporary storage for the image that's loaded 
	SDL_Surface* loadedImage = NULL; 
	//The optimized image that will be used 
	SDL_Surface* optimizedImage = NULL;

	//Load the image 
	loadedImage = SDL_LoadBMP( filename.c_str() ); 

	//If nothing went wrong in loading the image 
	if( loadedImage != NULL )
	{ 
		//Create an optimized image 
		optimizedImage = SDL_DisplayFormat( loadedImage ); 
		//Free the old image 
		SDL_FreeSurface( loadedImage ); 
	}

	//Return the optimized image 
	return optimizedImage;
} 

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{ 
	//Make a temporary rectangle to hold the offsets 
	SDL_Rect offset;
	//Give the offsets to the rectangle 
	offset.x = x; offset.y = y; 
	//Blit the surface 
	SDL_BlitSurface( source, NULL, destination, &offset );
} 


void drawATile(SDL_Surface * toDraw, SDL_Rect * drawRect, int index, SDL_Surface * destSurf, int posX, int posY)
{
	//this is used with spritesheets to only print pieces
	int distance = index * drawRect->w;
	SDL_Rect source = {distance % toDraw->w, (distance / toDraw->w) * drawRect->h, drawRect->w, drawRect->h};
	SDL_Rect destination = {posX, posY, 0, 0};
	SDL_BlitSurface(toDraw, &source, destSurf, &destination);
}

int getRandomNum()
{
	MTRand mtrand;
	int random;
	//use multiple mersenne twisters from various ranges and average
	random = (int)((mtrand.rand(12000) + mtrand.rand(4500) + mtrand.rand(17539)) / 3);
	return abs(random);
}

void printStrings(string text, SDL_Rect destRect, SDL_Surface *screen, SDL_Color text_color, TTF_Font *font)
{
	int lineSkip = TTF_FontLineSkip(font);
	int width = 0, height = 10;
	vector<string> vLines;
	string strSub;
	int n = 0, w = 0;
	while(n != -1)
	{
		n = text.find('\n', 0);
		strSub = text.substr(0, n);
		if(n!=-1)
			text = text.substr(n+1, -1);
		vLines.push_back(strSub);
		TTF_SizeText(font, strSub.c_str(), &w, &height);
		if(w > width)
			width = w;
	}
	height = (vLines.size() - 1) * lineSkip + height;
	SDL_Surface * sTemp = NULL;
	SDL_Rect tempStrRect;
	for(int i = 0; i < vLines.size(); ++i)
	{
		sTemp = TTF_RenderText_Solid(font, vLines[i].c_str(), text_color);
		tempStrRect.x = destRect.x;
		tempStrRect.y = destRect.y + (i * lineSkip);
		SDL_BlitSurface(sTemp, NULL, screen, &tempStrRect);
		SDL_FreeSurface(sTemp);
	}
}

void writeText(SDL_Surface * ascii, SDL_Rect * asciiSize, SDL_Surface * screen, string output, int positionX, int positionY)
{
	for(int i = 0; i < (unsigned)output.length(); ++i)
	{
		drawATile(ascii, asciiSize, (int)(output[i]), screen, positionX, positionY);
		positionX += asciiSize->w;
		if(positionX >= screen->w)
		{
			positionX = 0;
			positionY += asciiSize->h;
		}
		if(positionY > screen->h)
			positionY = 0;
	}
}

void writeToConsole(SDL_Surface * ascii, SDL_Rect * asciiSize, SDL_Surface * screen, string output, SDL_Rect * console)
{
	int tempPosX = console->x;
	int tempPosY = console->y;
	for(int i = 0; i < output.length(); ++i)
	{
		if(output[i] == '^' && (i + 2) < output.length())
		{
			switch(output[i + 1])
			{
			case 'N':
			case 'n':
				tempPosX = console->x;
				tempPosY += asciiSize->h;
				break;
			case 'R':
			case 'r':
				tempPosX = console->x;
			case 'T':
			case 't':
				if(tempPosX + (5 * asciiSize->w) < console->w)
					tempPosX += (5 * asciiSize->w);
				else
				{
					tempPosX = console->x + (5 * asciiSize->w);
					tempPosY += asciiSize->h;
				}
				break;
			case 'B':
			case 'b':
				if(tempPosX > 0)
					tempPosX -= asciiSize->w;				
				break;
			}
			i += 2;
		}
		drawATile(ascii, asciiSize, (int)(output[i]), screen, tempPosX, tempPosY);
		tempPosX += asciiSize->w;
		if(tempPosX >= console->x + console->w)
		{
			tempPosX = console->x;
			tempPosY += asciiSize->h;
		}
	}
}
////////OBSOLETE CODE FROM PREVIOUS PROJECTS//////////////
//////////// USE FOR REFERENCE MATERIAL //////////////////

/*

void merge(int arr[], int low, int mid, int high, int max)
{
	
	int i, j, k;
	int * c = new int[max];
	i = low;
	j = mid + 1;
	k = low;
	while((i <= mid) && (j <= high))
	{
		if(arr[i] < arr[j])
		{
			c[k] = arr[i];
			k++;
			i++;
		}
		else
		{
			c[k] = arr[j];
			k++;
			j++;
		}
	}
	while(i <= mid)
	{
		c[k] = arr[i];
		k++;
		i++;
	}
	while(j <= high)
	{
		c[k] = arr[j];
		k++;
		j++;
	}
	for(i = low; i < k; ++i) 
	{
		arr[i] = c[i];
	}
	delete c;
	
}

void merge_sort(int arr[], int low, int high, int max)
{
	int mid;
	if(low < high)
	{
		mid = (int)((low + high) / 2);
		merge_sort(arr, low, mid, max);
		merge_sort(arr, mid +1, high, max);
		merge(arr, low, mid, high, max);
	}
}



bool gameCanEnd(Info_Handler & gameInfo)
{
	DeckOfCards tempP1;
	tempP1 = gameInfo.player1;
	DeckOfCards tempP2;
	tempP2 = gameInfo.player2;
	int played[52] = {-1}, numDrawn = 0, roundOver = 0, k = 0;
	for(int i = 0; i < MAX_NUM_ROUNDS; ++i)
	{
		if(roundOver)
		{
			for(int j = 0; j < 52; ++j)
				played[j] = -1;
			numDrawn = 0;
			roundOver = 0;
			k = 0;
		}
		if(tempP1.getNumOf() < 1 || tempP2.getNumOf() < 1)
			return true;
		played[k] = tempP1.drawACard();
		played[k + 1] = tempP2.drawACard();	
		numDrawn += 2;
		if(played[k] % 13 > played[k + 1] % 13)
		{
			for(int j = 0; j < numDrawn; ++j)			
				tempP1.insertCard(played[j], 0);
			roundOver = 1;
		}
		else if(played[k] % 13 < played[k + 1] % 13)
		{
			for(int j = 0; j < numDrawn; ++j)			
				tempP2.insertCard(played[j], 0);
			roundOver = 1;
		}
		else
		{
			if(tempP1.getNumOf() >=  4 && tempP2.getNumOf() >= 4)
			{
				played[k + 2] = tempP1.drawACard();
				played[k + 3] = tempP1.drawACard();
				played[k + 4] = tempP1.drawACard();
				played[k + 5] = tempP2.drawACard();
				played[k + 6] = tempP2.drawACard();
				played[k + 7] = tempP2.drawACard();
				numDrawn += 6;
			}
			else if(tempP1.getNumOf() < 4 || tempP2.getNumOf() < 4)
				return true;
		}
		k += 8;
	}
	return false;
}

void resetGame(Info_Handler & gameInfo)
{
	DeckOfCards tempDeck;
	gameInfo.player1.killDeck();
	gameInfo.player2.killDeck();
	for(int i = 0; i < 52; ++i)
	{
		tempDeck.addCard(i);		
	}
	tempDeck.shuffle();
	for(int i = 0; i < 26; ++i)
	{
		gameInfo.player1.addCard(tempDeck.drawACard());
		gameInfo.player2.addCard(tempDeck.drawACard());
	}
	gameInfo.counter = 0;
	gameInfo.gameOver = 0;
	gameInfo.isPaused = true;
	gameInfo.canClick = true;
	if(!gameCanEnd(gameInfo))
		resetGame(gameInfo);
}

int playRound(Info_Handler & gameInfo, SDL_Surface * screen, 
			  SDL_Surface * cards, SDL_Rect * cardRect,
			  SDL_Surface * winLogo, SDL_Rect * winRect)
{
	gameInfo.canClick = false;
	int played[52] = {-1}, i = 0, numDrawn = 0;
	int k = 0;
	int warOver = 0;	
	while(!warOver)
	{
		if(gameInfo.player1.getNumOf() < 1)
		{
			gameInfo.canClick = true;
			return -1;
		}
		if(gameInfo.player2.getNumOf() < 1)
		{
			gameInfo.canClick = true;
			return 1;
		}
		played[i] = gameInfo.player1.drawACard();
		played[i + 1] = gameInfo.player2.drawACard();	
		numDrawn += 2;
		if(played[i] % 13 > played[i + 1] % 13)
			warOver = 1;
		else if(played[i] % 13 < played[i + 1] % 13)
			warOver = -1;
		else
		{
			if(gameInfo.player1.getNumOf() >=  4 && gameInfo.player2.getNumOf() >= 4)
			{
				played[i + 2] = gameInfo.player1.drawACard();
				played[i + 3] = gameInfo.player1.drawACard();
				played[i + 4] = gameInfo.player1.drawACard();
				played[i + 5] = gameInfo.player2.drawACard();
				played[i + 6] = gameInfo.player2.drawACard();
				played[i + 7] = gameInfo.player2.drawACard();
				numDrawn += 6;
			}
			else if(gameInfo.player1.getNumOf() < 4 && gameInfo.player2.getNumOf() < 4)
				warOver = 3;
			else if(gameInfo.player1.getNumOf() < 4)
				warOver = -2;
			else
				warOver =  2;
		}
		k = 0;
		for(int j = 0; j < numDrawn; ++j)
		{
			switch(j % 8)
			{
			case 0:
				drawATile(cards, cardRect, played[j], screen, screen->w / 2 + k * 8, screen->h / 2 - 24);
				break;
			case 1:
				drawATile(cards, cardRect, played[j], screen, screen->w / 2 + (k - 1) * 8, screen->h / 2 + 24);
				break;
			case 2:
			case 3:
			case 4:
				drawATile(cards, cardRect, 52, screen, (j * 20), (screen->h / 2) - 24 - (k * 24));
				break;
			case 5:
			case 6:
			case 7:
				drawATile(cards, cardRect, 52, screen, (j * 20), (screen->h / 2) + 24 + (k * 24));
				break;
			}
			++k;
		}
		SDL_Flip(screen);		
		SDL_Delay(1);
		i += 8;
	}
	k = 0;
	for(int j = 0; j < numDrawn; ++j)
	{
		switch(j % 8)
		{
		case 0:
			drawATile(cards, cardRect, played[j], screen, screen->w / 2 + j * 10, screen->h / 2 - 24);
			break;
		case 1:
			drawATile(cards, cardRect, played[j], screen, screen->w / 2 + (j - 1) * 10, screen->h / 2 + 24);
			break;
		case 2:
		case 3:
		case 4:
			drawATile(cards, cardRect, played[j], screen, (j * 20), screen->h / 2 - 24 - (j * 24));
			break;
		case 5:
		case 6:
		case 7:
			drawATile(cards, cardRect, played[j], screen, (j * 20), screen->h / 2 + 24 + (j * 24));
			break;
		}
		++k;
	}
	if(warOver == 1)
		drawATile(winLogo, winRect, 0, screen, screen->w - 256, 32);
	if(warOver == -1)
		drawATile(winLogo, winRect, 0, screen, screen->w - 256, screen->h - 96);
	SDL_Flip(screen);		
	SDL_Delay(2);
	for(int j = 0; j < numDrawn; ++j)
	{
		if(warOver == 1)
			gameInfo.player1.insertCard(played[j], 0);
		else if(warOver == -1)
			gameInfo.player2.insertCard(played[j], 0);
	}
	if(warOver == -2)
	{
		gameInfo.canClick = true;
		return -1;
	}
	else if(warOver == 2)
	{
		gameInfo.canClick = true;
		return 1;
	}
	else if(warOver == 3)
	{
		gameInfo.canClick = true;
		return 2;
	}
	gameInfo.canClick = true;
	return 0;
}



void eventHandler(SDL_Event event, Info_Handler & gameInfo, 
				  SDL_Surface * screen, SDL_Surface * cards, SDL_Rect * cardRect,
				  SDL_Surface * winLogo, SDL_Rect * winRect,
				  SDL_Surface * ascii, SDL_Rect * asciiRect)
{
	if(event.type == SDL_QUIT)
		gameInfo.keepGoing = false;
	if(event.type == SDL_KEYDOWN)
	{
		switch(event.key.keysym.sym)
		{
		case SDLK_SPACE:
			if(gameInfo.canClick)
			{
				if(!gameInfo.gameOver)
					gameInfo.isPaused = !gameInfo.isPaused;
				else
				{
					writeText(ascii, asciiRect, screen, "Calculating New Game", screen->w / 2 - 80, screen->h / 2 - 6);
					SDL_Flip(screen);
					resetGame(gameInfo);				
				}
			}
			break;
		}
	}
	if( event.type == SDL_MOUSEBUTTONDOWN ) 
	{
		if(event.button.button == SDL_BUTTON_LEFT) 
		{
			//pause/unpause game buttons
			if(event.motion.x <= 64 && event.motion.y <= 64 && !gameInfo.gameOver)
			{
				gameInfo.isPaused = !gameInfo.isPaused;
			}
			//yes restart game button
			if(event.motion.x <= 64 && event.motion.y >= 132 && event.motion.y <= 195)
			{
				if(gameInfo.gameOver)
				{
					writeText(ascii, asciiRect, screen, "Calculating New Game", screen->w / 2 - 80, screen->h / 2 - 6);
					SDL_Flip(screen);
					resetGame(gameInfo);
				}
			}
			//no don't restart game button
			if(event.motion.x <= 64 && event.motion.y >= 195 && event.motion.y <= 259)
			{
				if(gameInfo.gameOver)
				{
					gameInfo.keepGoing = false;
				}
			}
		}
	}


}


int findDeckSpot(Info_Handler & gameInfo, int xMouse, int yMouse)
{
	int xSpot = 0, ySpot = 0, deckSpot = -1;
	if(gameInfo.deck.getNumOf() < 1)
		return -1;
	for(int i = 0; i <= ((gameInfo.deck.getNumOf() - 1) / 13); ++i)
	{
		if(yMouse <= i * 31 + 106 && yMouse >= i * 31 + 75)
		{
			if(i == ((gameInfo.deck.getNumOf() - 1) / 13) && xMouse <= ((gameInfo.deck.getNumOf() % 13) * 19))
			{
				deckSpot = (i * 13) + (xMouse / 19);
			}
			else if(xMouse <= 247)
			{
				deckSpot = (i * 13) + (xMouse / 19);
			}
		}
	}
	if(deckSpot >= gameInfo.deck.getNumOf())
		return -1;
	return deckSpot;
}

int findHandSpot(Info_Handler & gameInfo, int xMouse, int yMouse)
{
	int xSpot = 0, ySpot = 0, deckSpot = -1;
	if(gameInfo.hand.getNumOf() < 1)
		return -1;
	for(int i = 0; i <= ((gameInfo.hand.getNumOf() - 1) / 13); ++i)
	{
		if(yMouse <= i * 31 + 260 && yMouse >= i * 31 + 230)
		{
			if(i == ((gameInfo.hand.getNumOf() - 1) / 13) && xMouse <= ((gameInfo.hand.getNumOf() % 13) * 19))
			{
				deckSpot = (i * 13) + (xMouse / 19);
			}
			else if(xMouse <= 247)
			{
				deckSpot = (i * 13) + (xMouse / 19);
			}
		}
	}
	if(deckSpot >= gameInfo.hand.getNumOf())
		return -1;
	return deckSpot;
}


	
	if(event.type == SDL_QUIT)
		gameInfo.keepGoing = false;
	if(event.type == SDL_KEYDOWN)
	{
		SDL_FillRect(screen, &screen->clip_rect, 0xffffff);
		switch(event.key.keysym.sym)
		{
		case SDLK_SPACE:
			gameInfo.deck.shuffle();
			break;
		case SDLK_RIGHT:
			gameInfo.hand.addCard(gameInfo.deck.drawACard());
			break;
		case SDLK_LEFT:
			gameInfo.deck.insertion_Sort();
			break;
		case SDLK_UP:
			gameInfo.deck.merge_Sort_Start();
			break;
		case SDLK_DOWN:
			gameInfo.numRevealedCards++;
			break;
		}
	}
	if( event.type == SDL_MOUSEBUTTONDOWN ) 
	{
		if(event.button.button == SDL_BUTTON_LEFT) 
		{
			if(event.motion.x >= SCREEN_WIDTH - 64)
			{
				if(event.motion.y <= 63)
				{
					gameInfo.deck.shuffle();
				}
				else if(event.motion.y <= 127)
				{
					gameInfo.deck.insertion_Sort();
				}
				else if(event.motion.y <= 191)
				{
					gameInfo.deck.merge_Sort_Start();
				}
				else if(event.motion.y <= 257)
				{
					if(gameInfo.numRevealedCards < gameInfo.deck.getNumOf())
						gameInfo.numRevealedCards++;
				}
				else if(event.motion.y <= 321)
				{
					if(gameInfo.numRevealedCards > 0)
						gameInfo.numRevealedCards--;
				}
				else if(event.motion.y <= 385)
				{
					gameInfo.hand.addCard(gameInfo.deck.drawACard());
				}
				else if(event.motion.y <= 449)
				{
					if(gameInfo.hand.getNumOf() > 0)
						gameInfo.deck.insertCard(gameInfo.hand.drawACard(), gameInfo.discardSlot);
				}
			}
			else if(event.motion.x <= 64 && event.motion.y >= SCREEN_HEIGHT - 64)
			{
				gameInfo.discardSlot--;
			}
			else if(event.motion.x >= 250 && event.motion.x <= 314 && event.motion.y >= SCREEN_HEIGHT - 64)
			{
				gameInfo.discardSlot++;
			}

			//picking up a card from the deck logic
			else if(event.motion.x >= 0 && event.motion.x <= 246 
				&& event.motion.y >= 75 && event.motion.y <= 198)
			{
				int temp = findDeckSpot(gameInfo, event.motion.x, event.motion.y);
				if(temp != -1 && !gameInfo.holdingCard)
				{
					gameInfo.heldCard = gameInfo.deck.getCard(temp);
					gameInfo.deck.deleteNode(temp);
					gameInfo.holdingCard = true;
				}
			}
			//picking up a card from the hand logic
			else if(event.motion.x >= 0 && event.motion.x <= 246
				&& event.motion.y >= 230 && event.motion.y <= 353)
			{
				int temp = findHandSpot(gameInfo, event.motion.x, event.motion.y);
				if(temp != -1 && !gameInfo.holdingCard)
				{
					gameInfo.heldCard = gameInfo.hand.getCard(temp);
					gameInfo.hand.deleteNode(temp);
					gameInfo.holdingCard = true;
				}
			}
			if(gameInfo.discardSlot < 0)
				gameInfo.discardSlot = 0;
			if(gameInfo.discardSlot >= gameInfo.deck.getNumOf())
				gameInfo.discardSlot = gameInfo.deck.getNumOf() - 1;
		}
	}
	if(event.type == SDL_MOUSEBUTTONUP)
	{
		if(event.button.button == SDL_BUTTON_LEFT)
		{
			if(gameInfo.holdingCard && 
				event.motion.x >= 0 && event.motion.x <= 246 && 
				event.motion.y >= 75 && event.motion.y <= 198)
			{
				int temp = findDeckSpot(gameInfo, event.motion.x, event.motion.y);
				if(temp!= -1)
				{
					gameInfo.deck.insertCard(gameInfo.heldCard, temp);
					gameInfo.heldCard = -1;
					gameInfo.holdingCard = false;
				}
				else
				{
					gameInfo.deck.addCard(gameInfo.heldCard);
					gameInfo.heldCard = -1;
					gameInfo.holdingCard = false;
				}
			}
			else if(gameInfo.holdingCard && 
				event.motion.x >= 0 && event.motion.x <= 246 && 
				event.motion.y >= 230 && event.motion.y <= 353)
			{
				int temp = findHandSpot(gameInfo, event.motion.x, event.motion.y);
				if(temp != -1)
				{
					gameInfo.hand.insertCard(gameInfo.heldCard, temp);
					gameInfo.heldCard = -1;
					gameInfo.holdingCard = false;
				}
				else
				{
					gameInfo.hand.addCard(gameInfo.heldCard);
					gameInfo.heldCard = -1;
					gameInfo.holdingCard = false;
				}
			}
			else if(gameInfo.holdingCard)
			{
				gameInfo.deck.addCard(gameInfo.heldCard);
				gameInfo.heldCard = -1;
				gameInfo.holdingCard = false;
			}
		}
	}
}

void array_handler(Info_Handler & info)
{
	int total = 0;
	for(int i = 0; i < info.m_numOfRoller; ++i)
	{
		total += (getRandomNum() % info.m_sizeOfRoller + 1);
	}
	total -= info.m_numOfRoller;
	info.m_intArray[total]++;
}

void drawHistogram(Info_Handler & info, SDL_Surface * screen, SDL_Rect graphRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
	int total = 0;			
	for(int i = 0; i < info.m_numElements; ++i)
		total += info.m_intArray[i];
	
	for(int i = 0; i < info.m_numElements; ++i)
	{
		graphRect.w = (SCREEN_WIDTH / (double)info.m_numElements) + 1;
		if(info.m_intArray[i] != 0 && total != 0)
			graphRect.h = ((double)info.m_intArray[i] / (double)total) * info.m_numOfRoller * 2 * SCREEN_HEIGHT;
		else
			graphRect.h = 0;
		graphRect.x = (i * (SCREEN_WIDTH / (double)info.m_numElements));
		graphRect.y = SCREEN_HEIGHT - graphRect.h;
		SDL_FillRect(screen, &graphRect, 0xff0000);
	}			
	info.m_iterationsRun++;
}

int calcGreatestProb(Info_Handler & info)
{
	double total = 0;
	double greatest = 0;
	for(int i = 0; i < info.m_numElements; ++i)
	{
		total += (double)info.m_intArray[i];
		if(info.m_intArray[i] > greatest)
			greatest = (double)info.m_intArray[i];
	}
	if(total != 0)
		return (int)((greatest / total) * 100);
	return 0;	
}

void eventHandler(SDL_Event event, Info_Handler & info, SDL_Surface * screen, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
	if(event.type == SDL_KEYDOWN)
	{
		switch(event.key.keysym.sym)
		{
		case SDLK_SPACE:
			info.m_keepIterating = !info.m_keepIterating;
			if(info.m_keepIterating)
			{
				SDL_FillRect( screen, &screen->clip_rect, 0xffffff );
				info.set_Array();						
			}
			break;
		case SDLK_LEFT:
			info.m_keepIterating = false;
			if(info.m_sizeOfRoller > 1)
				info.m_sizeOfRoller--;
			break;
		case SDLK_RIGHT:
			info.m_keepIterating = false;
			info.m_sizeOfRoller++;
			break;
		case SDLK_UP:
			info.m_keepIterating = false;
			info.m_numOfRoller++;
			break;
		case SDLK_DOWN:
			info.m_keepIterating = false;
			if(info.m_numOfRoller > 1)
				info.m_numOfRoller--;
			break;
		}
	}
	if( event.type == SDL_MOUSEBUTTONDOWN ) 
	{
		if( event.button.button == SDL_BUTTON_LEFT ) 
		{
			info.m_keepIterating = false;
			if(event.motion.x <= 64)
			{
				if(event.motion.y <= 64)
				{
					info.m_keepIterating = false;
					if(info.m_numOfRoller > 1)
						info.m_numOfRoller--;					
				}
				else if(event.motion.y <= 128)
				{
					info.m_keepIterating = false;
					if(info.m_sizeOfRoller > 1)
						info.m_sizeOfRoller--;
				}
			}
			else if(event.motion.x >= SCREEN_WIDTH -64)
			{
				if(event.motion.y <= 64)
				{
					info.m_keepIterating = false;
					info.m_numOfRoller++;
				}
				else if(event.motion.y <= 128)
				{
					info.m_keepIterating = false;
					info.m_sizeOfRoller++;
				}
			}
			else if(event.motion.x >= SCREEN_WIDTH / 2 - 28 && 
				event.motion.x <= SCREEN_WIDTH / 2 + 36 &&
				event.motion.y >= 32 && 
				event.motion.y <= 96)
			{
				info.m_keepIterating = !info.m_keepIterating;
				if(info.m_keepIterating)
				{
					SDL_FillRect( screen, &screen->clip_rect, 0xffffff );
					info.set_Array();						
				}
			}
		}
	}
}


*/