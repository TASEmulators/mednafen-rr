#ifndef __MDFN_MMMREADER_H
#define __MDFN_MMMREADER_H

#include <vector>

enum
{
 VIDEO_COMP_NONE = 0,
 VIDEO_COMP_MINILZO = 1
};

typedef struct
{
 uint32 version;
 uint16 video_compressor;
 uint16 audio_compressor;


 uint32 min_width;
 uint32 max_width;
 uint32 min_height;
 uint32 max_height;

 uint64 total_frames;
 uint64 total_audio_frames;

 uint32 nominal_width;
 uint32 nominal_height;

 uint64 rate;   // 32.32
 uint32 soundchan;

 uint32 max_aframes_per_vframe;

 double average_fps; // 32.32
} MMM_Stream_Info;

typedef struct
{
 uint16 width;
 uint16 *line_widths;	// If width is 0, then use this.
 uint16 height;

 uint32 *video;
 uint32 pitch32;

 uint32 audio_frame_count;
 int16 *audio;
 
} MMM_Stream_Frame;

class MMM_Reader
{
 public:

 MMM_Reader();
 ~MMM_Reader();

 bool Open(const char *path, bool StaticVideoBuffer = FALSE, bool StaticAudioBuffer = FALSE);
 void GetStreamInfo(MMM_Stream_Info *);
 MMM_Stream_Frame *ReadFrame(void);
 void FreeFrame(MMM_Stream_Frame *);

 void SetAlphaComponent(uint8 c);
 void SetRGBAFormat(unsigned int Rs, unsigned int Gs, unsigned int Bs, unsigned int As);

 uint64 GetCurFrameNum(void);
 uint64 GetCurFrameTime(void);		// Returns in milliseconds
 bool SeekFrame(uint64 FrameNumber);

 const char *GetVideoCompressorName(void);
 const char *GetAudioCompressorName(void);

 void Close(void);

 private:

 std::vector <uint64> FrameIndex;
 std::vector <uint64> FrameTime;

 uint32 *static_vb;
 int16 *static_ab;
 FILE *fp;

 uint32 version;
 uint16 video_compressor;
 uint16 audio_compressor;

 uint32 min_width;
 uint32 max_width;
 uint32 min_height;
 uint32 max_height;

 uint64 total_frames;
 uint64 total_audio_frames;

 uint32 nominal_width;
 uint32 nominal_height;

 uint64 rate;	// 32.32

 uint32 max_aframes_per_vframe;

 uint8 soundchan;


 uint64 frame_counter;


 
 unsigned int Rshift, Gshift, Bshift, Ashift;
 uint8 alpha_component;
};

#endif
