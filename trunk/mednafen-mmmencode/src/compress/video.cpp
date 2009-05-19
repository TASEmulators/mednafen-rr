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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "main.h"
#include "video.h"

MDFN_Surface *MDFN_CreateSurface(uint32 *pixels, uint32 width, uint32 height, uint32 pitch32, uint8 rs, uint8 gs, uint8 bs, uint8 as)
{
 MDFN_Surface *ret = NULL;

 assert((rs + gs + bs + as) == 48);
 assert(!((rs | gs | bs | as) & 0x7));
 assert(pitch32 >= width);

 if(!(ret = (MDFN_Surface *)calloc(1, sizeof(MDFN_Surface))))
  return(NULL);
 
 if(pixels)
  ret->pixels = pixels;
 else
 {
  if(!(ret->pixels = (uint32 *)calloc(1, pitch32 * height * sizeof(uint32))))
  {
   free(ret);
   return(NULL);
  }
 }
 ret->w = width;
 ret->h = height;
 ret->pitch32 = pitch32;
 ret->Rshift = rs;
 ret->Gshift = gs;
 ret->Bshift = bs;
 ret->Ashift = as;

 return(ret);
}

void MDFN_SetSurfaceFormat(MDFN_Surface *surface, uint8 rs, uint8 gs, uint8 bs, uint8 as, bool convert)
{
 assert(surface);

 assert((rs + gs + bs + as) == 48);
 assert(!((rs | gs | bs | as) & 0x7));

 if(convert)
 {
  // We should assert that surface->pixels is non-NULL even if we don't need to convert the surface, to catch more insidious bugs.
  assert(surface->pixels);

  if(((surface->Rshift != rs) || (surface->Gshift != gs) || (surface->Bshift != bs) || (surface->Ashift != as)))
  {
   for(uint32 x = 0; x < surface->pitch32 * surface->h; x++)
   {
    uint32 c = surface->pixels[x];
    int r, g, b, a;

    DECOMP_COLOR(surface, c, r, g, b, a);
    surface->pixels[x] = (r << rs) | (g << gs) | (b << bs) | (a << as);
   }
  }
 }

 surface->Rshift = rs;
 surface->Gshift = gs;
 surface->Bshift = bs;
 surface->Ashift = as;
}


void MDFN_FillSurface(MDFN_Surface *surface, uint8 r, uint8 g, uint8 b, uint8 a)
{
 uint32 color = (r << surface->Rshift) | (g << surface->Gshift) | (b << surface->Bshift) | (a << surface->Ashift);

 assert(surface);
 assert(surface->pixels);

 for(uint32 i = 0; i < surface->pitch32 * surface->h; i++)
 {
  surface->pixels[i] = color;
 }
}

void MDFN_KillSurface(MDFN_Surface *surface)
{
 assert(surface);

 if(surface->pixels)
  free(surface->pixels);

 free(surface);
}

