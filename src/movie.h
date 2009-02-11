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

static void ReserveInputBufferSpace(uint32 spaceNeeded);

static int movie_readchar();

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




