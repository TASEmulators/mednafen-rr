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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <poll.h>
#include <sys/time.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include <unistd.h>

#include "alsa.h"

typedef struct
{
	snd_pcm_t *alsa_pcm;

	uint32_t period_size;
	int interleaved;
} ADStruct;

void SexyALI_ALSA_EnumerateDevices(int (*func)(uint8_t *name, char *id, void *udata), void *udata)
{

}

static int Pause(SexyAL_device *device, int state)
{
 if(0)
  snd_pcm_pause(((ADStruct *)device->private_data)->alsa_pcm, state);
 else
 {
  snd_pcm_drop(((ADStruct *)device->private_data)->alsa_pcm);
 }
 return(0);
}

static int Clear(SexyAL_device *device)
{
 snd_pcm_drop(((ADStruct *)device->private_data)->alsa_pcm);

 return(1);
}

static uint32_t RawCanWrite(SexyAL_device *device)
{
 ADStruct *ads = (ADStruct *)device->private_data;
 uint32_t ret;
 snd_pcm_sframes_t delay;


 while(snd_pcm_delay(ads->alsa_pcm, &delay) < 0)
 {
  // If the call to snd_pcm_delay() fails, try to figure out the status of the PCM stream and take the best action.
  switch(snd_pcm_state(ads->alsa_pcm))
  {
   // This shouldn't happen, but in case it does...
   default: //puts("What1?"); 
	    return(device->buffering.totalsize);

   //default: break;
   //case SND_PCM_STATE_RUNNING: break;
   //case SND_PCM_STATE_PREPARED: break;

   case SND_PCM_STATE_PAUSED:
   case SND_PCM_STATE_DRAINING:
   case SND_PCM_STATE_OPEN:
   case SND_PCM_STATE_DISCONNECTED: return(device->buffering.totalsize);

   case SND_PCM_STATE_SETUP:
   case SND_PCM_STATE_SUSPENDED:
   case SND_PCM_STATE_XRUN:
			   //puts("XRun1");
			   snd_pcm_prepare(ads->alsa_pcm);
			   return(device->buffering.totalsize);
  }
 }

 ret = device->buffering.totalsize - delay;

 // If overrun of >= 100 milliseconds, call snd_pcm_drop() to reset ALSA's overrun voodoo.
 // (RawWrite() calls RawCanWrite() for this purpose)
 #if 0
 if(ret >= device->buffering.totalsize + 100 * device->format.rate / 1000)
 {
  ret = device->buffering.totalsize;
  snd_pcm_drop(ads->alsa_pcm);
 }
 #endif

 if(ret < 0) ret = 0;
 return(ret);
}

// TODO:  Provide de-interleaving code in SexyAL outside of individual drivers

static uint32_t RawWrite(SexyAL_device *device, void *data, uint32_t len)
{
 ADStruct *ads = (ADStruct *)device->private_data;
 uint32_t ret = len;

 #if 0
 RawCanWrite(device);
 #endif

 while(len > 0)
 {
  int snore = 0;

  do
  {
   if(ads->interleaved)
    snore = snd_pcm_writei(ads->alsa_pcm, data, len / (device->format.sampformat>>4) / device->format.channels);
   else
   {
    if(device->format.channels == 1)
    {
     void *foodata[1];
     foodata[0] = data;
     snore = snd_pcm_writen(ads->alsa_pcm, foodata, len / (device->format.sampformat>>4) / device->format.channels);
    }
    else
    {
     uint8_t meowdata[device->format.channels][len / device->format.channels] __attribute__ ((aligned(4)));
     void *foodata[device->format.channels];
     int i, ch;
     int max_moo = len / (device->format.sampformat>>4) / device->format.channels;
     int max_ch = device->format.channels;

     for(ch = 0; ch < max_ch; ch++)
      foodata[ch] = meowdata[ch];

     if((device->format.sampformat>>4) == 1)
     {
      for(ch = 0; ch < max_ch; ch++)
       for(i = 0; i < max_moo; i++)
        ((uint8_t *)meowdata[ch])[i] = ((uint8_t *)data)[i * max_ch + ch];
     }
     else if((device->format.sampformat>>4) == 2)
     {
      for(ch = 0; ch < max_ch; ch++)
       for(i = 0; i < max_moo; i++)
        ((uint16_t *)meowdata[ch])[i] = ((uint16_t *)data)[i * max_ch + ch];
     }
     else if((device->format.sampformat>>4) == 4)
     {
      for(ch = 0; ch < max_ch; ch++)
       for(i = 0; i < max_moo; i++)
        ((uint32_t *)meowdata[ch])[i] = ((uint32_t *)data)[i * max_ch + ch];
     }
     snore = snd_pcm_writen(ads->alsa_pcm, foodata, max_moo);
    }
   }

   if(snore <= 0)
   { 
    switch(snd_pcm_state(ads->alsa_pcm))
    {
     // This shouldn't happen, but if it does, and there was an error, exit out of the loopie.
     default: //puts("What2");
	      if(snore < 0)
	       snore = len / (device->format.sampformat>>4) / device->format.channels;
	      break;

     // Don't unplug your sound card, silly human! ;)
     case SND_PCM_STATE_OPEN:
     case SND_PCM_STATE_DISCONNECTED: snore = len / (device->format.sampformat>>4) / device->format.channels; 
				      //usleep(1000);
				      break;

     case SND_PCM_STATE_SETUP:
     case SND_PCM_STATE_SUSPENDED:
     case SND_PCM_STATE_XRUN: //puts("XRun2");
                             snd_pcm_prepare(ads->alsa_pcm);
                             break;
    }
   }

  } while(snore <= 0);

  data = (uint8_t*)data + snore * (device->format.sampformat>>4) * device->format.channels;

  len -= snore * (device->format.sampformat>>4) * device->format.channels;

  if(snd_pcm_state(ads->alsa_pcm) == SND_PCM_STATE_PREPARED)
   snd_pcm_start(ads->alsa_pcm);
 }

 return(ret * (device->format.sampformat>>4) * device->format.channels);
}

static int RawClose(SexyAL_device *device)
{
 if(device)
 {
  if(device->private_data)
  {
   ADStruct *ads = (ADStruct *)device->private_data;
   snd_pcm_close(ads->alsa_pcm);
   free(device->private_data);
  }
  free(device);
  return(1);
 }
 return(0);
}

#define ALSA_TRY(func) { 	\
	int error; 	\
	error = func; 	\
	if(error) 	\
	{ 	\
	 printf("ALSA Error: %s %s\n", #func, snd_strerror(error)); 	\
	 if(hw_params)  snd_pcm_hw_params_free(hw_params);	\
	 if(alsa_pcm) snd_pcm_close(alsa_pcm);	\
	 if(ads) free(ads);	\
	 if(device) free(device);	\
	 return(0); 	\
	} }

SexyAL_device *SexyALI_ALSA_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering)
{
 ADStruct *ads = NULL;
 SexyAL_device *device = NULL;
 snd_pcm_t *alsa_pcm = NULL;
 snd_pcm_hw_params_t *hw_params = NULL;
 snd_pcm_sw_params_t *sw_params = NULL;
 int interleaved = 1;
 int des_ps;

 snd_pcm_format_t sampformat;

 ALSA_TRY(snd_pcm_open(&alsa_pcm, id ? id : "hw:0", SND_PCM_STREAM_PLAYBACK, 0));
 ALSA_TRY(snd_pcm_hw_params_malloc(&hw_params));
 ALSA_TRY(snd_pcm_sw_params_malloc(&sw_params));

 switch(format->sampformat)
 {
  case SEXYAL_FMT_PCMU8: sampformat = SND_PCM_FORMAT_U8; break;
  case SEXYAL_FMT_PCMS8: sampformat = SND_PCM_FORMAT_S8; break;
  case SEXYAL_FMT_PCMU16: sampformat = SND_PCM_FORMAT_U16; break;
  case SEXYAL_FMT_PCMS16: sampformat = SND_PCM_FORMAT_S16; break;

  default: sampformat = 0; break; // FIXME
 }

 ALSA_TRY(snd_pcm_hw_params_any(alsa_pcm, hw_params));
 ALSA_TRY(snd_pcm_hw_params_set_periods_integer(alsa_pcm, hw_params));

 if(snd_pcm_hw_params_set_access(alsa_pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) != 0)
 {
  puts("Interleaved format not supported, trying non-interleaved instead. :(");
  ALSA_TRY(snd_pcm_hw_params_set_access(alsa_pcm, hw_params, SND_PCM_ACCESS_RW_NONINTERLEAVED));
  interleaved = 0;
 }
 ALSA_TRY(snd_pcm_hw_params_set_format(alsa_pcm, hw_params, sampformat));

 #if SND_LIB_VERSION >= 0x10009
 ALSA_TRY(snd_pcm_hw_params_set_rate_resample(alsa_pcm, hw_params, 0));
 #endif

 unsigned int rrate = format->rate;
 ALSA_TRY(snd_pcm_hw_params_set_rate_near(alsa_pcm, hw_params, &rrate, 0));
 format->rate = rrate;

 if(snd_pcm_hw_params_set_channels(alsa_pcm, hw_params, format->channels) != 0)
 {
  if(format->channels == 2)
  {
   puts("Warning:  Couldn't set stereo sound, trying mono instead...");
   format->channels = 1;
  }
  else if(format->channels == 1)
   format->channels = 2;

  ALSA_TRY(snd_pcm_hw_params_set_channels(alsa_pcm, hw_params, format->channels));
 }

 des_ps = 128;

 if(format->rate <= 24000)
 {
  des_ps >>= 1;
  if(format->rate <= 12000)
   des_ps >>= 1;
 }

 if(snd_pcm_hw_params_set_period_size(alsa_pcm, hw_params, des_ps, 0) != 0)
 {
  //puts("Zoom");
  if(snd_pcm_hw_params_set_period_size(alsa_pcm, hw_params, des_ps * 2, 0) != 0)
  {
   puts("ALSA Warning:  Could not set period size to a nice value. :(");
  }
 }
 snd_pcm_uframes_t tmp_uft;
 tmp_uft = buffering->ms * format->rate / 1000;
 ALSA_TRY(snd_pcm_hw_params_set_buffer_size_near(alsa_pcm, hw_params, &tmp_uft));

 ALSA_TRY(snd_pcm_hw_params(alsa_pcm, hw_params));
 snd_pcm_uframes_t buffer_size, period_size;
 unsigned int periods;

 ALSA_TRY(snd_pcm_hw_params_get_period_size(hw_params, &period_size, NULL));
 ALSA_TRY(snd_pcm_hw_params_get_periods(hw_params, &periods, NULL));
 snd_pcm_hw_params_free(hw_params);

 ALSA_TRY(snd_pcm_sw_params_current(alsa_pcm, sw_params));

 #if 0
 ALSA_TRY(snd_pcm_sw_params_set_xrun_mode(alsa_pcm, sw_params, SND_PCM_XRUN_NONE));
 #endif

 ALSA_TRY(snd_pcm_sw_params(alsa_pcm, sw_params));
 snd_pcm_sw_params_free(sw_params);


 buffer_size = period_size * periods;

 buffering->totalsize = buffer_size;
 buffering->ms = buffering->totalsize * 1000 / format->rate;
 buffering->latency = buffering->totalsize;

 device = (SexyAL_device *)calloc(1, sizeof(SexyAL_device));
 ads = (ADStruct *)calloc(1, sizeof(ADStruct));

 ads->alsa_pcm = alsa_pcm;
 ads->period_size = period_size;
 ads->interleaved = interleaved;

 device->private_data = ads;
 device->RawWrite = RawWrite;
 device->RawCanWrite = RawCanWrite;
 device->RawClose = RawClose;
 device->Pause = Pause;
 device->Clear = Clear;

 memcpy(&device->buffering,buffering,sizeof(SexyAL_buffering));
 memcpy(&device->format,format,sizeof(SexyAL_format));

 ALSA_TRY(snd_pcm_prepare(alsa_pcm));

 return(device);
}

