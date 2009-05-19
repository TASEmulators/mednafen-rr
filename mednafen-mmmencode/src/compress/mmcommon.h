#ifndef __MDFN_MMCOMMON_H
#define __MDFN_MMCOMMON_H

/* Elements of this structure that are multi-byte variables(uint16, uint32, uint64)
   MUSTN'T BE ACCESSED "DIRECTLY"
   Use the MDFN_enlsb() and MDFN_delsb() functions to access them.
*/
typedef struct
{
 /* 0x00 */ uint8 magic[16];            // "Mednafen M Movie"
 /* 0x10 */ uint32 version;             // 0x00
 /* 0x14 */ uint16 video_compressor;    // 0x01 = MiniLZO
 /* 0x16 */ uint16 audio_compressor;    // 0x00 = Raw

 /* 0x18 */ uint32 nominal_width;       // Nominal width of video output(scale from individual frame's width to this)
 /* 0x1C */ uint32 nominal_height;      // Nominal height of video output(scale from individual frame's height to this)

 /* 0x20 */ uint32 min_width;           // Minimum video width
 /* 0x24 */ uint32 max_width;           // Maximum video width
 /* 0x28 */ uint32 min_height;          // Minimum video height
 /* 0x2C */ uint32 max_height;          // Maximum video height

 /* 0x30 */ uint64 video_frame_counter; // Total number of video/stream frames
 /* 0x38 */ uint64 audio_frame_counter; // Total number of audio frames(audio samples / sound channels)
 /* 0x40 */ uint64 frame_index_offset;  // Offset of the frame index chunk in the file
 /* 0x48 */ uint64 sound_rate;          // Sound rate(32.32 fixed point)
 /* 0x50 */ uint32 sound_channels;      // Number of sound channels(1 or 2)
 /* 0x54 */ uint32 max_aframes_per_vframe;	// Maximum number of audio frames per video/stream frame.
 /* 0x58 */ uint8 padding[0x100 - 0x58];
} __attribute__((__packed__)) MM_Main_Header;

#endif
