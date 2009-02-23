/* Mednafen - Multi-system Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2002 Xodnizel
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

#ifdef WIN32
#define snprintf _snprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define snprintf sprintf_s
#endif

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "sound.h"

#include "../sexyal/sexyal.h"

static SexyAL *Interface;
static SexyAL_device *Output;
static SexyAL_format format;
static SexyAL_buffering buffering;
static SexyAL_enumtype *DriverTypes;
static int CurDriverIndex = 0;

uint32 GetMaxSound(void)
{
 if(!Output) return(0);
 return(buffering.totalsize);
}

uint32 GetWriteSound(void)
{
 if(!Output) return(0);
 return(Output->CanWrite(Output));
}

void WriteSound(int16 *Buffer, int Count)
{
 if(!Output) return;
 //printf("%d\n",Output->CanWrite(Output));
 Output->Write(Output, Buffer, Count);
}

void WriteSoundSilence(int ms)
{
 unsigned int frames = (uint64)format.rate * ms / 1000;
 int16 SBuffer[frames * format.channels];

 memset(SBuffer, 0, sizeof(SBuffer));
 Output->Write(Output, SBuffer, frames);
}

int InitSound(MDFNGI *gi)
{
 memset(&format,0,sizeof(format));
 memset(&buffering,0,sizeof(buffering));

 MDFNI_SetSoundVolume(MDFN_GetSettingUI("soundvol"));

 Interface = (SexyAL *)SexyAL_Init(0);
 DriverTypes = Interface->EnumerateTypes(Interface);

 format.sampformat=SEXYAL_FMT_PCMS16;
 format.channels=gi->soundchan?gi->soundchan:1;
 format.byteorder = 0;
 format.rate=gi->soundrate?gi->soundrate : MDFN_GetSettingUI("soundrate");
 buffering.ms = MDFN_GetSettingUI("soundbufsize");

 std::string zedevice = MDFN_GetSettingS("sounddevice");
 std::string zedriver = MDFN_GetSettingS("sounddriver");

 CurDriverIndex = 0;

 if(strcasecmp(zedriver.c_str(), "default"))
 {
  for(int x = 0; DriverTypes[x].short_name; x++)
  {
   if(!strcasecmp(zedriver.c_str(), DriverTypes[x].short_name))
   {
    CurDriverIndex = x;
    break;
   }
  }
 }

 MDFNI_printf(_("\nInitializing sound...\n"));
 MDFN_indent(1);
 MDFNI_printf(_("Using \"%s\" audio driver with device \"%s\":"),DriverTypes[CurDriverIndex].name, zedevice.c_str());
 MDFN_indent(1);

 if(!(Output=Interface->Open(Interface, zedevice.c_str(), &format, &buffering, DriverTypes[CurDriverIndex].type)))
 {
  MDFND_PrintError(_("Error opening a sound device."));
  Interface->Destroy(Interface);
  Interface=0;
  MDFN_indent(-2);
  return(0);
 }

 if(format.rate<8192 || format.rate > 48000)
 {
  MDFND_PrintError(_("Set rate is out of range [8192-48000]"));
  KillSound();
  MDFN_indent(-2);
  return(0);
 }
 MDFNI_printf(_("\nBits: %u\nRate: %u\nChannels: %u\nByte order: CPU %s\nBuffer size: %u sample frames(%f ms)\n"),(format.sampformat>>4)*8,format.rate,format.channels,format.byteorder?"Reversed":"Native",buffering.latency,(double)buffering.latency*1000/format.rate);

 format.sampformat=SEXYAL_FMT_PCMS16;
 format.channels=gi->soundchan?gi->soundchan:1;
 format.byteorder=0;

 //format.rate=gi->soundrate?gi->soundrate:soundrate;

 Output->SetConvert(Output,&format);

 MDFNI_Sound(format.rate);
 MDFN_indent(-2);

 extern char *soundrecfn;
 if(soundrecfn)
 {
  if(!MDFNI_BeginWaveRecord(format.rate, format.channels, soundrecfn))
  {
   free(soundrecfn);
   soundrecfn=0;
  }
 }
 return(1);
}

void SilenceSound(int n)
{

}

int KillSound(void)
{
 MDFNI_Sound(0);
 if(Output)
  Output->Close(Output);
 if(Interface)
  Interface->Destroy(Interface);
 Interface=0;
 if(!Output) return(0);
 Output=0;
 return(1);
}
