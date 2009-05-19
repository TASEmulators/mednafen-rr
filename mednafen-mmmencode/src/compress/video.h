#ifndef __MDFN_VIDEO_H
#define __MDFN_VIDEO_H

//#include "video/primitives.h"
//#include "video/text.h"

// Only supports 32-bit RGBA
typedef struct
{
 uint32 *pixels;

 uint32 w;
 uint32 h;
 uint32 pitch32; // In pixels, not in bytes.

 uint8 Rshift;	// Bit position of the lowest bit of the red component
 uint8 Gshift;  // [...] green component
 uint8 Bshift;  // [...] blue component
 uint8 Ashift;  // [...] alpha component.
} MDFN_Surface;

static inline void DECOMP_COLOR(const MDFN_Surface *surface, uint32 c, int &r, int &g, int &b)
{
 r = (c >> surface->Rshift) & 0xFF;
 g = (c >> surface->Gshift) & 0xFF;
 b = (c >> surface->Bshift) & 0xFF;
}

static inline void DECOMP_COLOR(const MDFN_Surface *surface, uint32 c, int &r, int &g, int &b, int &a)
{
 r = (c >> surface->Rshift) & 0xFF;
 g = (c >> surface->Gshift) & 0xFF;
 b = (c >> surface->Bshift) & 0xFF;
 a = (c >> surface->Ashift) & 0xFF;
}

MDFN_Surface *MDFN_CreateSurface(uint32 *pixels, uint32 width, uint32 height, uint32 pitch32, uint8 rs, uint8 gs, uint8 bs, uint8 as);
void MDFN_SetSurfaceFormat(MDFN_Surface *surface, uint8 rs, uint8 gs, uint8 bs, uint8 as, bool convert = FALSE);
void MDFN_FillSurface(MDFN_Surface *surface, uint8 r, uint8 g, uint8 b, uint8 a);
void MDFN_KillSurface(MDFN_Surface *surface);

#endif
