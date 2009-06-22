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

#include        "mednafen.h"


#ifdef _MSC_VER
#include "unixstuff.h"
#else
#include	<stdarg.h>
#include	<errno.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#endif

#include        <string.h>
#include	<trio/trio.h>

#include "drivers/main.h"

#include	"netplay.h"
#include	"netplay-driver.h"
#include	"general.h"
#include	"endian.h"
#include        "memory.h"

#include	"state.h"
#include	"movie.h"
#include        "video.h"
#include	"file.h"
#include	"wave.h"
#include	"cdrom/cdromif.h"
#include	"mempatcher.h"
#include	"compress/minilzo.h"
#include	"tests.h"
#include	"video/vblur.h"
#include    "settings-driver.h"
#include    "pce/pce.h"
#include <iostream>

void MMRecord_End(void);
bool MMRecord_Start(const char *filename);
void MMRecord_WriteFrame(const EmulateSpecStruct *espec);


static const char *CSD_vblur = gettext_noop("Blur each frame with the last frame.");
static const char *CSD_vblur_accum = gettext_noop("Accumulate color data rather than discarding it.");
static const char *CSD_vblur_accum_amount = gettext_noop("Blur amount in accumulation mode, specified in percentage of accumulation buffer to mix with the current frame.");

static bool ValidateSetting(const char *name, const char *value)
{
 if(!strcasecmp(name, "srwcompressor"))
 {
  // If it doesn't match "minilzo", "quicklz", or "blz", error out!
  if(strcasecmp(value, "minilzo") && strcasecmp(value, "quicklz") && strcasecmp(value, "blz"))
   return(FALSE);
 }

 return(TRUE);
}

static MDFNSetting MednafenSettings[] =
{

  { "srwcompressor", gettext_noop("Compressor to use with state rewinding:  \"minilzo\", \"quicklz\", or \"blz\""), MDFNST_STRING, "minilzo", NULL, NULL, ValidateSetting },
  { "srwframes", gettext_noop("Number of frames to keep states for when state rewinding is enabled."), MDFNST_UINT, "600", "10", "99999" },
  { "snapname", gettext_noop("If value is true, use an alternate naming scheme(file base and numeric) for screen snapshots."), MDFNST_BOOL, "0"},
  { "dfmd5", gettext_noop("Include the MD5 hash of the loaded game in the filenames of the data file(save states, SRAM backups) Mednafen creates."), MDFNST_BOOL, "1" },

  #ifdef NEED_CDEMU
  { "cdrom.lec_eval", gettext_noop("Enable simple error correction of raw data sector rips by evaluating L-EC and EDC data."), MDFNST_BOOL, "1" },
  #endif

  { "lagadvance", gettext_noop("Automatically advance lag frames"), MDFNST_BOOL, "0" },
  { "author", gettext_noop("Author's name"), MDFNST_STRING, "" },
  { "mov", gettext_noop("Path to the movie to be played/recorded to"), MDFNST_STRING, "mov PATH NOT SET" },
  { "play", gettext_noop("Start playing the current movie immediately on startup"), MDFNST_BOOL, "0" },
  { "record", gettext_noop("Start recording the current movie immediately on startup"), MDFNST_BOOL, "0" },
  { "pause", gettext_noop("Start the emulator paused"), MDFNST_BOOL, "0" },
  { "loadstate", gettext_noop("Load the specified state"), MDFNST_STRING, "" },
  { "readonly", gettext_noop("Start the emulator in read only mode"), MDFNST_BOOL, "1" },
  { "allowlrud", gettext_noop("Enable/Disable button exclusion"), MDFNST_BOOL, "0" },
  { "mmm", gettext_noop("Record an mmm"), MDFNST_BOOL, "0" },
  { "mmmframes", gettext_noop("Number of frames to keep states for when state rewinding is enabled."), MDFNST_UINT, "0", "0", "999999" },
  { "mmmfile", gettext_noop("Record mmm to this file"), MDFNST_STRING, "" },

  //{ "recordmov", gettext_noop("Path to the movie to be recorded to"), MDFNST_STRING, "recordmov PATH NOT SET" },


  { "path_snap", gettext_noop("Path override for screen snapshots."), MDFNST_STRING, "" },
  { "path_sav", gettext_noop("Path override for save games and nonvolatile memory."), MDFNST_STRING, "" },
  { "path_state", gettext_noop("Path override for save states."), MDFNST_STRING, "" },
  { "path_movie", gettext_noop("Path override for movies."), MDFNST_STRING, "" },
  { "path_cheat", gettext_noop("Path override for cheats."), MDFNST_STRING, "" },
  { "path_palette", gettext_noop("Path override for custom palettes."), MDFNST_STRING, "" },

  { "filesys.snap_samedir", gettext_noop("Write screen snapshots to the same directory the ROM/disk/disc image is in."),
        MDFNST_BOOL, "0" },

  { "filesys.sav_samedir", gettext_noop("Write/Read save games and nonvolatile memory to/from the same directory the ROM/disk/disc image is in."),
	MDFNST_BOOL, "0" },

  { "filesys.state_samedir", gettext_noop("Write/Read save states to/from the same directory the ROM/disk/disc image is in."),
        MDFNST_BOOL, "0" },

  { "filesys.movie_samedir", gettext_noop("Write/Read movies to/from the same directory the ROM/disk/disc image is in."),
        MDFNST_BOOL, "0" },

  { "filesys.disablesavegz", gettext_noop("Disable gzip compression when saving save states and backup memory."), MDFNST_BOOL, "0" },


  { "gb.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "gb.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "gb.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "gba.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "gba.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "gba.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "gg.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "gg.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "gg.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "lynx.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "lynx.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "lynx.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "nes.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "nes.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "nes.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "ngp.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "ngp.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "ngp.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "pce.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "pce.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "pce.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "pcfx.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "pcfx.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "pcfx.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "sms.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "sms.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "sms.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { "wswan.vblur", CSD_vblur, MDFNST_BOOL, "0" },
  { "wswan.vblur.accum", CSD_vblur_accum, MDFNST_BOOL, "0" },
  { "wswan.vblur.accum.amount", CSD_vblur_accum_amount, MDFNST_FLOAT, "50", "0", "100" },

  { NULL }
};

static char *PortDeviceCache[16];
static void *PortDataCache[16];
static uint32 PortDataLenCache[16];

MDFNGI *MDFNGameInfo = NULL;
static bool CDInUse = 0;
static float LastSoundMultiplier;

void MDFNI_CloseGame(void)
{
 if(MDFNGameInfo)
 {
  #ifdef NETWORK
  if(MDFNnetplay)
   MDFNI_NetplayStop();
  #endif
  MDFNMOV_Stop();
  MDFNGameInfo->CloseGame();
  if(MDFNGameInfo->name)
  {
   free(MDFNGameInfo->name);
   MDFNGameInfo->name=0;
  }
  MDFNMP_Kill();

  MDFNGameInfo = NULL;
  MDFN_StateEvilEnd();
  #ifdef NEED_CDEMU
  if(CDInUse)
  {
   CDIF_Close();
   CDInUse = 0;
  }
  #endif
 }
 VBlur_Kill();

 #ifdef WANT_DEBUGGER
 MDFNDBG_Kill();
 #endif
}

int MDFNI_NetplayStart(uint32 local_players, uint32 netmerge, const std::string &nickname, const std::string &game_key, const std::string &connect_password)
{
 return(NetplayStart((const char**)PortDeviceCache, PortDataLenCache, local_players, netmerge, nickname, game_key, connect_password));
}


#ifdef WANT_NES_EMU
extern MDFNGI EmulatedNES;
#endif

#ifdef WANT_GBA_EMU
extern MDFNGI EmulatedGBA;
#endif

#ifdef WANT_GB_EMU
extern MDFNGI EmulatedGB;
#endif

#ifdef WANT_LYNX_EMU
extern MDFNGI EmulatedLynx;
#endif

#ifdef WANT_NGP_EMU
extern MDFNGI EmulatedNGP;
#endif

#ifdef WANT_PCE_EMU
extern MDFNGI EmulatedPCE;
#endif

#ifdef WANT_PCFX_EMU
extern MDFNGI EmulatedPCFX;
#endif

#ifdef WANT_WSWAN_EMU
extern MDFNGI EmulatedWSwan;
#endif

#ifdef WANT_SMS_EMU
extern MDFNGI EmulatedSMS, EmulatedGG;
#endif

MDFNGI *MDFNSystems[] = 
{ 
 #ifdef WANT_NES_EMU
 &EmulatedNES,
 #endif

 #ifdef WANT_GB_EMU
 &EmulatedGB, 
 #endif

 #ifdef WANT_GBA_EMU
 &EmulatedGBA,
 #endif

 #ifdef WANT_PCE_EMU
 &EmulatedPCE,
 #endif

 #ifdef WANT_LYNX_EMU
 &EmulatedLynx, 
 #endif

 #ifdef WANT_PCFX_EMU
 &EmulatedPCFX, 
 #endif

 #ifdef WANT_NGP_EMU
 &EmulatedNGP,
 #endif

 #ifdef WANT_WSWAN_EMU
 &EmulatedWSwan,
 #endif

 #ifdef WANT_SMS_EMU
 &EmulatedSMS,
 &EmulatedGG,
 #endif
};

unsigned int MDFNSystemCount = sizeof(MDFNSystems) / sizeof(MDFNGI *);

#ifdef NEED_CDEMU
MDFNGI *MDFNI_LoadCD(const char *sysname, const char *devicename)
{
 MDFNI_CloseGame();

 LastSoundMultiplier = 1;

 int ret = CDIF_Open(devicename);
 if(!ret)
 {
  MDFN_PrintError(_("Error opening CD."));
  return(0);
 }

 if(sysname == NULL)
 {
  uint8 sector_buffer[2048];

  memset(sector_buffer, 0, sizeof(sector_buffer));

  sysname = "pce";

  for(int32 track = CDIF_GetFirstTrack(); track <= CDIF_GetLastTrack(); track++)
  {
   CDIF_Track_Format format;
   if(CDIF_GetTrackFormat(track, format) && format == CDIF_FORMAT_MODE1)
   {
    CDIF_ReadSector(sector_buffer, NULL, CDIF_GetTrackStartPositionLBA(track), 1);
    if(!strncmp("PC-FX:Hu_CD-ROM", (char*)sector_buffer, strlen("PC-FX:Hu_CD-ROM")))
    {
     sysname = "pcfx";
     break;
    }
   }
  }

 } 

 for(unsigned int x = 0; x < MDFNSystemCount; x++)
 {
  if(!strcasecmp(MDFNSystems[x]->shortname, sysname))
  {
   if(!MDFNSystems[x]->LoadCD)
   {
    MDFN_PrintError(_("Specified system \"%s\" doesn't support CDs!"), sysname);
    return(0);
   }
   MDFNGameInfo = MDFNSystems[x];

   if(!(MDFNSystems[x]->LoadCD()))
   {
    CDIF_Close();
    MDFNGameInfo = NULL;
    return(0);
   }
   CDInUse = 1;

   #ifdef WANT_DEBUGGER
   MDFNDBG_PostGameLoad(); 
   #endif

   MDFNSS_CheckStates();
   //MDFNMOV_CheckMovies();

   MDFN_ResetMessages();   // Save state, status messages, etc.

   VBlur_Init();

   MDFN_StateEvilBegin();
   return(MDFNGameInfo);
  }
 }
 MDFN_PrintError(_("Unrecognized system \"%s\"!"), sysname);
 return(0);
}
#endif



MDFNGI *MDFNI_LoadGame(const char *name)
{
        MDFNFILE *fp;
	struct stat stat_buf;
	static const char *valid_iae =	".nes\0.fds\0.nsf\0.nsfe\0.unf\0.unif\0.nez\0.gb\0.gbc\0.gba\0.agb\0.cgb\0.bin\0.pce\0.hes\0.sgx\0.ngp\0.ngc\0.ws\0.wsc\0";

	#ifdef NEED_CDEMU
	if(strlen(name) > 4 && (!strcasecmp(name + strlen(name) - 4, ".cue") || !strcasecmp(name + strlen(name) - 4, ".toc")))
	{
	 return(MDFNI_LoadCD(NULL, name));
	}
	
	if(!stat(name, &stat_buf) && !S_ISREG(stat_buf.st_mode))
	{
		MDFNGI * tmp = MDFNI_LoadCD(NULL, name);
	 if(tmp) return tmp;
	}
	#endif

	MDFNI_CloseGame();

	LastSoundMultiplier = 1;

	MDFNGameInfo = NULL;

	MDFN_printf(_("Loading %s...\n\n"),name);

	MDFN_indent(1);

        GetFileBase(name);

	fp=MDFN_fopen(name, MDFN_MakeFName(MDFNMKF_IPS,0,0).c_str(),"rb", valid_iae);
	if(!fp)
        {
	 MDFNGameInfo = NULL;
	 return 0;
	}

	for(unsigned int x = 0; x < MDFNSystemCount; x++)
	{
	 int t;

	 if(!MDFNSystems[x]->Load) continue;
         MDFNGameInfo = MDFNSystems[x];
         MDFNGameInfo->soundchan = 0;
         MDFNGameInfo->soundrate = 0;
         MDFNGameInfo->name = NULL;
         MDFNGameInfo->rotated = 0;

	 t = MDFNGameInfo->Load(name, fp);

	 if(t == 0)
	 {
	  MDFN_fclose(fp);
	  MDFN_indent(-1);
	  MDFNGameInfo = NULL;
	  return(0);
	 }	 
	 else if(t == -1)
	 {
	  if(x == MDFNSystemCount - 1)
	  {
           MDFN_PrintError(_("Unrecognized file format.  Sorry."));
           MDFN_fclose(fp);
           MDFN_indent(-1);
	   MDFNGameInfo = NULL;
           return 0;
	  }
	 }
	 else
	  break;	// File loaded successfully.
	}

        MDFN_fclose(fp);

	#ifdef WANT_DEBUGGER
	MDFNDBG_PostGameLoad();
	#endif

	MDFNSS_CheckStates();
	//MDFNMOV_CheckMovies();

	MDFN_ResetMessages();	// Save state, status messages, etc.

	MDFN_indent(-1);

	if(!MDFNGameInfo->name)
        {
         unsigned int x;
         char *tmp;

         MDFNGameInfo->name = (UTF8 *)strdup(GetFNComponent(name));

         for(x=0;x<strlen((char *)MDFNGameInfo->name);x++)
         {
          if(MDFNGameInfo->name[x] == '_')
           MDFNGameInfo->name[x] = ' ';
         }
         if((tmp = strrchr((char *)MDFNGameInfo->name, '.')))
          *tmp = 0;
        }

	VBlur_Init();

        MDFN_StateEvilBegin();
        return(MDFNGameInfo);
}

int MDFNI_Initialize(char *basedir, const std::vector<MDFNSetting> &DriverSettings)
{
        MDFNI_printf(_("Starting Mednafen %s\n"), MEDNAFEN_VERSION);
        MDFN_indent(1);

	if(!MDFN_RunMathTests())
	{
	 return(0);
	}


	memset(PortDataCache, 0, sizeof(PortDataCache));
	memset(PortDataLenCache, 0, sizeof(PortDataLenCache));
	memset(PortDeviceCache, 0, sizeof(PortDeviceCache));

	lzo_init();

	MDFNI_SetBaseDirectory(basedir);

        memset(&FSettings,0,sizeof(FSettings));

	FSettings.SoundVolume=100;
	FSettings.soundmultiplier = 1;
	MDFN_InitFontData();

	// First merge all settable settings, then load the settings from the SETTINGS FILE OF DOOOOM
	MDFN_MergeSettings(MednafenSettings);
	MDFN_MergeSettings(MDFNMP_Settings);

	if(DriverSettings.size())
 	 MDFN_MergeSettings(DriverSettings);

	for(unsigned int x = 0; x < MDFNSystemCount; x++)
	 MDFN_MergeSettings(MDFNSystems[x]);

        if(!MFDN_LoadSettings(basedir))
	 return(0);

	#ifdef WANT_DEBUGGER
	MDFNDBG_Init();
	#endif

        return(1);
}

void MDFNI_Kill(void)
{

 MDFNI_SetSetting("mov", "mov PATH NOT SET");
 MDFNI_SetSetting("loadstate", "loadstate PATH NOT SET");
 
 MDFN_SaveSettings();
 //MDFNNES_Kill();

 for(unsigned int x = 0; x < sizeof(PortDeviceCache) / sizeof(char *); x++)
 {
  if(PortDeviceCache[x])
  {
   free(PortDeviceCache[x]);
   PortDeviceCache[x] = NULL;
  }
 }
}

//so that a movie doesn't play again after finishing when using -play
int alreadyplayed;

//so that the emulator only pauses on startup
int alreadypaused;


//so that your readonly command line setting is only done once
int alreadyreadonly;

int templagFlag;

bool retjustLagged(void) {
	return(justLagged); 
}

bool startedwriting=false;
int moviecounter=0;
int endingframe=0;
int wantrecording=0;
int alreadyloadedstate=0;
int alreadyrecorded=0;
//extern uint32 FrameCounter;

void MDFNI_Emulate(EmulateSpecStruct *espec) //uint32 *pXBuf, MDFN_Rect *LineWidths, int16 **SoundBuf, int32 *SoundBufSize, int skip, float soundmultiplier)
{

SetLagFlag(1); 
//incFrameCounter();


 FSettings.soundmultiplier = espec->soundmultiplier;

 if(espec->soundmultiplier != LastSoundMultiplier)
 {
  MDFNGameInfo->SetSoundMultiplier(espec->soundmultiplier);
  LastSoundMultiplier = espec->soundmultiplier;
 }

 if(MDFN_GetSettingB("pause") == 1 && alreadypaused == 0)  {
 
DoPause();
alreadypaused = 1;

 }

 if(alreadyreadonly == 0)  {

	 int tempvalue; 
	 tempvalue = MDFN_GetSettingB("readonly");

	// setreadonlycli(tempvalue);
	 alreadyreadonly = 1;
 }

 //for loading a movie upon start
 if(!MDFNMOV_IsPlaying() && MDFN_GetSettingB("play") == 1 && alreadyplayed == 0)
  {

	MDFNI_LoadMovie(NULL);
	alreadyplayed = 1;
	//so it doesn't play again next time you open the emulator
	MDFNI_SetSettingB("play", 0);
 }

  if(!MDFNMOV_IsPlaying() && MDFN_GetSettingB("record") == 1 && alreadyrecorded == 0)
  {

	  //MDFNI_SaveMovie(NULL, NULL, NULL);
	//MDFNI_LoadMovie(NULL);
	alreadyrecorded = 1;
	//so it doesn't play again next time you open the emulator
	MDFNI_SetSettingB("record", 0);
 }
/*
 if(!alreadyloadedstate) {
	 if(FrameCounter > 1) {

		//if someone specifies loading a state with loadstate
		if(strcmp(MDFN_GetSettingS("loadstate").c_str(),"loadstate PATH NOT SET")) {

			MDFNI_LoadState(MDFN_GetSettingS("loadstate").c_str(), NULL);
			alreadyloadedstate=1;
		}
	 }
 }
*/
 #ifdef NETWORK
 if(MDFNnetplay)
 {
  NetplayUpdate((const char**)PortDeviceCache, PortDataCache, PortDataLenCache, MDFNGameInfo->InputInfo->InputPorts);
 }
 #endif

// FCEUMOV_AddInputState(PortDataCache[0]);
/*
 for(int x = 0; x < 16; x++)
	 if(PortDataCache[x]) {
   MDFNMOV_AddJoy(PortDataCache[x], PortDataLenCache[x]);
	 }
*/
	 //SetCommandAdded();

 if(VBlur_IsOn())
  espec->skip = 0;

 if(espec->NeedRewind)
 {
  if(MDFNMOV_IsPlaying())
  {
   espec->NeedRewind = 0;
   MDFN_DispMessage((UTF8*)_("Can't rewind during movie playback(yet!)."));
  }
  else if(MDFNnetplay)
  {
   espec->NeedRewind = 0;
   MDFN_DispMessage((UTF8*)_("Silly-billy, can't rewind during netplay."));
  }
  else if(MDFNGameInfo->GameType == GMT_PLAYER)
  {
   espec->NeedRewind = 0;
   MDFN_DispMessage((UTF8*)_("Music player rewinding is unsupported."));
  }
 }

 if (MDFN_StateEvil(espec->NeedRewind))
	 espec->NeedSoundReverse = true;
 else
	 espec->NeedSoundReverse = false;

 MDFNGameInfo->Emulate(espec);

 VBlur_Run(espec);

 if(espec->SoundBuf && espec->SoundBufSize)
 {
  if(espec->NeedSoundReverse)
  {
   int16 *yaybuf = *(espec->SoundBuf);
   int32 slen = *(espec->SoundBufSize);

   if(MDFNGameInfo->soundchan == 1)
   {
    for(int x = 0; x < (slen / 2); x++)    
    {
     int16 cha = yaybuf[slen - x - 1];
     yaybuf[slen - x - 1] = yaybuf[x];
     yaybuf[x] = cha;
    }
   }
   else if(MDFNGameInfo->soundchan == 2)
   {
    for(int x = 0; x < (slen * 2) / 2; x++)
    {
     int16 cha = yaybuf[slen * 2 - (x&~1) - ((x&1) ^ 1) - 1];
     yaybuf[slen * 2 - (x&~1) - ((x&1) ^ 1) - 1] = yaybuf[x];
     yaybuf[x] = cha;
    }
   }
  }
  MDFN_WriteWaveData(*(espec->SoundBuf), *(espec->SoundBufSize)); /* This function will just return if sound recording is off. */
 }

 wantrecording = MDFN_GetSettingB("mmm");
 endingframe = MDFN_GetSettingUI("mmmframes");

 if(wantrecording) {

if(!startedwriting) {

std::string path = MDFN_GetSettingS("mmmfile");

MMRecord_Start(path.c_str());

	startedwriting=true;
}

if(startedwriting && moviecounter < endingframe) {
MMRecord_WriteFrame(espec);

moviecounter++;
	std::cout << moviecounter << std::endl;
}

if(moviecounter >= endingframe) {

MMRecord_End();
MDFN_DispMessage((UTF8 *)_("Finished"));
MDFNI_SetSettingB("mmm", 0);
}
 }

 if (GetLagFlag() == 1) 
	{
		lagCounter++;
		justLagged = true;
	}
	else justLagged = false;
}

int GetlagCounter(void) {

	return(lagCounter);
}

void SetlagCounter(int input) {

	lagCounter = input;
}

int ResetlagCounter(void) {

	lagCounter=0;
	return 0;
}



// This function should only be called for state rewinding.
// FIXME:  Add a macro for SFORMAT structure access instead of direct access
int MDFN_RawInputStateAction(StateMem *sm, int load, int data_only)
{
 static const char *stringies[16] = { "RI00", "RI01", "RI02", "RI03", "RI04", "RI05", "RI06", "RI07", "RI08", "RI09", "RI0a", "RI0b", "RI0c", "RI0d", "RI0e", "RI0f" };
 SFORMAT StateRegs[17];
 int x;

 for(x = 0; x < 16; x++)
 {
  StateRegs[x].desc = stringies[x];
  if(PortDataCache[x])
  {
   StateRegs[x].v = PortDataCache[x];
   StateRegs[x].s = PortDataLenCache[x];
  }
  else
  {
   StateRegs[x].v = NULL;
   StateRegs[x].s = 0;
  }
 }

 StateRegs[x].v = NULL;
 StateRegs[x].s = 0;
 StateRegs[x].desc = NULL;

 int ret = MDFNSS_StateAction(sm, load, data_only, StateRegs, "rinp");

 return(ret);
}


MDFNS FSettings;

static int curindent = 0;

void MDFN_indent(int indent)
{
 curindent += indent;
}

static uint8 lastchar = 0;
void MDFN_printf(const char *format, ...)
{
 char *format_temp;
 char *temp;
 unsigned int x, newlen;

 va_list ap;
 va_start(ap,format);


 // First, determine how large our format_temp buffer needs to be.
 uint8 lastchar_backup = lastchar; // Save lastchar!
 for(newlen=x=0;x<strlen(format);x++)
 {
  if(lastchar == '\n' && format[x] != '\n')
  {
   int y;
   for(y=0;y<curindent;y++)
    newlen++;
  }
  newlen++;
  lastchar = format[x];
 }

 format_temp = (char *)malloc(newlen + 1); // Length + NULL character, duh
 
 // Now, construct our format_temp string
 lastchar = lastchar_backup; // Restore lastchar
 for(newlen=x=0;x<strlen(format);x++)
 {
  if(lastchar == '\n' && format[x] != '\n')
  {
   int y;
   for(y=0;y<curindent;y++)
    format_temp[newlen++] = ' ';
  }
  format_temp[newlen++] = format[x];
  lastchar = format[x];
 }

 format_temp[newlen] = 0;

 temp = trio_vaprintf(format_temp, ap);
 free(format_temp);

 MDFND_Message(temp);
 free(temp);

 va_end(ap);
}

void MDFN_PrintError(const char *format, ...)
{
 char *temp;

 va_list ap;

 va_start(ap, format);

 temp = trio_vaprintf(format, ap);
 MDFND_PrintError(temp);
 free(temp);

 va_end(ap);
}

MDFNException::MDFNException()
{


}

MDFNException::~MDFNException()
{


}

void MDFNException::AddPre(const char *format, ...)
{
 char oldmsg[sizeof(TheMessage)];

 strcpy(oldmsg, TheMessage);

 va_list ap;
 va_start(ap, format);
 trio_vsnprintf(oldmsg, sizeof(TheMessage), format, ap);
 va_end(ap);

 int freelen = sizeof(TheMessage) - strlen(TheMessage);
 strncpy(TheMessage + strlen(TheMessage), oldmsg, freelen);
}

void MDFNException::AddPost(const char *format, ...)
{
 int freelen = sizeof(TheMessage) - strlen(TheMessage);

 if(freelen <= 0)
 {
  puts("ACKACKACK Exception erorrorololoz");
  return;
 }

 va_list ap;

 va_start(ap, format);
 trio_vsnprintf(TheMessage + strlen(TheMessage), freelen, format, ap);
 va_end(ap);
}


void MDFNI_SetPixelFormat(int rshift, int gshift, int bshift, int ashift)
{
 FSettings.rshift = rshift;
 FSettings.gshift = gshift;
 FSettings.bshift = bshift;
 FSettings.ashift = ashift;

 if(MDFNGameInfo)
  MDFNGameInfo->SetPixelFormat(rshift, gshift, bshift);
}


void MDFN_DoSimpleCommand(int cmd)
{
 MDFNGameInfo->DoSimpleCommand(cmd);
}

void MDFN_QSimpleCommand(int cmd)
{
 #ifdef NETWORK
 if(MDFNnetplay)
  MDFNNET_SendCommand(cmd, 0);
 else
 #endif
 {
  if(!MDFNMOV_IsPlaying())
  {
   MDFN_DoSimpleCommand(cmd);
   //MDFNMOV_AddCommand(cmd);
  }
 }
}

void MDFNI_Power(void)
{
 if(MDFNGameInfo)
  MDFN_QSimpleCommand(MDFNNPCMD_POWER);
}

void MDFNI_Reset(void)
{
 if(MDFNGameInfo)
  MDFN_QSimpleCommand(MDFNNPCMD_RESET);
}


void MDFNI_SetSoundVolume(uint32 volume)
{
 FSettings.SoundVolume=volume;
 if(MDFNGameInfo)
 {
  MDFNGameInfo->SetSoundVolume(volume);
 }
}

void MDFNI_Sound(int Rate)
{
 FSettings.SndRate=Rate;
 if(MDFNGameInfo)
 {
  MDFNGameInfo->Sound(Rate);
 }
}

void MDFNI_ToggleLayer(int which)
{
 if(MDFNGameInfo)
 {
  const char *goodies = MDFNGameInfo->LayerNames;
  int x = 0;
  while(x != which)
  {
   while(*goodies)
    goodies++;
   goodies++;
   if(!*goodies) return; // ack, this layer doesn't exist.
   x++;
  }
  if(MDFNGameInfo->ToggleLayer(which))
   MDFN_DispMessage((UTF8*)_("%s enabled."), _(goodies));
  else
   MDFN_DispMessage((UTF8*)_("%s disabled."), _(goodies));
 }
}

//initialization, not regular emulation
//however this sets up a pointer, which points to another pointer, which.....
//that eventually gets you to the emulated console

void MDFNI_SetInput(int port, const char *type, void *ptr, uint32 ptr_len_thingy)
{
 if(MDFNGameInfo)
 {
  assert(port < 16);

  PortDataCache[port] = ptr;
  PortDataLenCache[port] = ptr_len_thingy;

  if(PortDeviceCache[port])
  {
   free(PortDeviceCache[port]);
   PortDeviceCache[port] = NULL;
  }

  PortDeviceCache[port] = strdup(type);

  MDFNGameInfo->SetInput(port, type, ptr);
 }
}

int MDFNI_DiskInsert(int oride)
{
 if(MDFNGameInfo && MDFNGameInfo->DiskInsert)
  return(MDFNGameInfo->DiskInsert(oride));

 return(0);
}

int MDFNI_DiskEject(void)
{
 if(MDFNGameInfo && MDFNGameInfo->DiskEject)
  return(MDFNGameInfo->DiskEject());

 return(0);
}

int MDFNI_DiskSelect(void)
{
 if(MDFNGameInfo && MDFNGameInfo->DiskSelect)
  return(MDFNGameInfo->DiskSelect());

 return(0);
}

#ifdef _MSC_VER
#pragma pack(push,1)
#endif

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
//} 

//__attribute__((__packed__)) MM_Main_Header;

#ifdef _MSC_VER
} MM_Main_Header;
#pragma pack(pop)
#else
} __attribute__((__packed__)) MM_Main_Header;
#endif

static FILE *output_fp = NULL;
static uint8 *pixel_buffer = NULL;
static uint32 compress_buffer_size;
static uint8 *compress_buffer = NULL;

// Filled out partially and written when movie playback starts, completed and rewritten when movie playback ends.
static MM_Main_Header main_header;

static uint64 video_frame_counter;
static uint64 audio_frame_counter;
static uint32 min_width, max_width, min_height, max_height;
static uint32 max_aframes_per_vframe;

static uint64 LastFrameOffset;
static uint64 LastFrameTime;

static std::vector<uint32> FrameRelativeIndex;
static std::vector<uint32> FrameRelativeTime;
/*
int WriteMovie(void) {

 output_fp = fopen("movietest.txt", "wb");
 if(!output_fp)
 {
  puts("fuck");
 // MDFN_PrintError("%m\n", errno);
  return(0);
 }

//TODO get proper height
 //if(!(pixel_buffer = (uint8 *)MDFN_malloc(MDFNGameInfo->pitch * MDFNGameInfo->fb_height, _("compression buffer"))))
  if(!(pixel_buffer = (uint8 *)MDFN_malloc(MDFNGameInfo->pitch * 232, _("compression buffer"))))
 {
  fclose(output_fp);
  return(0);
 }

 //if(!(pixel_buffer = (uint8 *)MDFN_malloc(MDFNGameInfo->pitch * MDFNGameInfo->fb_height, _("compression buffer"))))
// {
 // fclose(output_fp);
 // return(0);
// }

 compress_buffer_size = MDFNGameInfo->pitch * 232 * 1.10;//TODO proper height
 if(!(compress_buffer = (uint8 *)MDFN_malloc(compress_buffer_size, _("compression buffer"))))
 {
  free(pixel_buffer);
  fclose(output_fp);
  return(0);
 }

 audio_frame_counter = 0;
 video_frame_counter = 0;

 min_width = min_height = 0xFFFFFFFF;
 max_width = max_height = 0x00000000;

 max_aframes_per_vframe = 1;

 LastFrameOffset = 0;
 LastFrameTime = 0;

 FrameRelativeIndex.clear(); 
 FrameRelativeTime.clear();

 //compress_buffer_size = MDFNGameInfo->pitch * 320 * 1.10;
 //if(!(compress_buffer = (uint8 *)MDFN_malloc(compress_buffer_size, _("compression buffer"))))
 //{
 //free(pixel_buffer);
//  fclose(output_fp);
//  return(0);
// }

return 1;

}
*/

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

/*
static INLINE void MDFN_en32lsb(uint8 *buf, uint32 morp)
{
 buf[0]=morp;
 buf[1]=morp>>8;
 buf[2]=morp>>16;
 buf[3]=morp>>24;
}
*/
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


bool MMRecord_Start(const char *filename)
{

int nominal_width=320;
int nominal_height=232;
int fb_height=232;

	MM_Main_Header header;

	memset(&header, 0, sizeof(MM_Main_Header));

	output_fp = fopen(filename, "wb");
	if(!output_fp)
	{
		MDFN_PrintError("%m\n", errno);
		return(0);
	}

	memcpy(main_header.magic, "Mednafen M Movie", 16);

	MDFN_enlsb(&main_header.version, 0);
	MDFN_enlsb(&main_header.video_compressor, 1);	// MiniLZO
	MDFN_enlsb(&main_header.audio_compressor, 0);	// Raw

	MDFN_enlsb(&main_header.nominal_width, nominal_width);
	MDFN_enlsb(&main_header.nominal_height, nominal_height);

	// 32.32 sound rate
	MDFN_enlsb(&main_header.sound_rate, (uint64)FSettings.SndRate << 32);

	// Number of sound channels
	MDFN_enlsb(&main_header.sound_channels, MDFNGameInfo->soundchan);


	fwrite(&main_header, 1, sizeof(MM_Main_Header), output_fp);

	if(!(pixel_buffer = (uint8 *)MDFN_malloc(MDFNGameInfo->pitch * fb_height, _("compression buffer"))))
	{
		fclose(output_fp);
		return(0);
	}

	compress_buffer_size = MDFNGameInfo->pitch * fb_height * 1.10;
	if(!(compress_buffer = (uint8 *)MDFN_malloc(compress_buffer_size, _("compression buffer"))))
	{
		free(pixel_buffer);
		fclose(output_fp);
		return(0);
	}

	audio_frame_counter = 0;
	video_frame_counter = 0;

	min_width = min_height = 0xFFFFFFFF;
	max_width = max_height = 0x00000000;

	max_aframes_per_vframe = 1;

	LastFrameOffset = 0;
	LastFrameTime = 0;

	FrameRelativeIndex.clear(); 
	FrameRelativeTime.clear();

	return(1);
}
#include <iostream>

static bool WriteChunk(const EmulateSpecStruct *espec)
{
uint32 frame_count = *espec->SoundBufSize;
 //uint32 frame_count = espec->SoundBufSize;
std::cout << *espec->SoundBufSize << std::endl;
std::cout << "frame_count" << frame_count << std::endl;
 const char *video_header = "FRAME\0\0";
 uint16 height = MDFNGameInfo->DisplayRect.h;
 uint16 width = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.w : 0;
 uint8 *pb_ptr = pixel_buffer;
 uint8 workmem[LZO1X_1_MEM_COMPRESS];
 lzo_uint compressed_len;
 
 uint32 compressed_size;
 uint32 uncompressed_size;

 FrameRelativeIndex.push_back(ftell(output_fp) - LastFrameOffset);
 FrameRelativeTime.push_back(LastFrameTime);

 LastFrameOffset = ftell(output_fp);
 LastFrameTime = frame_count;

 fwrite(video_header, 1, 8, output_fp);

 fwrite(&width, 1, 2, output_fp);
 fwrite(&height, 1, 2, output_fp);

 if(height > max_height)
  max_height = height;

 if(height < min_height)
  min_height = height;

 for(int y = MDFNGameInfo->DisplayRect.y; y < MDFNGameInfo->DisplayRect.y + MDFNGameInfo->DisplayRect.h; y++)
 {
  uint16 meow_width = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.w : espec->LineWidths[y].w;
  int meow_x = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.x : espec->LineWidths[y].x;
  uint32 *fb_line = espec->pixels + y * (MDFNGameInfo->pitch >> 2) + meow_x;

  //if(!width)
  //{
  // fwrite(&meow_width, 1, 2, output_fp);
  //}

  if(meow_width > max_width)
   max_width = meow_width;

  if(meow_width < min_width)
   min_width = meow_width;

  for(int x = 0; x < meow_width; x++)
  {
   uint32 pixel = fb_line[x];
   int r, g, b;

 //  espec->surface->DecodeColor(pixel, r, g, b);
   DECOMP_COLOR(pixel, r, g, b);

   *pb_ptr++ = r;
   *pb_ptr++ = g;
   *pb_ptr++ = b;
  }
 }

 uncompressed_size = pb_ptr - pixel_buffer;
 lzo1x_1_compress(pixel_buffer, uncompressed_size, compress_buffer, &compressed_len, workmem);
 compressed_size = compressed_len;

 fwrite(&compressed_size, 1, 4, output_fp);
 fwrite(&uncompressed_size, 1, 4, output_fp);
 fwrite(&frame_count, 1, 4, output_fp);

 if(!width)
 {
  for(int y = MDFNGameInfo->DisplayRect.y; y < MDFNGameInfo->DisplayRect.y + MDFNGameInfo->DisplayRect.h; y++)
  {
   uint16 meow_width = espec->LineWidths[y].w;
   fwrite(&meow_width, 1, 2, output_fp);
  }
 }

 fwrite(compress_buffer, 1, compressed_len, output_fp);


 fwrite(*espec->SoundBuf, 2 * MDFNGameInfo->soundchan, frame_count, output_fp);

 if(max_aframes_per_vframe < frame_count)
  max_aframes_per_vframe = frame_count;

 audio_frame_counter += frame_count;

 video_frame_counter++;

 return(1);
}
/* sort of works
static bool WriteChunk(const EmulateSpecStruct *espec)
{
	uint32 frame_count = 1536;//(uint32*)espec->SoundBufSize;// TODO
	//uint32 frame_count = 100;

	const char *video_header = "FRAME\0\0";
	uint16 height = MDFNGameInfo->DisplayRect.h;
	uint16 width = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.w : 0;
	uint8 *pb_ptr = pixel_buffer;
	uint8 workmem[LZO1X_1_MEM_COMPRESS];
	lzo_uint compressed_len;

	uint32 compressed_size;
	uint32 uncompressed_size;

	FrameRelativeIndex.push_back(ftell(output_fp) - LastFrameOffset);
	FrameRelativeTime.push_back(LastFrameTime);

	LastFrameOffset = ftell(output_fp);
	LastFrameTime = frame_count;

	fwrite(video_header, 1, 8, output_fp);

	fwrite(&width, 1, 2, output_fp);
	fwrite(&height, 1, 2, output_fp);

	if(height > max_height)
		max_height = height;

	if(height < min_height)
		min_height = height;

	for(int y = MDFNGameInfo->DisplayRect.y; y < MDFNGameInfo->DisplayRect.y + MDFNGameInfo->DisplayRect.h; y++)
	{
		uint16 meow_width = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.w : espec->LineWidths[y].w;
		int meow_x = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.x : espec->LineWidths[y].x;
		uint32 *fb_line = espec->pixels + y * (MDFNGameInfo->pitch >> 2) + meow_x;

		//if(!width)
		//{
		// fwrite(&meow_width, 1, 2, output_fp);
		//}
static bool WriteChunk(const EmulateSpecStruct *espec)
{
 uint32 frame_count = espec->SoundBufSize;
 const char *video_header = "FRAME\0\0";
 uint16 height = MDFNGameInfo->DisplayRect.h;
 uint16 width = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.w : 0;
 uint8 *pb_ptr = pixel_buffer;
 uint8 workmem[LZO1X_1_MEM_COMPRESS];
 lzo_uint compressed_len;
 
 uint32 compressed_size;
 uint32 uncompressed_size;

 FrameRelativeIndex.push_back(ftell(output_fp) - LastFrameOffset);
 FrameRelativeTime.push_back(LastFrameTime);

 LastFrameOffset = ftell(output_fp);
 LastFrameTime = frame_count;

 fwrite(video_header, 1, 8, output_fp);

 fwrite(&width, 1, 2, output_fp);
 fwrite(&height, 1, 2, output_fp);

 if(height > max_height)
  max_height = height;

 if(height < min_height)
  min_height = height;

 for(int y = MDFNGameInfo->DisplayRect.y; y < MDFNGameInfo->DisplayRect.y + MDFNGameInfo->DisplayRect.h; y++)
 {
  uint16 meow_width = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.w : espec->LineWidths[y].w;
  int meow_x = (espec->LineWidths[0].w == ~0) ? MDFNGameInfo->DisplayRect.x : espec->LineWidths[y].x;
  uint32 *fb_line = espec->surface->pixels + y * (MDFNGameInfo->pitch >> 2) + meow_x;

  //if(!width)
  //{
  // fwrite(&meow_width, 1, 2, output_fp);
  //}

  if(meow_width > max_width)
   max_width = meow_width;

  if(meow_width < min_width)
   min_width = meow_width;

  for(int x = 0; x < meow_width; x++)
  {
   uint32 pixel = fb_line[x];
   int r, g, b;

   espec->surface->DecodeColor(pixel, r, g, b);

   *pb_ptr++ = r;
   *pb_ptr++ = g;
   *pb_ptr++ = b;
  }
 }

 uncompressed_size = pb_ptr - pixel_buffer;
 lzo1x_1_compress(pixel_buffer, uncompressed_size, compress_buffer, &compressed_len, workmem);
 compressed_size = compressed_len;

 fwrite(&compressed_size, 1, 4, output_fp);
 fwrite(&uncompressed_size, 1, 4, output_fp);
 fwrite(&frame_count, 1, 4, output_fp);

 if(!width)
 {
  for(int y = MDFNGameInfo->DisplayRect.y; y < MDFNGameInfo->DisplayRect.y + MDFNGameInfo->DisplayRect.h; y++)
  {
   uint16 meow_width = espec->LineWidths[y].w;
   fwrite(&meow_width, 1, 2, output_fp);
  }
 }

 fwrite(compress_buffer, 1, compressed_len, output_fp);


 fwrite(espec->SoundBuf, 2 * MDFNGameInfo->soundchan, frame_count, output_fp);

 if(max_aframes_per_vframe < frame_count)
  max_aframes_per_vframe = frame_count;

 audio_frame_counter += frame_count;

 video_frame_counter++;

 return(1);
}
		if(meow_width > max_width)
			max_width = meow_width;

		if(meow_width < min_width)
			min_width = meow_width;

		for(int x = 0; x < meow_width; x++)
		{
			uint32 pixel = fb_line[x];
			int r, g, b;

			//espec->pixels->DecodeColor(pixel, r, g, b);

			DECOMP_COLOR(pixel, r, g, b);
			*pb_ptr++ = r;
			*pb_ptr++ = g;
			*pb_ptr++ = b;
		}
	}

	uncompressed_size = pb_ptr - pixel_buffer;
	lzo1x_1_compress(pixel_buffer, uncompressed_size, compress_buffer, &compressed_len, workmem);
	compressed_size = compressed_len;

	fwrite(&compressed_size, 1, 4, output_fp);
	fwrite(&uncompressed_size, 1, 4, output_fp);
	fwrite(&frame_count, 1, 4, output_fp);

	if(!width)
	{
		for(int y = MDFNGameInfo->DisplayRect.y; y < MDFNGameInfo->DisplayRect.y + MDFNGameInfo->DisplayRect.h; y++)
		{
			uint16 meow_width = espec->LineWidths[y].w;
			fwrite(&meow_width, 1, 2, output_fp);
		}
	}

	fwrite(compress_buffer, 1, compressed_len, output_fp);


	fwrite(espec->SoundBuf, 2 * MDFNGameInfo->soundchan, frame_count, output_fp);

	if(max_aframes_per_vframe < frame_count)
		max_aframes_per_vframe = frame_count;

	audio_frame_counter += frame_count;

	video_frame_counter++;

	return(1);
}
*/
void MMRecord_WriteFrame(const EmulateSpecStruct *espec)
{
	WriteChunk(espec);
}

//#endif
/*
void MDFN_en32lsb(uint8 *buf, uint32 morp)
{ 
 buf[0]=morp;
 buf[1]=morp>>8;
 buf[2]=morp>>16;
 buf[3]=morp>>24;
} 
*/


void MMRecord_End(void) {
if(output_fp)
	if(output_fp)
 {
  uint64 FrameIndexTableOffset;
  const char *fi_header = "INDEX\0\0";

  // First, write the frame index chunk
  FrameIndexTableOffset = ftell(output_fp);
  fwrite(fi_header, 1, 8, output_fp);
  
  // FIXME:  Speedup
  for(uint64_t i = 0; i < FrameRelativeIndex.size(); i++)
  {
   uint32 qbuf[2];
   MDFN_enlsb(&qbuf[0], FrameRelativeIndex[i]);
   MDFN_enlsb(&qbuf[1], FrameRelativeTime[i]);
   fwrite(qbuf, 1, 4 * 2, output_fp);
  }

  // Finish our main header.
  MDFN_enlsb(&main_header.min_width, min_width);
  MDFN_enlsb(&main_header.max_width, max_width);
  MDFN_enlsb(&main_header.min_height, min_height);
  MDFN_enlsb(&main_header.max_height, max_height);

  MDFN_enlsb(&main_header.video_frame_counter, video_frame_counter);
  MDFN_enlsb(&main_header.audio_frame_counter, audio_frame_counter);
  MDFN_enlsb(&main_header.frame_index_offset, FrameIndexTableOffset);

  MDFN_enlsb(&main_header.max_aframes_per_vframe, max_aframes_per_vframe);

  // Now seek back and rewrite the completed header.
  fseek(output_fp, 0, SEEK_SET);
  fwrite(&main_header, 1, sizeof(MM_Main_Header), output_fp);

  fclose(output_fp);
  output_fp = NULL;
  LastFrameOffset = 0;
  LastFrameTime = 0;

  FrameRelativeIndex.clear();
  FrameRelativeTime.clear();
 // return(1);
 }
}

