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

#include "mednafen.h"
#include "endian.h"
#include "video.h"
#include "mmmreader.h"
#include "mmcommon.h"

#include <zlib.h>
#include "compress/minilzo.h"

MMM_Reader::MMM_Reader()
{
 SetRGBAFormat(0, 8, 16, 24);
 SetAlphaComponent(0xFF);
}

MMM_Reader::~MMM_Reader()
{
 Close();
}

void MMM_Reader::Close(void)
{
 if(static_vb)
 {
  free(static_vb);
  static_vb = NULL;
 }

 if(static_ab)
 {
  free(static_ab);
  static_ab = NULL;
 }

 if(fp)
 {
  fclose(fp);
  fp = NULL;
 }
 FrameIndex.clear();
 FrameTime.clear();
}

bool MMM_Reader::Open(const char *path, bool StaticVideoBuffer, bool StaticAudioBuffer)
{
 MM_Main_Header main_header;
 uint64 fi_offset;

 assert(sizeof(MM_Main_Header) == 0x100);

 static_vb = NULL;
 static_ab = NULL;
 fp = NULL;
 FrameIndex.clear();
 FrameTime.clear();

 fp = fopen(path, "rb");

 if(!fp)
  return(FALSE);

 if(fread(&main_header, 1, sizeof(MM_Main_Header), fp) != sizeof(MM_Main_Header))
 {
  Close();
  return(0);
 }

 if(memcmp(main_header.magic, "Mednafen M Movie", 16))
 {
  Close();
  return(0);
 }

 video_compressor = MDFN_delsb(&main_header.video_compressor);
 audio_compressor = MDFN_delsb(&main_header.audio_compressor);
 min_width = MDFN_delsb(&main_header.min_width);
 max_width = MDFN_delsb(&main_header.max_width);
 min_height = MDFN_delsb(&main_header.min_height);
 max_height = MDFN_delsb(&main_header.max_height);
 total_frames = MDFN_delsb(&main_header.video_frame_counter);
 total_audio_frames = MDFN_delsb(&main_header.audio_frame_counter);
 nominal_width = MDFN_delsb(&main_header.nominal_width);
 nominal_height = MDFN_delsb(&main_header.nominal_height);
 rate = MDFN_delsb(&main_header.sound_rate);
 soundchan = MDFN_delsb(&main_header.sound_channels);
 max_aframes_per_vframe = MDFN_delsb(&main_header.max_aframes_per_vframe);

 fi_offset = MDFN_delsb(&main_header.frame_index_offset);
 if(fseek(fp, fi_offset, SEEK_SET) == -1)
 {
  return(0);
 }
 uint8 IndexHeader[8];

 if(fread(IndexHeader, 1, 8, fp) != 8)
 {
  Close();
  return(0);
 }

 if(memcmp(IndexHeader, "INDEX\0\0", 8))
 {
  Close();
  return(0);
 }

 // FIXME:  Speed
 uint64 running_total = 0;
 uint64 running_time_total = 0;
 for(uint64 i = 0; i < total_frames; i++)
 {
  uint32 rawbuf[2];

  if(fread(&rawbuf[0], 1, 2 * sizeof(uint32), fp) != 2 * sizeof(uint32))
   return(0);
  running_total += MDFN_delsb(&rawbuf[0]);
  FrameIndex.push_back(running_total);

  running_time_total += MDFN_delsb(&rawbuf[1]);
  FrameTime.push_back(running_time_total);
 }

 if(fseek(fp, sizeof(main_header), SEEK_SET))
 {
  Close();
  return(FALSE);
 }

 if((rate >> 32) < 8192 || (rate >> 32) > 96000)
 {
  printf(_("Invalid playback rate!"));
  Close();
  return(0);
 }

 if(soundchan < 1 || soundchan > 2)
 {
  printf(_("Invalid number of sound channels specified!"));
  Close();
  return(0);
 }

 if(min_width > max_width)
 {
  printf(_("Minimum width is > maximum width!"));
  Close();
  return(0);
 }

 if(min_height > max_height)
 {
  printf(_("Minimum height is > maximum height!"));
  Close();
  return(0);
 }

 if(max_width > 2048)
 {
  printf(_("Maximum width is too large!"));
  Close();
  return(0);
 }

 if(max_height > 2048)
 {
  printf(_("Maximum height is too large!"));
  Close();
  return(0);
 }

 if(audio_compressor != 0x0)
 {
  printf(_("Unknown audio compressor!"));
  Close();
  return(0);
 }

 if(video_compressor != 0x1)
 {
  printf(_("Unknown video compressor!"));
  Close();
  return(0);
 }

 if(!total_frames)
 {
  printf(_("Cowardly refusing to play an empty video!"));
  Close();
  return(0);
 }

 if(max_aframes_per_vframe > (1 << 24))
 {
  printf(_("Max audio frames per video frame is too high: %u\n"), max_aframes_per_vframe);
  Close();
  return(0);
 }

 if(StaticVideoBuffer)
 {
  if(!(static_vb = (uint32 *)malloc(max_width * max_height * sizeof(uint32))))
  {
   Close();
   return(0);
  }
 }

 if(StaticAudioBuffer)
 {
  if(!(static_ab = (int16 *)malloc(max_aframes_per_vframe * sizeof(int16) * soundchan)))
  {
   Close();
   return(0);
  }
 }
 return(TRUE);
}

void MMM_Reader::GetStreamInfo(MMM_Stream_Info *info)
{
 info->version = version;
 info->video_compressor = video_compressor;
 info->audio_compressor = audio_compressor;



 info->min_width = min_width;
 info->max_width = max_width;
 info->min_height = min_height;
 info->max_height = max_height;

 info->total_frames = total_frames;
 info->total_audio_frames = total_audio_frames;

 info->nominal_width = nominal_width;
 info->nominal_height = nominal_height;

 info->rate = rate;
 info->soundchan = soundchan;

 info->average_fps = total_frames / ((double)total_audio_frames / rate) / (double)((uint64)1 << 32);
}

bool MMM_Reader::SeekFrame(uint64 FrameNumber)
{
 if(FrameNumber >= FrameIndex.size())
  FrameNumber = FrameIndex.size() - 1;

 if(fseek(fp, FrameIndex[FrameNumber], SEEK_SET))
  return(FALSE);

 frame_counter = FrameNumber;

 return(TRUE);
}


MMM_Stream_Frame *MMM_Reader::ReadFrame(void)
{
 uint8 frame_header[8 + 2 + 2 + 4 + 4 + 4];
 uint8 *fhi = frame_header;

 if(fread(frame_header, 1, sizeof(frame_header), fp) == sizeof(frame_header))
 {
  if(!memcmp(frame_header, "FRAME\0\0", 8))
  {
   MMM_Stream_Frame *frame = (MMM_Stream_Frame *)calloc(1, sizeof(MMM_Stream_Frame));
   uint8 *compressed_buffer = NULL;
   uint8 *uncompressed_buffer = NULL;
   uint16 width;
   uint16 height;
   uint32 c_len;
   uint32 uc_len;
   uint32 audio_frames;
   uint8 *video_read_ptr;
   lzo_uint dst_len;

   fhi += 8;

   width = MDFN_de16lsb(fhi);
   fhi += 2;

   height = MDFN_de16lsb(fhi);
   fhi += 2;

   c_len = MDFN_de32lsb(fhi);
   fhi += 4;

   uc_len = MDFN_de32lsb(fhi);
   fhi += 4;

   audio_frames = MDFN_de32lsb(fhi);
   fhi += 4;

   // FIXME
   if(audio_frames > max_aframes_per_vframe)
   {
    puts("Audio frame count for this video frame exceeds max_aframes_per_vframe statistic!!");
    return(NULL);
   }

   frame->width = width;
   frame->height = height;
   frame->pitch32 = max_width;

   if(static_vb)
    frame->video = static_vb;
   else
    frame->video = (uint32 *)malloc(frame->pitch32 * height * sizeof(uint32));

   if(!width)
   {
    frame->line_widths = (uint16 *)calloc(sizeof(uint16), height);
    fread(frame->line_widths, sizeof(uint16), height, fp);
   }

   compressed_buffer = (uint8 *)realloc(compressed_buffer, c_len);
   uncompressed_buffer = (uint8 *)realloc(uncompressed_buffer, uc_len);

   fread(compressed_buffer, 1, c_len, fp);
   dst_len = uc_len;
   lzo1x_decompress(compressed_buffer, c_len, uncompressed_buffer, &dst_len, NULL);

   video_read_ptr = uncompressed_buffer;
   for(int y = 0; y < height; y++)
   {
    uint32 *line_ptr = frame->video + frame->pitch32 * y;
    int line_width = width ? width : frame->line_widths[y];

    for(int x = 0; x < line_width; x++)
    {
     uint8 r, g, b;

     r = *video_read_ptr++;
     g = *video_read_ptr++;
     b = *video_read_ptr++;

     line_ptr[x] = (r << Rshift) | (g << Gshift) | (b << Bshift) | (alpha_component << Ashift); //MK_COLOR(r, g, b);
    }
   }
   free(compressed_buffer);
   free(uncompressed_buffer);

   if(static_ab)
    frame->audio = static_ab;
   else
    frame->audio = (int16 *)malloc(audio_frames * sizeof(int16) * soundchan);
   fread(frame->audio, sizeof(int16) * soundchan, audio_frames, fp);
   frame->audio_frame_count = audio_frames;

   frame_counter++;

   return(frame);
  }
 }

 return(NULL);
}

void MMM_Reader::FreeFrame(MMM_Stream_Frame *frame)
{
 //assert(frame); 
 if(frame)
 {
  if(frame->video && frame->video != static_vb)
  {
   free(frame->video);
   frame->video = NULL;
  }
  if(frame->audio && frame->audio != static_ab)
  {
   free(frame->audio);
   frame->audio = NULL;
  }
  if(frame->line_widths)
  {
   free(frame->line_widths);
   frame->line_widths = NULL;
  }
  free(frame);
 }
}

uint64 MMM_Reader::GetCurFrameNum(void)
{
 return(frame_counter);
}

uint64 MMM_Reader::GetCurFrameTime(void)
{
 uint64 tmp = frame_counter;

 if(tmp >= FrameTime.size())
  tmp = FrameTime.size() - 1;

 return((uint64)1000 * FrameTime[tmp] / (rate >> 32));
}

const char * MMM_Reader::GetVideoCompressorName(void)
{
 switch(video_compressor)
 {
  case 0x00: return("Raw");
  case 0x01: return("MiniLZO");
 }
 return("Invalid");
}

const char * MMM_Reader::GetAudioCompressorName(void)
{
 switch(audio_compressor)
 {
  case 0x00: return("Raw");
 }
 return("Invalid");
}


void MMM_Reader::SetRGBAFormat(unsigned int Rs, unsigned int Gs, unsigned int Bs, unsigned int As)
{
 Rshift = Rs;
 Gshift = Gs;
 Bshift = Bs;
 Ashift = As;
}

void MMM_Reader::SetAlphaComponent(uint8 c)
{
 alpha_component = c;
}
