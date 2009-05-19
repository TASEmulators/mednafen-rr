#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <quicktime.h>
#include <lqt.h>
#include <colormodels.h>

#include "mmmreader.h"
#include "video.h"
#include "resize.h"

void log_callback(lqt_log_level_t level, const char *domain, const char *message, void *data)
{
 printf("Callback %s: %s\n", domain, message);
}

int main(int argc, char *argv[])
{
 MMM_Stream_Info stream_info;
 MMM_Reader *reader = new MMM_Reader();
 quicktime_t *qt;
 lqt_codec_info_t **codecs, **codecs_iter;
 lqt_codec_info_t *video_codec = NULL, *audio_codec = NULL;
 uint32 eff_width, eff_height;

 if(!(reader->Open(argv[1])))
 {
  puts("MMM open error");
  return(-1);
 }

 reader->GetStreamInfo(&stream_info);

 if(!(qt = lqt_open_write_with_log("test.mov", LQT_FILE_QT, log_callback, NULL)))
 {
  puts("QT open error");
  return(-1);
 }

 codecs = lqt_query_registry(TRUE, TRUE, TRUE, FALSE); 

 for(codecs_iter = codecs; *codecs_iter; codecs_iter++)
 {
  lqt_codec_info_t *cod = *codecs_iter;

  //if(cod->
   if(!audio_codec && !strcasecmp(cod->name, "twos")) //vorbis"))
   {
    audio_codec = cod;
   }
   if(!video_codec && !strcasecmp(cod->name, "png"))
   {
    video_codec = cod;
   }
  printf("%s\n", (*codecs_iter)->name);
 }
 if(!audio_codec)
  puts("Audio codec not found");
 if(!video_codec)
  puts("Video codec not found");
 if(!video_codec || !audio_codec)
  return(-1);

 for(int i = 0; i < audio_codec->num_encoding_parameters; i++)
 {
  printf("%s %s\n", audio_codec->encoding_parameters[i].name, audio_codec->encoding_parameters[i].real_name);
 }

 for(int i = 0; i < video_codec->num_encoding_parameters; i++)
 {
  lqt_parameter_info_t *pi = video_codec->encoding_parameters;

  printf("Parameter: %d, %s %s\n\n%s\n\n", pi[i].type, pi[i].name, pi[i].real_name, pi[i].help_string);
  for(int z = 0; z < pi[i].num_stringlist_options; z++)
  {
   printf("%d: %s: %d\n", z, pi[i].stringlist_labels[z], pi[i].stringlist_options[z]);
  }
 }
#if 0
 lqt_parameter_value_t moo;
 moo.val_int = 2;
 lqt_set_default_parameter(LQT_CODEC_VIDEO, 1, "x264", "x264_i_rc_method", &moo);
#endif

 eff_width = 256; //stream_info.nominal_width * 2;
 eff_height = stream_info.nominal_height; // * 2;

 printf("%d %d %d %d %f\n", stream_info.nominal_width, stream_info.nominal_height, (int)((stream_info.rate >> 32) / stream_info.average_fps), stream_info.rate >> 32, stream_info.average_fps);

 if(lqt_add_video_track(qt, eff_width, eff_height, (int)((stream_info.rate >> 32) / stream_info.average_fps), stream_info.rate >> 32, video_codec))
 {
  puts("Error adding video track");
  return(-1);
 }

 lqt_set_cmodel(qt, 0, BC_RGBA8888);
 //lqt_set_pixel_aspect(qt, 0, 1, 1);
 if(lqt_add_audio_track(qt, stream_info.soundchan, stream_info.rate >> 32, 16, audio_codec))
 {
  puts("Error adding audio track");
  return(-1);
 }
 MDFN_Surface *ResizeSurface;
 MDFN_Rect ResizeRect;
 uint64 audio_frame_count = 0;
 uint32 i = 0;
 MMM_Stream_Frame *frame;

 ResizeSurface = MDFN_CreateSurface(NULL, eff_width, eff_height, eff_width, 0, 8, 16, 24);
 ResizeRect.x = ResizeRect.y = 0;
 ResizeRect.w = eff_width;
 ResizeRect.h = eff_height;

 while((frame = reader->ReadFrame()))
 {
  MDFN_Surface SourceSurface;
  MDFN_Rect SourceRect;
  MDFN_Rect SourceLineWidths[frame->height];

  int16 crazy_buf[stream_info.soundchan][frame->audio_frame_count];
  int16 *meow[2] = { crazy_buf[0], crazy_buf[1] };
  uint8 *rows[eff_height];

  for(int i = 0; i < frame->audio_frame_count; i++)
  {
   for(int ch = 0; ch < stream_info.soundchan; ch++)
    crazy_buf[ch][i] = frame->audio[ch + i * stream_info.soundchan];
  }

  SourceSurface.pixels = frame->video;
  SourceSurface.w = frame->width;
  SourceSurface.h = frame->height;
  SourceSurface.pitch32 = frame->pitch32;
  SourceSurface.Rshift = 0;
  SourceSurface.Gshift = 8;
  SourceSurface.Bshift = 16;
  SourceSurface.Ashift = 24;

  SourceRect.x = SourceRect.y = 0;
  SourceRect.w = frame->width;
  SourceRect.h = frame->height;

  //printf("%d\n", frame->pitch32);

  if(frame->line_widths)
   for(int y = 0; y < frame->height; y++)
   {
    SourceLineWidths[y].x = 0;
    SourceLineWidths[y].w = frame->line_widths[y];
   }

  //MDFN_ResizeSurface(&SourceSurface, &SourceRect, frame->line_widths ? SourceLineWidths : NULL, ResizeSurface, &ResizeRect);

  for(int y = 0; y < eff_height; y++)
  {
   //*(frame->video + y * frame->pitch32) = rand();
   rows[y] = (uint8 *)(frame->video + y * frame->pitch32);
   //rows[y] = (uint8 *)(ResizeSurface->pixels + y * ResizeSurface->pitch32);
  }
  //memset(crazy_buf, 0, sizeof(crazy_buf));

  lqt_encode_video(qt, rows, 0, audio_frame_count);
  lqt_encode_audio_track(qt, meow, NULL, frame->audio_frame_count, 0);

  audio_frame_count += frame->audio_frame_count;

  reader->FreeFrame(frame);

  if(!(i & 0xF))
   printf("%d\n", i);

  i++;

  //if(i >= 300) break;
 }
  //MDFN_Surface(ResizeSurface);

 // lqt_close??
 quicktime_close(qt);

 return(0);
}
