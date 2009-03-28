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

struct MovieStruct
{
	int status;
	FILE *fp;
	int readonly;
	int rerecords;
	int size;
	int frames;
	char* filename;
	int framelength;
	int headersize;
};

#define stopped	  1
#define recording 2
#define playback  3

struct MovieBufferStruct
{
	int size;
	char* data;
};

extern struct MovieStruct Movie;

void LoadMovieCLI(void);

void ResetVariables(void);

void setreadonlycli(int value);

void SetCurrent(int incurrent);
 
static char MovMD5Sum[33];
static char MovRomFilename[64];

void SetCommandAdded(void);

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

extern int current;

void AddRerecordCount(void);

extern int RecordingSwitchToPlayback;

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


void MovieLoadState(void);

void ReplaceMovie(FILE* fp);

void CopyMovie(FILE* fp);

struct MovieBufferStruct ReadMovieIntoABuffer(FILE* fp);

void OpenMovie(const char * arg);

void MakeMovieFilename(const char* arg);

#endif




