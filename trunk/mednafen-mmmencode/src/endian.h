#ifndef __MDFN_ENDIAN_H
#define __MDFN_ENDIAN_H

int write16le(uint16 b, FILE *fp);
int write32le(uint32 b, FILE *fp);
int read32le(uint32 *Bufo, FILE *fp);

void Endian_A16_NE_to_LE(void *src, uint32 nelements);
void Endian_A32_NE_to_LE(void *src, uint32 nelements);
void Endian_A64_NE_to_LE(void *src, uint32 nelements);

void Endian_A16_LE_to_NE(void *src, uint32 nelements);
void Endian_A16_BE_to_NE(void *src, uint32 nelements);
void Endian_A32_LE_to_NE(void *src, uint32 nelements);
void Endian_A64_LE_to_NE(void *src, uint32 nelements);

void Endian_V_LE_to_NE(void *src, uint32 bytesize);
void Endian_V_NE_to_LE(void *src, uint32 bytesize);

void FlipByteOrder(uint8 *src, uint32 count);

// The following functions can encode/decode to unaligned addresses.

static INLINE void MDFN_en16lsb(uint8 *buf, uint16 morp)
{
 buf[0]=morp;
 buf[1]=morp>>8;
}

static INLINE void MDFN_en24lsb(uint8 *buf, uint32 morp)
{
 buf[0]=morp;
 buf[1]=morp>>8;
 buf[2]=morp>>16;
}


static INLINE void MDFN_en32lsb(uint8 *buf, uint32 morp)
{
 buf[0]=morp;
 buf[1]=morp>>8;
 buf[2]=morp>>16;
 buf[3]=morp>>24;
}

static INLINE void MDFN_en64lsb(uint8 *buf, uint64 morp)
{
 buf[0]=morp >> 0;
 buf[1]=morp >> 8;
 buf[2]=morp >> 16;
 buf[3]=morp >> 24;
 buf[4]=morp >> 32;
 buf[5]=morp >> 40;
 buf[6]=morp >> 48;
 buf[7]=morp >> 56;
}


// Overloaded functions, yay.
static INLINE void MDFN_enlsb(uint16 * buf, uint16 value)
{
 MDFN_en16lsb((uint8 *)buf, value);
}

static INLINE void MDFN_enlsb(uint32 * buf, uint32 value)
{
 MDFN_en32lsb((uint8 *)buf, value);
}

static INLINE void MDFN_enlsb(uint64 * buf, uint64 value)
{
 MDFN_en64lsb((uint8 *)buf, value);
}


static INLINE uint16 MDFN_de16lsb(uint8 *morp)
{
 return(morp[0] | (morp[1] << 8));
}


static INLINE uint32 MDFN_de24lsb(uint8 *morp)
{
 return(morp[0]|(morp[1]<<8)|(morp[2]<<16));
}

static INLINE uint32 MDFN_de32lsb(uint8 *morp)
{
 return(morp[0]|(morp[1]<<8)|(morp[2]<<16)|(morp[3]<<24));
}

static INLINE uint64 MDFN_de64lsb(uint8 *morp)
{
 uint64 ret = 0;

 ret |= morp[0];
 ret |= morp[1] << 8;
 ret |= morp[2] << 16;
 ret |= morp[3] << 24;
 ret |= (uint64)morp[4] << 32;
 ret |= (uint64)morp[5] << 40;
 ret |= (uint64)morp[6] << 48;
 ret |= (uint64)morp[7] << 56;

 return(ret);
}

static INLINE uint16 MDFN_delsb(uint16 *buf)
{
 return(MDFN_de16lsb((uint8 *)buf));
}

static INLINE uint32 MDFN_delsb(uint32 *buf)
{
 return(MDFN_de32lsb((uint8 *)buf));
}

static INLINE uint64 MDFN_delsb(uint64 *buf)
{
 return(MDFN_de64lsb((uint8 *)buf));
}

static INLINE uint32 MDFN_de32msb(uint8 *morp)
{
 return(morp[3]|(morp[2]<<8)|(morp[1]<<16)|(morp[0]<<24));
}

#endif
