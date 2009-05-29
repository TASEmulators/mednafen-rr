#ifndef __WSWAN_H
#define __WSWAN_H

#include "../types.h"
#include "../mednafen.h"
#include "../state.h"
#include "../general.h"
#include "../memory.h"

#define  mBCD(value) (((value)/10)<<4)|((value)%10)

extern          uint32 rom_size;
//extern          int wsc;	//adelikat: removing from header file and declaring external in gfx.cpp to avoid unresolved externals

#include "interrupt.h"

#endif
