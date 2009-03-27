/* SexyAL - Simple audio abstraction library.

Copyright (c) 2005 Mednafen Team

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "../sexyal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include "unixstuff.h"
#else
#include <unistd.h>
#endif

#include <SDL.h>

typedef struct
{
	void *Buffer;
	int32_t BufferSize;
	int32_t BufferRead;
	int32_t BufferWrite;
	int32_t BufferIn;
	int32_t BufferGranularity;

	int StartPaused;
	int ProgPaused;
} SDLWrap;

static void fillaudio(void *udata, uint8_t *stream, int len)
{
 SDLWrap *sw = udata;
 int tocopy = len;

 if(tocopy > sw->BufferIn) tocopy = sw->BufferIn;

 if(tocopy < len)
 {
  //puts("Start pause");
  SDL_PauseAudio(1);
  sw->StartPaused = 1;
 }

 while(tocopy)
 {
  int maxcopy = tocopy;

  if((maxcopy + sw->BufferRead) > sw->BufferSize)
   maxcopy = sw->BufferSize - sw->BufferRead;

  memcpy(stream, (char *)sw->Buffer + sw->BufferRead, maxcopy);

  sw->BufferRead = (sw->BufferRead + maxcopy) % sw->BufferSize;

  sw->BufferIn -= maxcopy;

  stream += maxcopy;
  tocopy -= maxcopy;
  //printf("TOC: %d %d %d %d\n",tocopy, maxcopy,sw->BufferIn,sw->BufferSize);
 }
}

static uint32_t RawCanWrite(SexyAL_device *device)
{
 SDLWrap *sw = device->private_data;
 uint32_t ret;

 SDL_LockAudio();
 ret = sw->BufferSize - sw->BufferIn;
 SDL_UnlockAudio();
 return(ret);
}

static uint32_t RawWrite(SexyAL_device *device, void *data, uint32_t len)
{
 SDLWrap *sw = device->private_data;
 uint32_t olen = len;

 while(len)
 {
  int tocopy;
  SDL_LockAudio();
  tocopy = len;

  if((tocopy + sw->BufferIn) > sw->BufferSize) tocopy = sw->BufferSize - sw->BufferIn;

  while(tocopy)
  {
   int maxcopy = tocopy;

   if((maxcopy + sw->BufferWrite) > sw->BufferSize)
    maxcopy = sw->BufferSize - sw->BufferWrite;

   memcpy((char*)sw->Buffer + sw->BufferWrite, data, maxcopy);
   sw->BufferWrite = (sw->BufferWrite + maxcopy) % sw->BufferSize;

   sw->BufferIn += maxcopy;

   #ifdef _MSC_VER
   (uint8_t*)data += maxcopy;
   #else
   data += maxcopy;
   #endif
   
   tocopy -= maxcopy;
   len -= maxcopy;
  }
  SDL_UnlockAudio();
  if(len)
  {
   if(sw->StartPaused)
   {
    sw->StartPaused = 0;
    SDL_PauseAudio(sw->ProgPaused);
   }
   SDL_Delay(1);
  }
  //else
  // puts("nodelay");
 }
 return(olen);
}

static int Pause(SexyAL_device *device, int state)
{
 SDLWrap *sw = device->private_data;

 sw->ProgPaused = state?1:0;
 SDL_PauseAudio(sw->ProgPaused | sw->StartPaused);

 return(sw->ProgPaused);
}

static int Clear(SexyAL_device *device)
{
 SDLWrap *sw = device->private_data;
 SDL_LockAudio();

 SDL_PauseAudio(1);
 sw->StartPaused = 1;
 sw->BufferRead = sw->BufferWrite = sw->BufferIn = 0;

 SDL_UnlockAudio();
 return(1);
}

static int RawClose(SexyAL_device *device)
{
 if(device)
 {
  if(device->private_data)
  {
   SDLWrap *sw = device->private_data;
   if(sw->Buffer)
    free(sw->Buffer);
   SDL_CloseAudio();
   free(device->private_data);
   SDL_Quit();
  }
  free(device);
  return(1);
 }
 return(0);
}

SexyAL_device *SexyALI_SDL_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering)
{
 SexyAL_device *device;
 SDLWrap *sw;
 SDL_AudioSpec desired, obtained;
 int iflags;

 iflags = SDL_INIT_AUDIO | SDL_INIT_TIMER;

 #ifdef SDL_INIT_EVENTTHREAD
 iflags |= SDL_INIT_EVENTTHREAD;
 #endif
 //iflags = SDL_INIT_EVERYTHING;
 SDL_Init(iflags);

 sw = (SDLWrap *)malloc(sizeof(SDLWrap));
 memset(sw, 0, sizeof(SDLWrap));

 device = (SexyAL_device *)malloc(sizeof(SexyAL_device));
 memset(device, 0, sizeof(SexyAL_device));
 device->private_data = sw;


 memset(&desired, 0, sizeof(SDL_AudioSpec));
 memset(&obtained, 0, sizeof(SDL_AudioSpec));

 desired.freq = format->rate;
 desired.format = AUDIO_S16;
 desired.channels = format->channels;
 desired.callback = fillaudio;
 desired.userdata = sw;
 desired.samples = 256; //512;	// FIXME

 if(SDL_OpenAudio(&desired, &obtained) < 0)
 {
  puts(SDL_GetError());
  RawClose(device);
  return(0);
 }

 format->channels = obtained.channels;
 format->rate = obtained.freq;

 if(obtained.format == AUDIO_U8)
  format->sampformat = SEXYAL_FMT_PCMU8;
 else if(obtained.format == AUDIO_S8)
  format->sampformat = SEXYAL_FMT_PCMS8;
 else if(obtained.format == AUDIO_S16LSB || obtained.format == AUDIO_S16MSB)
 {
  format->sampformat = SEXYAL_FMT_PCMS16;
  if(AUDIO_S16 != AUDIO_S16SYS)
   format->byteorder = 1;
 }
 else if(obtained.format == AUDIO_U16LSB || obtained.format == AUDIO_U16MSB)
 {
  format->sampformat = SEXYAL_FMT_PCMU16;
  if(AUDIO_S16 != AUDIO_U16SYS)
   format->byteorder = 1;
 }

 if(!buffering->ms) 
  buffering->ms = 100;

 sw->BufferSize = (format->rate * buffering->ms / 1000) - obtained.samples * 2;
 if(sw->BufferSize < obtained.samples) sw->BufferSize = obtained.samples;

 sw->BufferIn = sw->BufferRead = sw->BufferWrite = 0;

 buffering->totalsize = sw->BufferSize;

 buffering->latency = (obtained.samples * 2 + sw->BufferSize);
 //printf("%d\n", buffering->latency);
 sw->BufferSize *= format->channels * (format->sampformat >> 4);
 sw->Buffer = malloc(sw->BufferSize);

 memcpy(&device->format,format,sizeof(SexyAL_format));
 memcpy(&device->buffering,buffering,sizeof(SexyAL_buffering));

 device->RawCanWrite = RawCanWrite;
 device->RawWrite = RawWrite;
 device->RawClose = RawClose;
 device->Clear = Clear;
 device->Pause = Pause;

 sw->StartPaused = 1;
 //SDL_PauseAudio(0);
 return(device);
}

