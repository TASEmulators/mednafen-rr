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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "mednafen.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <vector>

#include "driver.h"
#include "state.h"
#include "general.h"
#include "video.h"
#include "endian.h"
#include "netplay.h"
#include "movie.h"

uint32 FrameCounter;

uint32 tempmloc;

StateMem temporarymoviebuffer;

int isMov = 0; //used to create a second set of savestates for nonrecording/nonplayback
//0 = not playing or recording

static int current = 0;		// > 0 for recording, < 0 for playback
static FILE* slots[10]={0};


static int RecentlySavedMovie = -1;
static int MovieStatus[10];
static StateMem RewindBuffer;

static uint8* moviedata = NULL;
static uint32 moviedatasize = 0;
static uint32 frameptr = 0;

char * tempbuffer;

int retisMov(void) {

std::cout << "isMov " << isMov <<std::endl;

//we want a separate set of savestates for nonrecording/nonplayback so movies don't accidentally get ruined
if(isMov == 0) {

return(42);  //this could be any number, doesn't matter

}

else {

return(NULL);

}

}


void incFrameCounter(void) {

FrameCounter++;

}

uint32 retFrameCounter(void) {

return(FrameCounter);

}

//used for conditionals in state.cpp depending on whether we are playing back or recording
int checkcurrent(void) {

return(current);

}


uint32 setFrameCounter(uint32 value) {

FrameCounter = value;

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


//this is useful for manipulating the moviebuffer in state.cpp
StateMem Grabtempmov(void) {

 StateMem ret = temporarymoviebuffer;
 return(ret);


}


void Writetempmov(StateMem in) {

temporarymoviebuffer = in;

}




static void StopRecording(void)
{
// MDFNMOV_RecordState(); //saves the ending state into the movie file with playback indicator 0x80 - we don't want this to happen
 if(MDFN_StateEvilIsRunning())  //state rewinding
 {
  MDFN_StateEvilFlushMovieLove();
 }
 fclose(slots[current-1]);
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
//////

FILE* tempbuffertest3;

smem_seek(&temporarymoviebuffer, 0, SEEK_SET);
tempbuffertest3=fopen("stoprecordingsmemmovie.txt","wb");
//tempbuffertest3=fopen(MDFN_MakeFName(MDFNMKF_MOVIE,CurrentMovie,0).c_str(),"wb3");
fwrite(temporarymoviebuffer.data, 1, temporarymoviebuffer.len, tempbuffertest3);

fclose(tempbuffertest3);

/////

isMov = 0;

}

void MDFNI_SaveMovie(char *fname, uint32 *fb, MDFN_Rect *LineWidths)
{
 FILE* fp;

//movies start at frame zero

FrameCounter = 0;

 if(current < 0)	// Can't save a movie during playback.
  return;

 if(current > 0)	//Stop saving if we are recording. 
 {
  StopRecording();
  return;   memset(&RewindBuffer, 0, sizeof(StateMem));
 RewindBuffer.initial_malloc = 16;
 }

 memset(&RewindBuffer, 0, sizeof(StateMem));  // init
 RewindBuffer.initial_malloc = 16;

 current=CurrentMovie;

 if(fname)  //if a filename was given in the arguments, use that
  fp = fopen(fname, "wb3");
 else
 {
 // fp=fopen(MDFN_MakeFName(MDFNMKF_MOVIE,CurrentMovie,0).c_str(),"wb3"); 
fp=fopen("junk.txt","wb3"); 
//MakeFName is a generic function used for naming movies, states, etc filenames - MDFNMKF_MOVIE is the type specified here - CurrentMovie is the number for the filename (state 1, 3, 4, etc) - c_str makes it null terminated
 }

 if(!fp) return;

 MDFNSS_SaveFP(fp, fb, LineWidths);
 fseek(fp, 0, SEEK_END);
//commented out for the moment
// fflush(fp, Z_SYNC_FLUSH); // Flush output so that previews will still work right while
			    // the movie is being recorded.  Purely cosmetic. :)
 slots[current] = fp;
 current++;  //Recording

///////////////////

//this actually gets recorded into the movie file

 MDFNI_Power(); //right now, movies always start from power on

//////////////////


isMov = 1;

 MDFN_DispMessage((UTF8 *)_("Movie recording started."));
}


static void StopPlayback(void)
{
 if(RewindBuffer.data)
 {
  RewindBuffer.data = NULL;
 }

 fclose(slots[-1 - current]);
 current=0;

isMov = 0;

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
 FILE* fp;
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

 if(fname)
  fp = fopen(fname, "rb");
 else
 {
 // fp=fopen(MDFN_MakeFName(MDFNMKF_MOVIE,CurrentMovie,0).c_str(),"rb");
fp=fopen("stoprecordingsmemmovie.txt","rb");
 }

 if(!fp) return;

//currently commented out because I've disabled savestates in movie files

// if(!MDFNSS_LoadFP(fp)) 
// {
//  MDFN_DispMessage((UTF8 *)_("Error loading state portion of the movie."));
//  return;
// }


/////////////////////
/////////////////////
/////////////////////

//load the movie file into a buffer

///get the size

fseek(fp, 0, SEEK_END);
moviedatasize=ftell (fp);
rewind(fp);

//copy it

tempbuffer = (char*) malloc (sizeof(char)*moviedatasize);

fread (tempbuffer,1,moviedatasize,fp);
rewind(fp);


//test temp buffer
//this is debugging junk

FILE* tempbuffertest;

tempbuffertest=fopen("tempbuffertest.txt","wb");
fwrite(tempbuffer, 1, moviedatasize, tempbuffertest);

fclose(tempbuffertest);


////////////////////////
////////////////////////
////////////////////////

//writing it to a statemem



memset(&temporarymoviebuffer, 0, sizeof(StateMem));
temporarymoviebuffer.initial_malloc = moviedatasize;

//smem_write(&temporarymoviebuffer, sm.data, sm.len);
smem_write(&temporarymoviebuffer, tempbuffer, moviedatasize);


//gzwrite(slots[current - 1], sm->data, sm->len);



//test temp buffer 2 
// more debugging junk

FILE* tempbuffertest2;

tempbuffertest2=fopen("smembuffertest.txt","wb");
fwrite(temporarymoviebuffer.data, 1, temporarymoviebuffer.len, tempbuffertest2);
std::cout << "temporarymbuflen" << temporarymoviebuffer.len <<std::endl;

fclose(tempbuffertest2);


//fwrite(

smem_seek(&temporarymoviebuffer, 0, SEEK_SET);


/////////////////////////

//movies start at frame zero

FrameCounter = 0;





 current = CurrentMovie;
 slots[current] = fp;

 current = -1 - current;
 MovieStatus[CurrentMovie] = 1;


/////movies always play back from poweron

 MDFNI_Power();

///////

isMov = 1;

 MDFN_DispMessage((UTF8*)_("Movie playback started."));
}



//takes care of either playing back the controller movie data, or recording it

//PDCdata points to PortDataCache, getting the commands from this function out to the emulator, or doing the opposite

// PortDataCache
// PortDataLenCache

//DPCdata and PDClen were originally donutdata and donutlen


int tempcount = 0;

void MDFNMOV_AddJoy(void *PDCdata, uint32 PDClen)
{
 FILE* fp;

 int t;

 if(!current) return;	/* Not playback nor recording. */
 if(current < 0)	/* Playback */
 {

  fp = slots[-1 - current];


//while((t = smem_getc(temporarymoviebuffer)) >=0 &&t)
  while((t = fgetc(fp)) >= 0 && t)  //slots[currrent] is a file pointer,   //t must always be greater or equal to zero, and t must be ?

  {

   if(t == MDFNNPCMD_LOADSTATE) //takes care of embedded savestates in movies, commented out
   {
  //  uint32 len;
  //  StateMem sm;
  //  len = fgetc(fp);
  //  len |= fgetc(fp) << 8;
  //  len |= fgetc(fp) << 16;
  //  len |= fgetc(fp) << 24;
  //  if(len >= 5 * 1024 * 1024) // A sanity limit of 5MiB
  //  {
  //   StopPlayback();
  //   return;
  //  }
  //  memset(&sm, 0, sizeof(StateMem));
  //  sm.len = len;
   // sm.data = (uint8 *)malloc(len);
  //  if(fread(fp, sm.data, len) != len)
  //  {
  //   StopPlayback();
  //   return;
  //  }
  //  if(!MDFNSS_LoadSM(&sm, 0, 0))
  //  {
  //   StopPlayback();
  //   return;
  //  }
   }
   else
    MDFN_DoSimpleCommand(t);
  }
  if(t < 0)
  {
   StopPlayback();
   return; 
  }

//we play movies back from the disk

if(fread(PDCdata, 1, PDClen, fp) != PDClen)
//smem_seek(&temporarymoviebuffer, 1, SEEK_CUR);
//if(smem_read(&temporarymoviebuffer, PDCdata, PDClen) != PDClen)

  {
   StopPlayback();
   return;
  }
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

   smem_putc(&temporarymoviebuffer, 0);
   smem_write(&temporarymoviebuffer, PDCdata, PDClen);

 //  fputc(0, fp);
 //  fwrite(PDCdata, 1, PDClen, fp);
  }
 }

//uncomment this line to get proper state saving during playback
//but it causes a segfault when you try to record

//tempmloc = ftell(fp); 

}




//adds non-controller data commmands like reset and poweron to a movie
void MDFNMOV_AddCommand(int cmd)
{
 if(current <= 0) return;	/* Return if not recording a movie */

 if(MDFN_StateEvilIsRunning())
  smem_putc(&RewindBuffer, 0);
 else
  //fputc(cmd, slots[current - 1]);
  smem_putc(&temporarymoviebuffer, cmd);
}


//handles loading a state during recording - current behavior is saving the state to the movie file so that the state loads during playback
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

//used during rewinding
void MDFNMOV_ForceRecord(StateMem *sm)
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
   uint8 previewbuffer[3 * width * height];
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
