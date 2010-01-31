#include "sound.h"

sound::sound(){fillMusic(); volume = 100;}
sound::~sound(){}

bool sound::fillMusic()
{
	musicArray = new string[2];
	musicArray[0] = "ourSound/paddy.wav";
	musicArray[1] = "ourSound/ConfederateBattleMusic.wav";
	return true;
}
void sound::playWAV(int wav)
{
    Mix_Chunk *music;
    Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048);
    music = Mix_LoadWAV(musicArray[wav].c_str());
	Mix_Volume(wav, volume);
    Mix_PlayChannel(wav,music,-1);
}
void sound::increaseVolume(int chan, int amount)
{
	volume+= amount;
	Mix_Volume(chan,volume);
}
void sound::decreaseVolume(int chan, int amount)
{
	volume-= amount;
	Mix_Volume(chan,volume);
}
void sound::stopMusic(int chan)
{
	Mix_Pause(chan);

}