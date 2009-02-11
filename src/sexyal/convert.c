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

#include <string.h>
#include "convert.h"

static inline uint16_t FLIP16(uint16_t b)
{
 return((b<<8)|((b>>8)&0xFF));
}

static inline uint32_t FLIP32(uint32_t b)
{
 return( (b<<24) | ((b>>8)&0xFF00) | ((b<<8)&0xFF0000) | ((b>>24)&0xFF) );
}


/* Only supports one input sample format right now:  SEXYAL_FMT_PCMS16 */
void SexiALI_Convert(SexyAL_format *srcformat, SexyAL_format *destformat, void *vdest, void *vsrc, uint32_t frames)
{
 uint32_t x;
 int16_t *src = vsrc;
 int downit, upit,fmul;

 if(destformat->sampformat == srcformat->sampformat)
  if(destformat->channels == srcformat->channels)
   if(destformat->byteorder == srcformat->byteorder)
   {
    memcpy(vdest, vsrc, frames * (destformat->sampformat >> 4) * destformat->channels);
    return;
   }

 downit = upit = 0;
 
 fmul = 1;

 if(destformat->channels == 1 && srcformat->channels == 2)
  downit = 1;
 else if(destformat->channels == 2 && srcformat->channels == 1)
  upit = 1;
 else
  fmul = srcformat->channels;

 if(destformat->sampformat == SEXYAL_FMT_PCMU8)
 {
  uint8_t *dest = vdest;
  for(x=0;x<frames*fmul;x++)
  {
   int32_t tmp;

   tmp = *src;
   tmp += 32768;
   src++;

   if(downit)
   {
    tmp += *src;
    tmp += 32768;
    tmp >>= 1;
    src++;
   }
   tmp >>= 8;
   *dest++ = tmp;
   if(upit) 
    *dest++ = tmp;
  }
 }

 if(destformat->sampformat == SEXYAL_FMT_PCMS8)
 {
  int8_t *dest = vdest;
  for(x=0;x<frames*fmul;x++)
  {
   int32_t tmp;

   tmp = *src;
   src++;

   if(downit)
   {
    tmp += *src;
    tmp >>= 1;
    src++;
   }
   tmp >>= 8;
   *dest++ = tmp;
   if(upit) 
    *dest++ = tmp;
  }
 }

 if(destformat->sampformat == SEXYAL_FMT_PCMU16)
 {
  uint16_t *dest = vdest;
  for(x=0;x<frames*fmul;x++)
  {
   int32_t tmp;

   tmp = *src;
   tmp += 32768;
   src++;
   if(downit)
   {
    tmp += *src;
    tmp += 32768;
    tmp >>= 1;
    src++;
   }
   *dest++ = tmp;
   if(upit) 
    *dest++ = tmp;
  }
 }

 if(destformat->sampformat == SEXYAL_FMT_PCMS16)
 {
  int16_t *dest = vdest;

  for(x=0;x<frames*fmul;x++)
  {
   int32_t tmp;

   tmp = *src;
   src++;

   if(downit)
   {
    tmp += *src;
    src++;
    tmp /= 2;
   }
   *dest++ = tmp;
   if(upit)
    *dest++ = tmp;
  }
 }

 if(destformat->sampformat == SEXYAL_FMT_PCMFLOAT)
 {
  float *dest = vdest;
  for(x=0;x<frames*fmul;x++)
  {
   float tmp;

   tmp = (float)*src / 32768;
   src++;
   if(downit)
   {
    tmp += (float)*src / 32768;
    tmp /= 2;
    src++;
   }
   *dest++ = tmp;
   if(upit)
    *dest++ = tmp;
  }
 }


 if(destformat->byteorder != srcformat->byteorder)
 {
  if((destformat->sampformat >> 4) == 2)
  {
   uint16_t *dest = vdest;
   for(x=0;x<frames * destformat->channels;x++)
   {
    *dest = FLIP16(*dest);
    dest++;
   }
  }
  else if((destformat->sampformat >> 4) == 4)
  {
   uint32_t *dest = vdest;
   for(x=0;x<frames;x++)
   {
    *dest = FLIP32(*dest);
    dest++;
   }
  }

 }
}
