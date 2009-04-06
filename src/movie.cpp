/* Mednafen - Multi-system Emulator
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
Functions you are probably looking for:

Stopping a recording: static void StopRecording(void)
Starting a recording - void MDFNI_SaveMovie(char *fname, uint32 *fb, MDFN_Rect *LineWidths)
Starting playback: void MDFNI_LoadMovie(char *fname)
Stopping playback: static void StopPlayback(void)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>
#include <iomanip>
#include <limits.h>
#include <stdarg.h>
#include <sstream>

#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include "unixstuff.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


#include <fstream>
#include <iostream>

#include "mednafen.h"

#include <string.h>
#include <vector>

#include "driver.h"
#include "state.h"
#include "general.h"
#include "video.h"
#include "endian.h"
#include "netplay.h"
#include "movie.h"

#include "endian.h"

#include "drivers/main.h"

struct MovieStruct Movie;

StateMem temporarymoviebuffer;

//movie status stuff

static uint32 RerecordCount;
uint32 FrameCounter;

int readonly = 1; //movie is read only by default

int isMov = 0; //used to create a second set of savestates for nonrecording/nonplayback
//0 = not playing or recording

int current = 0;		// > 0 for recording, < 0 for playback

///////////////////

static FILE* slots[10]={0};

static int RecentlySavedMovie = -1;
static int MovieStatus[10];
static StateMem RewindBuffer;

static uint32 moviedatasize = 0;
char * tempbuffer;


//playback related variables

uint8  md5_of_rom_used[16];
char movieauthor[33];
char MovieMD5Sum[33];
uint32 tempmloc;//contains ftell of a playing movie file, allowing resuming a movie from a playback-made savestate possible
uint32 MoviePlaybackPointer;

///////////////////////

void ResetVariables(void) {

moviedatasize=0;
tempbuffer = (char*) malloc (sizeof(char)*moviedatasize);
memset(&temporarymoviebuffer, 0, sizeof(StateMem));
FrameCounter = 0;
tempmloc=0;
MoviePlaybackPointer=0;

}

//used for starting recording when a state is loaded in read+write playback
void SetCurrent(int incurrent) {

	current=CurrentMovie;
	current++;
}

void ReadHeader(FILE* headertest) { //TODO actual comparisons

	//check file indicator

	//compare mednafen version  //MEDNAFEN_VERSION_NUMERIC
	
	//read32le(version, headertest);

	fseek(headertest, 8, SEEK_SET);

	//compare movie file format version

	uint32 movversion;
	fseek(headertest, 12, SEEK_SET);
	movversion = fgetc(headertest);

	//compare MD5 Sums

	fread(md5_of_rom_used, 1, 16, headertest);
	snprintf(MovieMD5Sum, 16, "%d", md5_of_rom_used);  //for displaying later

	//update rerecords with value from file

	fseek(headertest, 112, SEEK_SET);
	read32le(&RerecordCount, headertest);

	//read console - only useful for counting frames //probably only useful for the site really

	//read author's name

	fseek(headertest, 121, SEEK_SET);
	fread(movieauthor, sizeof(char), 32, headertest);

	//finished
}

void AddRerecordCount(void) { //increment the record counter

	//only if we are in record mode
	if(current > 0)
		RerecordCount++;

}

void WriteHeader(FILE* headertest) {

	//FILE* headertest;

	//smem_seek(&temporarymoviebuffer, 0, SEEK_SET);
	//headertest=fopen("headertest.txt","wb");
	//tempbuffertest3=fopen(MDFN_MakeFName(MDFNMKF_MOVIE,CurrentMovie,0).c_str(),"wb3");

	fseek(headertest, 0, SEEK_SET);

	//file indicator
	//MDFNMOVI

	static char MDFNMOVI[9] = "MDFNMOVI";

	//snprintf(writeauthor, 32, "%s", author.c_str());

	//strlen so that we don't write a bunch of junk to the file

	fwrite(MDFNMOVI, sizeof(char), strlen(MDFNMOVI), headertest);

	//write mednafen version

	write32le(MEDNAFEN_VERSION_NUMERIC, headertest);

	//write movie format version

	uint32 MovieVersion = 1;

	write32le(MovieVersion, headertest);

	//write MD5, Filename of the rom
	//GetMD5AndFilename(headertest);  //up to 64 chars of filename

	snprintf(MovMD5Sum, 16, "%s", MDFNGameInfo->MD5);

	fwrite(MovMD5Sum, sizeof(char), 32, headertest);

	//Filename

	GetMovFileBase(headertest);

	//Rerecords
	write32le(RerecordCount, headertest);


	//console

	static char MovConsole[6];

	snprintf(MovConsole, 6, "%s", CurGame->shortname);

	fwrite(MovConsole, sizeof(char), 5, headertest);


	//author's name

	std::string author = MDFN_GetSettingS("author");

	char writeauthor[32];

	snprintf(writeauthor, 32, "%s", author.c_str());

	//strlen so that we don't write a bunch of junk to the file

	fwrite(writeauthor, sizeof(char), strlen(writeauthor), headertest);

	//some padding


	int j;

	for (j = 0; j < 103; j++) {

		fputc(0, headertest);

	}
}

void MovClearAllSRAM(void) {

	//TODO ZERO
	ClearPCESRAM();

}

int MovieFrameCount;  //total number of frames in a movie that is being played back

//this is for the sake of properly counting frames

//pce defaults
int NumberOfPorts = 5;
uint32 PortDataCacheLength = 2;

void SetNumberOfPorts(void) {

	if (strcmp (CurGame->shortname, "lynx") == 0 || strcmp (CurGame->shortname, "wswan") == 0)
		NumberOfPorts = 1;
	
	if (strcmp (CurGame->shortname, "pcfx") == 0 ) 
		NumberOfPorts = 2;
	
	if (strcmp (CurGame->shortname, "pce")== 0 ) 
		NumberOfPorts = 5;
	if (strcmp (CurGame->shortname, "sms")== 0 ) {
		NumberOfPorts = 2;
		PortDataCacheLength=1;
	}
	if (strcmp (CurGame->shortname, "ngp")== 0 ) {
		NumberOfPorts = 1;
		PortDataCacheLength=1;
	}
	//not really correct, the nes has 5 ports with the last one as length 0 apparently...
	if (strcmp (CurGame->shortname, "nes")== 0 ) {
		NumberOfPorts = 4;
		PortDataCacheLength=1;
	}
}

////////////////////////



void setMoviePlaybackPointer(uint32 value) {

	MoviePlaybackPointer = value;

}

FILE* getSlots() {

	return(slots[-1 - current]);

}


///////////////////////////////////////////
//seek to a particular frame in the movie//
///////////////////////////////////////////


void MDFNMOV_Seek(FILE* fp)
{

	fseek (fp , MoviePlaybackPointer, SEEK_SET );
}

/////////////////////////////////////////
//count the number of frames in a movie//
/////////////////////////////////////////

void MDFNMOV_Count(FILE* fp)
{
	MovieFrameCount = 0;

	int t = 0;

	int moviesize1 = 0;

	SetNumberOfPorts();
	//get the size of the movie

	fseek(fp, 0, SEEK_END);
	moviesize1=ftell (fp);
	rewind(fp);

	//each port increases the size of the movie
	MovieFrameCount = (moviesize1-256) / ((NumberOfPorts*2) + 1); //MovieFrameCount / NumberOfPorts;

//	free(junkbuffer);
}

int getreadonly(void) {

	return(Movie.readonly);
}

int RecordingSwitchToPlayback;

void setreadonly(void) { //it's a toggle

	if(!(current > 0)) { //special case if we are recording

		if(Movie.readonly == 1) {
			Movie.readonly=0;

			//readonly = 0; // read+write
			MDFN_DispMessage((UTF8 *)_("Read+Write"));
		}

		else {

			Movie.readonly=1;
			//readonly = 1;// read onlymovie.cpp:124: error: at this point in file
			MDFN_DispMessage((UTF8 *)_("Read Only"));
		}
	}
	else {
		MDFN_DispMessage((UTF8 *)_("Recording continuing - Load a state to play read-only"));
		RecordingSwitchToPlayback=1;
	}
}

//allows people to start in either readonly or read+write mode by the command line
void setreadonlycli(int value) {

//	readonly = value;
	Movie.readonly=value;

}

//gets the total number of frames to the hud
int DisplayTotalFrames(void) {

	return(MovieFrameCount);
}

//display plackback/recording indicator
//also for indicating whether saveram should be cleared
int MovInd(void) 
{
	if(isMov == 1) 
	{
		if(current > 0 ) //recording
			return(666);	
		
		else  //playback
			return(333);
	}
	else  
		return(111); //not recording or playback 
}


//we want a separate set of savestates for nonrecording/nonplayback so movies don't accidentally get ruined
//this is done by increasing the savestate number so that the normal ones won't be overwritten
int retisMov(void) {

	if(isMov == 0) 
		return(42);  //this could be any number large enough to make the savestates not conflict

	else 
		return(NULL);
}

//framecounter functions

void incFrameCounter(void) {

	FrameCounter++;
}

uint32 retFrameCounter(void) {

	return(FrameCounter);

}

uint32 setFrameCounter(uint32 value) {

	FrameCounter = value;
	//TODO TODO TODO - zero didnt know what this should do
	return 0;
	//TODO TODO TODO - zero didnt know what this should do
}

//used for conditionals in state.cpp depending on whether we are playing back or recording
int checkcurrent(void) {

	return(current);

}



//used for truncating the movie file when a state is saved during playback
uint32 getmloc(void) {

	return(tempmloc);

}


//used in mednafen.cpp for QSimpleCommand and preventing rewinding 
//returns 1 if mdfn is playing back
bool MDFNMOV_IsPlaying(void)
{
	if(current < 0) return(1);
	else return(0);
}

//used in netplay and rewinding code
//returns 1 if mdfn is recording
bool MDFNMOV_IsRecording(void)
{
	if(current > 0) return(1);
	else return(0);
}

int firstopen = 1;

//when a recording is stopped, write the movie file to disk
static void StopRecording(void)
{

	// MDFNMOV_RecordState(); //saves the ending state into the movie file with playback indicator 0x80 - we don't want this to happen
	if(MDFN_StateEvilIsRunning())  //state rewinding
	{
		MDFN_StateEvilFlushMovieLove();
	}
	// fclose(slots[current-1]);
	MovieStatus[current - 1] = 1;
	RecentlySavedMovie = current - 1;
	current=0; // we are not recording
	MDFN_DispMessage((UTF8 *)_("Movie recording stopped."));

	if(RewindBuffer.data)  //get rid of rewind data
	{
		//puts("Oops");
		free(RewindBuffer.data);
		RewindBuffer.data = NULL;
	}

	WriteHeader(Movie.fp);

	fclose(Movie.fp);

	isMov = 0;  //change the hud indicator to stopped
	Movie.status=stopped;

}

void MDFNI_SaveMovie(char *fname, uint32 *fb, MDFN_Rect *LineWidths)
{

	if(Movie.readonly == 1) {
		MDFN_DispMessage((UTF8 *)_("Can't record. Toggle read only first"));
		return;
	}
	//FILE* fp;

	//movies start at frame zero

	if(current < 0)	// Can't save a movie during playback.
		return;

	if(current > 0)	//Stop saving if we are recording. 
	{
		StopRecording();
		return;   memset(&RewindBuffer, 0, sizeof(StateMem));
		RewindBuffer.initial_malloc = 16;
	}

	FrameCounter = 0;
	ResetlagCounter();
	SetNumberOfPorts();

	//memset(&temporarymoviebuffer, 0, sizeof(StateMem));

	memset(&RewindBuffer, 0, sizeof(StateMem));  // init
	RewindBuffer.initial_malloc = 16;

	current=CurrentMovie;

    MakeMovieFilename("w+b");

	// if(!fp) return;

	// MDFNSS_SaveFP(fp, fb, LineWidths);

	// fseek(fp, 0, SEEK_END);


	// fflush(fp, Z_SYNC_FLUSH); // Flush output so that previews will still work right while
	// the movie is being recorded.  Purely cosmetic. :)

	// slots[current] = fp;
	current++;  //Recording



	//start from clean sram

	MovClearAllSRAM();

    fseek(Movie.fp, 256, SEEK_SET);//the header will be written later

	//this actually gets recorded into the movie file

	MDFNI_Power(); //right now, movies always start from power on

	Movie.readonly = 0; //we are Read+Write

	isMov = 1;// use movie specific savestates
	Movie.status=recording;

	MDFN_DispMessage((UTF8 *)_("Movie recording started."));
}

char filename[4096];

void MakeMovieFilename(const char* arg) {

	//record should be w+b
	//play should be r+b

	if(!strcmp(MDFN_GetSettingS("mov").c_str(), "mov PATH NOT SET") == 0){// && firstopen == 1) {

		//if there's a setting, use the designated movie
		if(Movie.fp=fopen(MDFN_GetSettingS("mov").c_str(),arg)) {
			snprintf(filename, 4096, "%s", MDFN_GetSettingS("mov").c_str());
		}
		//firstopen = 0;  //stuff breaks without this
	}
	else
	{
		//we do a default movie name
		Movie.fp=fopen(MDFN_MakeFName(MDFNMKF_MOVIE,CurrentMovie + 666,0).c_str(),arg);
		snprintf(filename, 4096, "%s", MDFN_MakeFName(MDFNMKF_MOVIE,CurrentMovie + 666,0).c_str());
	}

	Movie.filename=filename;

}

static void StopPlayback(void)
{
	if(RewindBuffer.data)
	{
		RewindBuffer.data = NULL;
	}

	fclose(Movie.fp);
	Movie.status=stopped;
	//fclose(slots[-1 - current]);
	current=0;

	isMov = 0;
	ResetlagCounter();

	//free(temporarymoviebuffer.data);

	void ResetVariables(void);

	MDFN_DispMessage((UTF8 *)_("Movie playback stopped."));
}


//used to stop playback and recording when a game is closed, and stop playback during netplay
void MDFNMOV_Stop(void)
{
	if(current < 0) StopPlayback();
	if(current > 0) StopRecording();
}

void MDFNI_LoadMovie(char *fname)
{
//	memset(&temporarymoviebuffer, 0, sizeof(StateMem));

	//free(temporarymoviebuffer.data);

	ResetlagCounter();
//	FILE* fp;
	//puts("KAO");

	if(current > 0)        // Can't interrupt recording.
		return;

	if(MDFNnetplay)	// Playback is UNPOSSIBLE during netplay. 
	{
		MDFN_DispMessage((UTF8*)_("Can't play movies during netplay."));
		return;
	}

	if(current < 0)        // We stop playback when loading a movie. 
	{
		StopPlayback();
		return;
	}

	MakeMovieFilename("r+b") ;
/*
	//make sure the setting isn't the default
	if(!strcmp(MDFN_GetSettingS("mov").c_str(), "mov PATH NOT SET") == 0) {

		//if there's a setting, play from the designated movie
		if(Movie.fp=fopen(MDFN_GetSettingS("mov").c_str(),"r+b")) {
		}
	}
	else
	{
		//do a default movie filename
		Movie.fp=fopen(MDFN_MakeFName(MDFNMKF_MOVIE,CurrentMovie + 666, 0).c_str(),"r+b");
		//fp=fopen("stoprecordingsmemmovie.txt","rb");
	}
*/
	if(!Movie.fp) return;

	//count the number of frames in the movie
	MovieFrameCount = 0;

	MDFNMOV_Count(Movie.fp);

	//currently commented out because I've disabled savestates in movie files

	// if(!MDFNSS_LoadFP(fp)) 
	// {
	//  MDFN_DispMessage((UTF8 *)_("Error loading state portion of the movie."));
	//  return;
	// }

	//first do the header

	ReadHeader(Movie.fp);

	//load the movie file into a buffer

	///get the size

	fseek(Movie.fp, 0, SEEK_END);
	Movie.size=ftell (Movie.fp);
	//moviedatasize = moviedatasize - 256; //subtract length of header
	fseek(Movie.fp, 256, SEEK_SET);  //seek past header

	//copy it

	/////////////////////////

	//movies start at frame zero

	FrameCounter = 0;
	SetNumberOfPorts(); //so we can load a state and continue playback correctly
	current = CurrentMovie;
	//slots[current] = Movie.fp;
	current = -1 - current;
	MovieStatus[CurrentMovie] = 1;
	Movie.status=playback;


	MovClearAllSRAM(); //start from clean sram
	MDFNI_Power(); /////movies always play back from poweron
	isMov = 1;  //use movie specfic savestates
	Movie.readonly = 1;  //we always start read only so that the user can toggle it later

	MDFN_DispMessage((UTF8*)_("Read-Only Playback - Frames: %d Re-records: %d Author: %s MD5: %s"), MovieFrameCount, RerecordCount, movieauthor, MovieMD5Sum);
}


//takes care of either playing back the controller movie data, or recording it

//PDCdata points to PortDataCache, getting the commands from this function out to the emulator, or doing the opposite

// PortDataCache
// PortDataLenCache

//DPCdata and PDClen were originally donutdata and donutlen

int tempcount = 0;
int AddCommand=0;
int CommandAdded=0;

void SetCommandAdded(void) {

	CommandAdded=0;
}

void MDFNMOV_AddJoy(void *PDCdata, uint32 PDClen)
{
//	FILE* fp;

	int t;

	if(!current) return;	/* Not playback nor recording. */
	if(current < 0)	/* Playback */
	{

		//fp = slots[-1 - current];

		//once per frame
		if(CommandAdded==0) {
		t = fgetc(Movie.fp);
		CommandAdded=1;
		if(t>0)
			MDFN_DoSimpleCommand(t);
		}
		//multiple times per frame
		if(fread(PDCdata, 1, PDClen, Movie.fp) != PDClen){
			StopPlayback();
			return;
		}
		//this allows you to save a state during playback and then resume a movie from it
		//tempmloc = ftell(fp);
	}

	else			/* Recording */
	{
		if(MDFN_StateEvilIsRunning())
		{
			smem_putc(&RewindBuffer, 0);
			smem_write(&RewindBuffer, PDCdata, PDClen);
		}
		else
		{
			// fp = slots[current - 1];
			//once per frame
           if(CommandAdded==0) {
			fputc(AddCommand, Movie.fp);
			AddCommand=0;//set the command back to nothing
			CommandAdded=1;
		   }
		   //multiple per frame
		   fwrite(PDCdata, 1, PDClen, Movie.fp);
		}
	}
}

//adds non-controller data commmands like reset and poweron to a movie
void MDFNMOV_AddCommand(int cmd)
{
	if(current <= 0) return;	/* Return if not recording a movie */

	if(MDFN_StateEvilIsRunning())
		smem_putc(&RewindBuffer, 0);
	else
		//fputc(cmd, slots[current - 1]);
		//smem_putc(&temporarymoviebuffer, cmd);
		AddCommand=cmd;
}

//current behavior is saving the state to the movie file so that the state loads during playback
void MDFNMOV_RecordState(void) 
{
	/*
	FILE* fp = slots[current - 1];
	StateMem sm;

	memset(&sm, 0, sizeof(StateMem));  // zero Statemem sm
	MDFNSS_SaveSM(&sm, 0, 0);

	if(MDFN_StateEvilIsRunning())
	{
	smem_putc(&RewindBuffer, MDFNNPCMD_LOADSTATE);
	smem_putc(&RewindBuffer, sm.len & 0xFF);
	smem_putc(&RewindBuffer, (sm.len >> 8) & 0xFF);
	smem_putc(&RewindBuffer, (sm.len >> 16) & 0xFF);
	smem_putc(&RewindBuffer, (sm.len >> 24) & 0xFF);
	smem_write(&RewindBuffer, sm.data, sm.len);
	}
	else
	{
	fputc(fp, MDFNNPCMD_LOADSTATE);
	fputc(fp, sm.len & 0xFF);
	fputc(fp, (sm.len >> 8) & 0xFF);
	fputc(fp, (sm.len >> 16) & 0xFF);
	fputc(fp, (sm.len >> 24) & 0xFF);
	fwrite(slots[current - 1], sm.data, sm.len);
	}
	free(sm.data);
	*/
}

void MDFNMOV_ForceRecord(StateMem *sm) //used during rewinding
{
	printf("Farced: %d\n", sm->len);
	// fwrite(slots[current - 1], sm->data, sm->len);
}

StateMem MDFNMOV_GrabRewindJoy(void)
{
	StateMem ret = RewindBuffer;
	memset(&RewindBuffer, 0, sizeof(StateMem));
	RewindBuffer.initial_malloc = 16;
	return(ret);
}

void MDFNMOV_CheckMovies(void)
{
	time_t last_time = 0;

	for(int ssel = 0; ssel < 10; ssel++)
	{
		struct stat stat_buf;

		MovieStatus[ssel] = 0;
		if(stat(MDFN_MakeFName(MDFNMKF_MOVIE, ssel, 0).c_str(), &stat_buf) == 0)
		{
			MovieStatus[ssel] = 1;
			if(stat_buf.st_mtime > last_time)
			{
				RecentlySavedMovie = ssel;
				last_time = stat_buf.st_mtime;
			}
		}
	}
	CurrentMovie = 0;
}

void MDFNI_SelectMovie(int w)
{
	FILE* fp;
	uint32 MovieShow = 0;
	uint32 *MovieShowPB = NULL;
	uint32 MovieShowPBWidth;
	uint32 MovieShowPBHeight;

	if(w == -1) { MovieShow = 0; return; }
	MDFNI_SelectState(-1);

	CurrentMovie=w;
	MovieShow = MDFND_GetTime() + 2000;;

	fp = fopen(MDFN_MakeFName(MDFNMKF_MOVIE,CurrentMovie,NULL).c_str(), "rb");

	if(fp)
	{
		uint8 header[32];

		//this should be fixed at some point
		//int gzread  (gzFile file, voidp buf, unsigned int len);
		fread(header, 1, 32, fp);
		uint32 width = MDFN_de32lsb(header + 24);
		uint32 height = MDFN_de32lsb(header + 28);

		if(width > 512) width = 512;
		if(height > 512) height = 512;

		{
			uint8 *previewbuffer = (uint8*)alloca(3 * width * height);
			uint8 *rptr = previewbuffer;

			fread(previewbuffer, 1, 3 * width * height, fp);

			if(MovieShowPB)
			{
				free(MovieShowPB);
				MovieShowPB = NULL;
			}

			MovieShowPB = (uint32 *)malloc(4 * width * height);
			MovieShowPBWidth = width;
			MovieShowPBHeight = height;

			for(unsigned int y=0; y < height; y++)
				for(unsigned int x=0; x < width; x++)
				{
					MovieShowPB[x + y * width] = MK_COLORA(rptr[0],rptr[1],rptr[2], 0xFF);
					rptr+=3;
				}
				fclose(fp);
		}
	}
	else
	{
		if(MovieShowPB)
		{
			free(MovieShowPB);
			MovieShowPB = NULL;
		}
		MovieShowPBWidth = MDFNGameInfo->ss_preview_width;
		MovieShowPBHeight = MDFNGameInfo->height;
	}

	StateStatusStruct *status = (StateStatusStruct*)calloc(1, sizeof(StateStatusStruct));

	memcpy(status->status, MovieStatus, 10 * sizeof(int));
	status->current = CurrentMovie;
	status->current_movie = current;
	status->recently_saved = RecentlySavedMovie;
	status->gfx = MovieShowPB;
	status->w = MovieShowPBWidth;
	status->h = MovieShowPBHeight;
	status->pitch = MovieShowPBWidth;

	MDFND_SetMovieStatus(status);
}

struct MovieBufferStruct truncatebuffer;

void TruncateMovie(struct MovieStruct mov) {
	int length;

	//when we resume recording, shorten the mov so that there isn't 
	//potential garbage data at the end

	fseek(mov.fp,0,SEEK_SET);
	truncatebuffer=ReadMovieIntoABuffer(mov.fp);
	fclose(mov.fp);
	
	//clear the file and write it again
	mov.fp=fopen(mov.filename,"wb");
	//fwrite(truncatebuffer.data,256,1,mov.fp);//header
	length=(mov.framelength*FrameCounter)+256;
	fwrite(truncatebuffer.data,1,length,mov.fp);
	fclose(mov.fp);

	mov.fp=fopen(mov.filename,"r+b");
}

void MovieLoadState(void) {

	Movie.headersize=256;

	
	Movie.framelength=PortDataCacheLength*NumberOfPorts;
	Movie.framelength+=1;//the control bit

	if (Movie.readonly == 1 && Movie.status == playback)  {
		//Movie.Status = Playback;
		fseek (Movie.fp,Movie.headersize+FrameCounter * Movie.framelength,SEEK_SET);
	}

	if(Movie.status == recording) {
		fseek (Movie.fp,Movie.headersize+FrameCounter * Movie.framelength,SEEK_SET);
	}

	if(Movie.status == playback && Movie.readonly == 0) {
		Movie.status = recording;
		TruncateMovie(Movie);
		fseek (Movie.fp,Movie.headersize+FrameCounter * Movie.framelength,SEEK_SET);
		SetCurrent(1);
		//RecordingFileOpened=1;
		//strcpy(MovieStatus, "Recording Resumed");
		

	}
}

void ReplaceMovie(FILE* fp) {

	struct MovieBufferStruct tempbuffer;

	//overwrite the main movie on disk if we are recording or read+write playback
	if(Movie.status == recording || (Movie.status == playback && Movie.readonly == 0)) {

		//figure out the size of the movie
		fseek(fp, 0, SEEK_END);
        tempbuffer.size=ftell (fp);
	    rewind(fp);

		//fread(&tempbuffer.size, 4, 1, fp);//size
		tempbuffer.data = (char*) malloc (sizeof(char)*tempbuffer.size);
		fread(tempbuffer.data, 1, tempbuffer.size, fp);//movie
		//fseek(fp, fpos, SEEK_SET);//reset savestate position

		rewind(Movie.fp);
		fwrite(tempbuffer.data, 1, tempbuffer.size, Movie.fp);
	}
}

struct MovieBufferStruct ReadMovieIntoABuffer(FILE* fp) {

	int fpos;
	struct MovieBufferStruct tempbuffer;

	fpos = ftell(fp);//save current pos

	fseek (fp,0,SEEK_END);
	tempbuffer.size=ftell(fp);  //get size
	rewind(fp);

	tempbuffer.data = (char*) malloc (sizeof(char)*tempbuffer.size);
	fread (tempbuffer.data, 1, tempbuffer.size, fp);

	fseek(fp, fpos, SEEK_SET); //reset back to correct pos
	return(tempbuffer);
}

void CopyMovie(FILE* fp) {

	struct MovieBufferStruct tempbuffer;

	if(Movie.status == recording || Movie.status == playback) {
		tempbuffer=ReadMovieIntoABuffer(Movie.fp);//read in the main movie

		//fwrite(&tempbuffer.size, 4, 1, fp);
		fwrite(tempbuffer.data, tempbuffer.size, 1, fp);//and copy it to destination
	}
}

using namespace std;

bool CheckFileExists(const char* filename)
{
	//This function simply checks to see if the given filename exists
	std::string checkFilename; 

    if (FILE * file = fopen(filename, "r")) //I'm sure, you meant for READING =)
    {
        fclose(file);
        return true;
    }
    return false;
}
//const char * backupfilename;

//char * backupfilename;

void FCEUI_MakeBackupMovie(bool dispMessage)
{
	FILE* backupfile;
	

	//This function generates backup movie files
	std::string currentFn;					//Current movie fillename
	std::string backupFn;					//Target backup filename
	std::string tempFn;						//temp used in back filename creation
	stringstream stream;
	int x;								//Temp variable for string manip
	bool exist = false;					//Used to test if filename exists
	bool overflow = false;				//Used for special situation when backup numbering exceeds limit

	currentFn = Movie.filename;		//Get current moviefilename
	backupFn = Movie.filename;		//Make backup filename the same as current moviefilename
	x = backupFn.find_last_of(".");		 //Find file extension
	backupFn = backupFn.substr(0,x);	//Remove extension
	tempFn = backupFn;					//Store the filename at this point
	for (unsigned int backNum=0;backNum<999;backNum++) //999 = arbituary limit to backup files
	{
		stream.str("");					 //Clear stream
		if (backNum > 99)
			stream << "-" << backNum;	 //assign backNum to stream
		else if (backNum <=99 && backNum >= 10)
			stream << "-0";				//Make it 010, etc if two digits
		else
			stream << "-00" << backNum;	 //Make it 001, etc if single digit
		backupFn.append(stream.str());	 //add number to bak filename
		backupFn.append(".bak");		 //add extension

		exist = CheckFileExists(backupFn.c_str());	//Check if file exists
		
		if (!exist) 
			break;						//Yeah yeah, I should use a do loop or something
		else
		{
			backupFn = tempFn;			//Before we loop again, reset the filename
			
			if (backNum == 999)			//If 999 exists, we have overflowed, let's handle that
			{
				backupFn.append("-001.bak"); //We are going to simply overwrite 001.bak
				overflow = true;		//Flag that we have exceeded limit
				break;					//Just in case
			}
		}
	}

//	MovieData md = currMovieData;								//Get current movie data

	//snprintf(backupfilename, 1023, "%s", backupFn);
	//backupfilename = backupFn.c_str ();
	//backupfilename = strdup(backupFn.c_str());


	//std::string str;
	char * backupfilename = new char[backupFn.size() + 1];
	std::copy(backupFn.begin(), backupFn.end(), backupfilename);
	backupfilename[backupFn.size()] = '\0'; // don't forget the terminating 0



	backupfile=fopen(backupfilename, "wb");	//open/create file
    CopyMovie(backupfile);

		// don't forget to free the string after finished using it
	delete[] backupfilename;
	//md.dump(outf,false);										//dump movie data
//	delete outf;												//clean up, delete file object
	
	//TODO, decide if fstream successfully opened the file and print error message if it doesn't

//	if (dispMessage)	//If we should inform the user 
//	{
	//	if (overflow)
	//		FCEUI_DispMessage("Backup overflow, overwriting %s",backupFn.c_str()); //Inform user of overflow
	//	else
	//		FCEUI_DispMessage("%s created",backupFn.c_str()); //Inform user of backup filename
//	}
}




