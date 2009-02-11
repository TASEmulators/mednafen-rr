/* SexyAL - Simple audio abstraction library.

Copyright (c) 2005-2007 Mednafen Team

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

#include "sexyal.h"

#include <stdlib.h>
#include <string.h>
#include "convert.h"


/* kludge.  yay. */
SexyAL_enumdevice *SexyALI_OSS_EnumerateDevices(void);
SexyAL_device *SexyALI_OSS_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering);
SexyAL_device *SexyALI_JACK_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering);
SexyAL_device *SexyALI_SDL_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering);
SexyAL_device *SexyALI_DSound_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering);

#ifdef HAVE_ALSA
SexyAL_enumdevice *SexyALI_ALSA_EnumerateDevices(void);
SexyAL_device *SexyALI_ALSA_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering);
#endif

static uint32_t FtoB(const SexyAL_format *format, uint32_t frames)
{
 return(frames*format->channels*(format->sampformat>>4));
}
/*
static uint32_t BtoF(const SexyAL_format *format, uint32_t bytes)
{
 return(bytes / (format->channels * (format->sampformat>>4)));
}
*/
static uint32_t CanWrite(SexyAL_device *device)
{
 uint32_t bytes,frames;

 bytes = device->RawCanWrite(device);

 frames = bytes / device->format.channels / (device->format.sampformat>>4);

 return(frames);
}

static uint32_t Write(SexyAL_device *device, void *data, uint32_t frames)
{
 uint8_t buffer[2048 * 4 * 2]; // Maximum frame size, 4 bytes * 2 channels
 uint32_t frame_count = 0;

 if(device->srcformat.sampformat == device->format.sampformat &&
	device->srcformat.channels == device->format.channels &&
	device->srcformat.rate == device->format.rate &&
	device->srcformat.byteorder == device->format.byteorder)
 {
  return(device->RawWrite(device, data, FtoB(&device->format, frames)));
 }
 else while(frames)
 {
  int32_t tmp;
  uint32_t fctmp;

  tmp=frames;
  if(tmp>2048) 
  { 
   tmp=2048;
   frames-=2048;
  }
  else frames-=tmp;

  SexiALI_Convert(&device->srcformat, &device->format, buffer, data, tmp);

  fctmp = device->RawWrite(device,buffer,FtoB(&device->format,tmp));
  frame_count += fctmp;

  if(fctmp != tmp) /* Uh oh. */
   return(frame_count);
 }
 return(frame_count);
}

static int Close(SexyAL_device *device)
{
 return(device->RawClose(device));
}

int SetConvert(struct __SexyAL_device *device, SexyAL_format *format)
{
 memcpy(&device->srcformat,format,sizeof(SexyAL_format));
 return(1);
}

void Destroy(SexyAL *iface)
{
 free(iface);
}

static SexyAL_driver drivers[] = 
{
        #if HAVE_ALSA
        { SEXYAL_TYPE_ALSA, "ALSA", "alsa", SexyALI_ALSA_Open, SexyALI_ALSA_EnumerateDevices },
        #endif

	#if HAVE_OSSDSP
	{ SEXYAL_TYPE_OSSDSP, "OSS(/dev/dsp*)", "oss", SexyALI_OSS_Open, SexyALI_OSS_EnumerateDevices },
	#endif

        #if HAVE_DIRECTSOUND
        { SEXYAL_TYPE_DIRECTSOUND, "DirectSound", "dsound", SexyALI_DSound_Open, NULL },
        #endif

        #if HAVE_SDL
        { SEXYAL_TYPE_SDL, "SDL", "sdl", SexyALI_SDL_Open, NULL },
        #endif

        #if HAVE_JACK
        { SEXYAL_TYPE_JACK, "JACK", "jack", SexyALI_JACK_Open, NULL },
        #endif

	{ 0, NULL, NULL, NULL, NULL }
};

static SexyAL_driver *FindDriver(int type)
{
 int x = 0;

 while(drivers[x].name)
 {
  if(drivers[x].type == type)
   return(&drivers[x]);

  x++;
 }
 return(0);
}

static SexyAL_device *Open(SexyAL *iface, const char *id, SexyAL_format *format, SexyAL_buffering *buffering, int type)
{
 SexyAL_device *ret;
 SexyAL_driver *driver;

 driver = FindDriver(type);
 if(!driver)
  return(0);

 if(id && id[0] == 0) // Set pointer to NULL on empty string.
  id = NULL;

 if(id)
 {
  if(!strcmp(id, "default"))
   id = NULL;
  else if(!strncmp(id, "sexyal-literal-", strlen("sexyal-literal-")))
   id += strlen("sexyal-literal-");
 }
 if(!(ret = driver->Open(id, format, buffering))) return(0);

 ret->Write=Write;
 ret->Close=Close;
 ret->CanWrite=CanWrite;
 ret->SetConvert=SetConvert;

 return(ret);
}

static SexyAL_enumtype *EnumerateTypes(SexyAL *sal)
{
 SexyAL_enumtype *typies;
 int numdrivers = sizeof(drivers) / sizeof(SexyAL_driver);
 int x;

 typies = (SexyAL_enumtype *)malloc(numdrivers * sizeof(SexyAL_enumtype));
 memset(typies, 0, numdrivers * sizeof(SexyAL_enumtype));

 x = 0;

 do
 {
  typies[x].name = drivers[x].name;
  typies[x].short_name = drivers[x].short_name;
  typies[x].type = drivers[x].type;
 } while(drivers[x++].name);

 return(typies);
}

static SexyAL_enumdevice * EnumerateDevices(SexyAL *iface, int type)
{
 SexyAL_driver *driver;

 driver = FindDriver(type);

 if(!driver)
  return(0);

 if(driver->EnumerateDevices)
  return(driver->EnumerateDevices());

 return(0);
}

void *SexyAL_Init(int version)
{
 SexyAL *iface;

 iface = (SexyAL *)malloc(sizeof(SexyAL));

 iface->Open=Open;
 iface->Destroy=Destroy;

 iface->EnumerateTypes = EnumerateTypes;
 iface->EnumerateDevices = EnumerateDevices;
 return((void *)iface);
}
