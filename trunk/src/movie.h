#ifndef _MOVIE_H
#define _MOVIE_H

#include <vector>
#include <map>
#include <string>
#include <ostream>
#include <istream>
#ifdef _MSC_VER
#include "unixstuff.h"
#else
#include	<stdarg.h>
#include	<errno.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#endif
//#include "../../core.h"
//#include "utils/guid.h"
//#include "utils/md5.h"

typedef struct
{
	int movie_version;					// version of the movie format in the file
	uint32 num_frames;
	uint32 rerecord_count;
	bool poweron;
	uint32 emu_version_used;
//	MD5DATA md5_of_rom_used;
	std::string name_of_rom_used;

	std::vector<std::wstring> comments;
	std::vector<std::string> subtitles;
} MOVIE_INFO;

enum EMOVIEMODE
{
	MOVIEMODE_INACTIVE = 1,
	MOVIEMODE_RECORD = 2,
	MOVIEMODE_PLAY = 4,
};

enum EMOVIECMD
{
	MOVIECMD_RESET = 1,
	MOVIECMD_POWER = 2,
};

//RLDUTSBAYXWEG

class MovieData;
class MovieRecord
{

public:
	uint16 pad[5];
	
	union {
		struct {
			uint8 x, y;
			uint8 touch;
		};

		uint32 padding;
	} touch;
	
	//misc commands like reset, etc.
	//small now to save space; we might need to support more commands later.
	//the disk format will support up to 64bit if necessary
	uint8 commands;
	bool command_reset() { return (commands&MOVIECMD_RESET)!=0; }
	bool command_power() { return (commands&MOVIECMD_POWER)!=0; }
/*
	void toggleBit(int bit)
	{
		pad ^= mask(bit);
	}

	void setBit(int bit)
	{
		pad |= mask(bit);
	}

	void clearBit(int bit)
	{
		pad &= ~mask(bit);
	}

	void setBitValue(int bit, bool val)
	{
		if(val) setBit(bit);
		else clearBit(bit);
	}

	bool checkBit(int bit)
	{
		return (pad & mask(bit))!=0;
	}
*/
	void clear();
	
	//a waste of memory in lots of cases..  maybe make it a pointer later?
	//std::vector<char> savestate;

	void parse(MovieData* md, std::istream* is);
	bool parseBinary(MovieData* md, std::istream* is);
	void dump(MovieData* md, std::ostream* os, int index);
	void dumpBinary(MovieData* md, std::ostream *os, int index);
	void parsePad(std::istream* is, uint16& pad);
	void dumpPad(std::ostream* os, uint16 pad);
	
	static const char mnemonics[8];

private:
	int mask(int bit) { return 1<<bit; }
};


class MovieData
{
public:
	MovieData();
	

	int version;
	int emuVersion;
	//todo - somehow force mutual exclusion for poweron and reset (with an error in the parser)
	
//	MD5DATA romChecksum;
	std::string romFilename;
	std::vector<char> savestate;
	std::vector<MovieRecord> records;
	std::vector<std::wstring> comments;
	
	int rerecordCount;
//	Desmume_Guid guid;

	//was the frame data stored in binary?
	bool binaryFlag;

	int ports; //number of recorded ports

	int getNumRecords() { return records.size(); }

	class TDictionary : public std::map<std::string,std::string>
	{
	public:
		bool containsKey(std::string key)
		{
			return find(key) != end();
		}

		void tryInstallBool(std::string key, bool& val)
		{
			if(containsKey(key))
				val = atoi(operator [](key).c_str())!=0;
		}

		void tryInstallString(std::string key, std::string& val)
		{
			if(containsKey(key))
				val = operator [](key);
		}

		void tryInstallInt(std::string key, int& val)
		{
			if(containsKey(key))
				val = atoi(operator [](key).c_str());
		}

	};

	void truncateAt(int frame);
	void installValue(std::string& key, std::string& val);
	int dump(std::ostream *os, bool binary);
	void clearRecordRange(int start, int len);
	void insertEmpty(int at, int frames);
	
	static bool loadSavestateFrom(std::vector<char>* buf);
	static void dumpSavestateTo(std::vector<char>* buf, int compressionLevel);
	//void TryDumpIncremental();

private:
	void installInt(std::string& val, int& var)
	{
		var = atoi(val.c_str());
	}

	void installBool(std::string& val, bool& var)
	{
		var = atoi(val.c_str())!=0;
	}
};

extern int currFrameCounter;
extern EMOVIEMODE movieMode;		//adelikat: main needs this for frame counter display
extern MovieData currMovieData;		//adelikat: main needs this for frame counter display

bool FCEUI_MovieGetInfo(std::istream* fp, MOVIE_INFO& info, bool skipFrameCount);
void FCEUI_SaveMovie(const char *fname, std::wstring author);
void FCEUI_LoadMovie(const char *fname, bool _read_only, bool tasedit, int _pauseframe);
void FCEUI_StopMovie();
void NDS_setTouchFromMovie(void);
//void mov_savestate(std::ostream* os);
//bool mov_loadstate(std::istream* is, int size);
void LoadFM2_binarychunk(MovieData& movieData, std::istream* fp, int size);
extern bool movie_readonly;
void FCEUI_MakeBackupMovie(bool dispMessage);
void ToggleReadOnly();
//#endif

//C stuff

int MovieIsActive();
void HardResetGame();
void MakeMovieStateName(const char *filename);

void PauseOrUnpause(void);
void MakeMovieStateName(const char *filename);
char* GetMovieLengthStr();

void FCEUMOV_AddInputState();

void SaveStateMovie(char* filename);
void LoadStateMovie(char* filename);




//void MDFNI_SelectMovie(int);

#include "movie-driver.h" 
#include "state.h"
//void MDFNMOV_AddJoy(void *donutdata, uint32 donutlen);
//void MDFNMOV_CheckMovies(void);
void MDFNMOV_Stop(void);
void MDFNMOV_AddCommand(int cmd);
bool MDFNMOV_IsPlaying(void);
bool MDFNMOV_IsRecording(void);
void MDFNMOV_ForceRecord(StateMem *sm);
StateMem MDFNMOV_GrabRewindJoy(void);
void MDFNMOV_RecordState();

struct MovieStruct
{
	int status;
	FILE *fp;
	int readonly;
	int rerecords;
	int size;
	int frames;
	char* filename;
	int framelength;
	int headersize;
};

#define stopped	  1
#define recording 2
#define playback  3

struct MovieBufferStruct
{
	int size;
	char* data;
};

//void FCEUI_MakeBackupMovie(bool dispMessage);

bool CheckFileExists(const char* filename);

extern struct MovieStruct Movie;

void LoadMovieCLI(void);

void ResetVariables(void);

void setreadonlycli(int value);

void SetCurrent(int incurrent);
 
static char MovMD5Sum[33];
static char MovRomFilename[64];

void SetCommandAdded(void);

int DisplayTotalFrames(void);

void ReadHeader(FILE* headertest);

void WriteHeader(FILE* headertest);

void MovClearAllSRAM(void);

void SetNumberOfPorts(void);

void setMoviePlaybackPointer(uint32 value);

void MDFNMOV_Seek(FILE* fp);

FILE* getSlots();

void MDFNMOV_Count(FILE* fp);

int getreadonly(void);

//void setreadonly(void);

extern int current;

void AddRerecordCount(void);

//extern int RecordingSwitchToPlayback;

int MovInd(void);

int retisMov(void);

static void ReserveInputBufferSpace(uint32 spaceNeeded);

void Stoptemp(void);

void Starttemp(void);

StateMem Grabtempmov(void);

void incFrameCounter(void);

uint32 retFrameCounter(void);

uint32 setFrameCounter(uint32 value);

//static 

void Writetempmov(StateMem in);

static int CurrentMovie = 0;

int checkcurrent(void);

uint32 Getloc(void);

uint32 getmloc(void);


void MovieLoadState(void);

void ReplaceMovie(FILE* fp);

void CopyMovie(FILE* fp);

struct MovieBufferStruct ReadMovieIntoABuffer(FILE* fp);

void OpenMovie(const char * arg);

void MakeMovieFilename(const char* arg);

#endif




