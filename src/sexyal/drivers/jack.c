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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <jack/jack.h>
#include <jack/ringbuffer.h>

#include "../sexyal.h"

typedef struct
{
	jack_port_t *output_port[2];
	jack_client_t *client;
	jack_ringbuffer_t *tmpbuf[2];
	unsigned int BufferSize;

	const char **ports;
	int closed;
} JACKWrap;

static int process(jack_nframes_t nframes, void *arg)
{
 JACKWrap *jw = arg;
 int tch = 1;
 int ch;
 size_t canread = jack_ringbuffer_read_space(jw->tmpbuf[0]) / sizeof(jack_default_audio_sample_t);

 if(jw->tmpbuf[1])
  tch = 2;

 if(tch == 2)
 {
  size_t canread2 = jack_ringbuffer_read_space(jw->tmpbuf[1]) / sizeof(jack_default_audio_sample_t);

  if(canread2 < canread) canread = canread2;
 }

 if(canread > nframes)
  canread = nframes;

 for(ch = 0; ch < tch; ch++)
 {
  jack_default_audio_sample_t *out = (jack_default_audio_sample_t *) jack_port_get_buffer(jw->output_port[ch], nframes);

  jack_ringbuffer_read(jw->tmpbuf[ch], (char *)out,canread * sizeof(jack_default_audio_sample_t));
  nframes -= canread;

  if(nframes)    /* Buffer underflow.  Hmm. */
  {

  }
 }
 return(1);
}

static uint32_t RawCanWrite(SexyAL_device *device)
{
 JACKWrap *jw = device->private_data;

 if(jw->closed) return(~0);
 return(jack_ringbuffer_write_space(jw->tmpbuf[0]));
}

static uint32_t RawWrite(SexyAL_device *device, void *data, uint32_t len)
{
 JACKWrap *jw = device->private_data;

 if(jw->closed) return(0);

 if(device->format.channels == 2)
 {
  float left_data[len / 2];
  float right_data[len / 2];
  float *in_data = (float *)data;
  int x;

  for(x = 0; x < len / 2; x++)
  {
   left_data[x] = *in_data++;
   right_data[x] = *in_data++;
  }

  while(jack_ringbuffer_write_space(jw->tmpbuf[0]) < len / 2 && jack_ringbuffer_write_space(jw->tmpbuf[1]) < len / 2)
  {
   if(jw->closed) return(0);
   usleep(1000);
  }

  jack_ringbuffer_write(jw->tmpbuf[0], left_data, len / 2);
  jack_ringbuffer_write(jw->tmpbuf[1], right_data, len / 2);

  return(len);
 }
 else
 {
  while(jack_ringbuffer_write_space(jw->tmpbuf[0]) < len) 
  {
   if(jw->closed) return(0);
   usleep(1000);
  }
  return(jack_ringbuffer_write(jw->tmpbuf[0], data, len));
 }
}


static int Clear(SexyAL_device *device)
{
 JACKWrap *jw = device->private_data;
 return(1);
}

static int RawClose(SexyAL_device *device)
{
 if(device)
 {
  if(device->private_data)
  {
   JACKWrap *jw = device->private_data;
   if(jw->tmpbuf[0])
   {
    jack_ringbuffer_free(jw->tmpbuf[0]);
   }
   if(jw->tmpbuf[1])
   {
    jack_ringbuffer_free(jw->tmpbuf[1]);
   }

   if(jw->client && !jw->closed)
   {
    //jack_deactivate(jw->client);
    //jack_disconnect(jw->client, jack_port_name(jw->output_port[0]), jw->ports[0]);
    jack_client_close(jw->client);
   }
   free(device->private_data);
  }
  free(device);
  return(1);
 }
 return(0);
}

static void DeadSound(void *arg)
{
 JACKWrap *jw = arg;
 jw->closed = 1;
 //puts("AGH!  Sound server hates us!  Let's go on a rampage.");
}

static int Pause(SexyAL_device *device, int state)
{
 return(0);
}

SexyAL_device *SexyALI_JACK_Open(char *id, SexyAL_format *format, SexyAL_buffering *buffering)
{
 SexyAL_device *device;
 JACKWrap *jw;

 jw = malloc(sizeof(JACKWrap));
 memset(jw, 0, sizeof(JACKWrap));

 device = malloc(sizeof(SexyAL_device));
 memset(device, 0, sizeof(SexyAL_device));
 device->private_data = jw;

 if(!(jw->client = jack_client_new("Mednafen")))
 {
  RawClose(device);
  return(0);
 }

 jack_set_process_callback(jw->client, process, jw);
 jack_on_shutdown(jw->client, DeadSound, jw);

 format->rate = jack_get_sample_rate(jw->client);
 format->sampformat = SEXYAL_FMT_PCMFLOAT;

 if(!(jw->output_port[0] = jack_port_register(jw->client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0)))
 {
  RawClose(device);
  return(0);
 }

 if(format->channels == 2)
 {
  if(!(jw->output_port[1] = jack_port_register(jw->client, "output-right", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0)))
  {
   RawClose(device);
   return(0);
  }
 }


 if(!buffering->ms) buffering->ms = 40;
 jw->BufferSize = format->rate * buffering->ms / 1000;
 buffering->totalsize = jw->BufferSize;

 if(!(jw->tmpbuf[0] = jack_ringbuffer_create(jw->BufferSize * sizeof(jack_default_audio_sample_t))))
 {
  RawClose(device);
  return(0);
 }

 if(format->channels == 2)
  if(!(jw->tmpbuf[1] = jack_ringbuffer_create(jw->BufferSize * sizeof(jack_default_audio_sample_t))))
  {
   RawClose(device);
   return(0);
  }

 if(jack_activate(jw->client))
 {
  RawClose(device);
  return(0);
 }

 if(!(jw->ports = jack_get_ports(jw->client, NULL, NULL, JackPortIsPhysical | JackPortIsInput)))
 {
  RawClose(device);
  return(0);
 }
 jack_connect(jw->client, jack_port_name(jw->output_port[0]), jw->ports[0]);

 if(format->channels == 2)
  jack_connect(jw->client, jack_port_name(jw->output_port[1]), jw->ports[1]);
 else
  jack_connect(jw->client, jack_port_name(jw->output_port[0]), jw->ports[1]);

 free(jw->ports);

 buffering->latency = jw->BufferSize + jack_port_get_latency(jw->output_port[0]);

 memcpy(&device->format,format,sizeof(SexyAL_format));
 memcpy(&device->buffering,buffering,sizeof(SexyAL_buffering));

 device->RawCanWrite = RawCanWrite;
 device->RawWrite = RawWrite;
 device->RawClose = RawClose;
 device->Clear = Clear;
 device->Pause = Pause;

 return(device);
}

