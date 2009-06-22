//Thanks to TaoTao for the original source.
//http://www6.atpages.jp/~appsouko/

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MCM_SIGNATURE "MDFNMOVI"
#define MCM_HDR_SIZE 0x100
#define MCM_FRAME_SIZE 0x0B

static void warnv(const char* fmt, va_list ap)
{
    vfprintf(stderr, fmt, ap);
    putc('\n', stderr);
}

static void warn(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    warnv(fmt, ap);
    va_end(ap);
}

static void error(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    warnv(fmt, ap);
    va_end(ap);
    exit(1);
}

static void read_mcm(const char* path,
                     unsigned char** body, unsigned long* size)
{
    FILE* in;
    unsigned long filesize;
    unsigned char hdr[MCM_HDR_SIZE];

    if(!(in = fopen(path, "rb"))) error("Can't open file");

    fseek(in, 0, SEEK_END);
    filesize = ftell(in);
    fseek(in, 0, SEEK_SET);
    if(filesize <= MCM_HDR_SIZE) error("Not MCM file (filesize <= %d)", MCM_HDR_SIZE);
    if((filesize-MCM_HDR_SIZE) % MCM_FRAME_SIZE) error("Broken MCM file?");

    fread(hdr, 1, MCM_HDR_SIZE, in);

    if(0 != memcmp(hdr, MCM_SIGNATURE, 8)) error("Not MCM file (signature not found)");

    *size = filesize - MCM_HDR_SIZE;
    if(!(*body = (unsigned char*)malloc(*size))) error("malloc() failed");

    fread(*body, 1, *size, in);

    fclose(in);
}

static void dump_frames(const unsigned char* body, unsigned long size)
{
    const unsigned char* p;
    unsigned char pad;
    char pad_str[9];
    unsigned long frame_count;
    unsigned long i;
#if 0
    int j;
#endif

	freopen( "file.txt", "w", stdout );

    p = body;
    pad_str[8] = '\0';
    frame_count = size / MCM_FRAME_SIZE;

	printf("%s\n", "version 1");
	printf("%s\n", "emuVersion 9");
	printf("%s\n", "rerecordCount 1");

    for(i = 0; i < frame_count; ++i){
#if 0
        for(j = 0; j < MCM_FRAME_SIZE; ++j){
            printf("%02X ", p[j]);
        }
        putchar('\n');
#endif

        pad = p[1];
		pad_str[0] = (pad&0x10) ? 'U' : '.';
		pad_str[1] = (pad&0x40) ? 'D' : '.';
		pad_str[2] = (pad&0x80) ? 'L' : '.';
        pad_str[3] = (pad&0x20) ? 'R' : '.';
        pad_str[4] = (pad&0x02) ? '1' : '.';
        pad_str[5] = (pad&0x01) ? '2' : '.';
        pad_str[6] = (pad&0x08) ? 'N' : '.';
        pad_str[7] = (pad&0x04) ? 'S' : '.';

    //    printf("%lu %s\n", i, pad_str);
		int command = 0;
		
		printf("%s%d%s%s%s\n", "|", command, "|", pad_str, "|");

        p += MCM_FRAME_SIZE;
    }
}

static void mcmdump(const char* path)
{
    unsigned long size;
    unsigned char* body;

    read_mcm(path, &body, &size);

    dump_frames(body, size);

    free(body);
}

static void usage()
{
    error("Usage: mcmdump <MCM file>");
}

int main(int argc, char** argv)
{
    if(argc != 2) usage();

    mcmdump(argv[1]);

    return 0;
}
