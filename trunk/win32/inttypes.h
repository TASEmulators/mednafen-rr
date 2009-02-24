#ifndef _INTTYPES_H
#define _INTTYPES_H

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef __int64 int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;
typedef int64_t int64;
typedef uint64_t uint64;

#ifdef __cplusplus
#define INLINE inline
#else
#define INLINE _inline
#define inline _inline
#endif

#endif
