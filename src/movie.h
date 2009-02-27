#ifndef _MOVIE_H
#define _MOVIE_H

void MDFNI_SelectMovie(int);

#include "movie-driver.h"
#include "state.h"
void MDFNMOV_AddJoy(void *donutdata, uint32 donutlen);
void MDFNMOV_CheckMovies(void);
void MDFNMOV_Stop(void);
void MDFNMOV_AddCommand(int cmd);
bool MDFNMOV_IsPlaying(void);
bool MDFNMOV_IsRecording(void);
void MDFNMOV_ForceRecord(StateMem *sm);
StateMem MDFNMOV_GrabRewindJoy(void);
void MDFNMOV_RecordState();

void LoadMovieCLI(void);

void SetCurrent(int incurrent);


static char MovMD5Sum[33];
static char MovRomFilename[64];

int DisplayTotalFrames(void);

void ReadHeader(FILE* headertest);

void WriteHeader(FILE* headertest);

void MovClearAllSRAM(void);

void SetNumberOfPorts(void);

void setMoviePlaybackPointer(uint32 value);

void MDFNMOV_Seek(FILE* fp);

FILE* getSlots();

void MDFNMOV_Count(FILE* fp);

int getreadonly(void);

//void setreadonly(void);



void AddRerecordCount(void);


int MovInd(void);

int retisMov(void);

static void ReserveInputBufferSpace(uint32 spaceNeeded);

void Stoptemp(void);

void Starttemp(void);

StateMem Grabtempmov(void);

void incFrameCounter(void);

uint32 retFrameCounter(void);

uint32 setFrameCounter(uint32 value);

//static 

void Writetempmov(StateMem in);

static int CurrentMovie = 0;

int checkcurrent(void);

uint32 Getloc(void);

uint32 getmloc(void);




#endif




