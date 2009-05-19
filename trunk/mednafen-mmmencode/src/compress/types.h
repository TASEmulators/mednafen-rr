#ifndef __MDFN_TYPES
#define __MDFN_TYPES

// Yes, yes, I know:  There's a better place for including config.h than here, but I'm tired, and this should work fine. :b
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <inttypes.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32; 
typedef int64_t int64;

typedef uint8_t uint8;  
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef uint64_t uint64;

#define INLINE inline
#define GINLINE inline

typedef struct
{
 int x, y, w, h;
} MDFN_Rect;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/*! Macros for big- and little-endian support (defaults to little-endian). */

#if !defined(MSB_FIRST) && !defined(LSB_FIRST)
 #error "Define MSB_FIRST or LSB_FIRST!"
#endif

#ifdef MSB_FIRST

 #ifdef LSB_FIRST
  #error Only define one of LSB_FIRST and MSB_FIRST
 #endif

 #ifndef le32toh
  #define le32toh(l)      ((((l)>>24) & 0xff) | (((l)>>8) & 0xff00) \
                         | (((l)<<8) & 0xff0000) | (((l)<<24) & 0xff000000))
 #endif
 #ifndef le16toh
  #define le16toh(l)      ((((l)>>8) & 0xff) | (((l)<<8) & 0xff00))
 #endif
#else
 #ifndef le32toh
  #define le32toh(l)      (l)
 #endif
 #ifndef le16toh
  #define le16toh(l)      (l)
 #endif
#endif

#ifndef htole32
#define htole32 le32toh
#endif

#ifndef htole16
#define htole16 le16toh
#endif


// Some compilers' optimizers and some platforms might fubar the generated code from these macros, so some tests are run in...tests.cpp
#define sign_9_to_s16(_value)  (((int16)((unsigned int)(_value) << 7)) >> 7)
#define sign_10_to_s16(_value)  (((int16)((uint32)(_value) << 6)) >> 6)
#define sign_11_to_s16(_value)  (((int16)((uint32)(_value) << 5)) >> 5)
#define sign_12_to_s16(_value)  (((int16)((uint32)(_value) << 4)) >> 4)
#define sign_13_to_s16(_value)  (((int16)((uint32)(_value) << 3)) >> 3)
#define sign_14_to_s16(_value)  (((int16)((uint32)(_value) << 2)) >> 2)
#define sign_15_to_s16(_value)  (((int16)((uint32)(_value) << 1)) >> 1)

// This obviously won't convert higher-than-32 bit numbers to signed 32-bit ;)
// Also, this shouldn't be used for 8-bit and 16-bit signed numbers, since you can
// convert those faster with typecasts...
#define sign_x_to_s32(_bits, _value) (((int32)((uint32)(_value) << (32 - _bits))) >> (32 - _bits))

#include "error.h"

#endif
