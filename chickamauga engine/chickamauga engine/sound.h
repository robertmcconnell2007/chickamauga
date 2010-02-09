#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "SDL.h"
#include "SDL_mixer.h"
#include <string>
using namespace std;

enum SoundFX
{
	unionMusic, confederateMusic, endGameMusic
};

class sound
{
private:
	int volume;
public:
	sound();
	~sound();
	string *musicArray;
	void playWAV(int);
	//void playMP3(enum);
	bool fillMusic();
	void increaseVolume(int, int);
	void decreaseVolume(int, int);
	void stopMusic(int);
};



//void sound::playMP3(const char *mp3)
//{
//    Mix_Music *music;
//    Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048);
//    music = Mix_LoadMUS(mp3);
//    Mix_VolumeMusic(100); 
//    Mix_PlayMusic(music,-1);
//
//}



