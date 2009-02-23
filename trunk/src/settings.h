#ifndef MDFN_SETTINGS_H
#define MDFN_SETTINGS_H

#ifdef WIN32
typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

#define int64 __int64
typedef unsigned __int64  uint64 ;
#endif

#include <string>

#include "git.h"
#include "settings-common.h"

bool MFDN_LoadSettings(const char *);
bool MDFN_MergeSettings(const MDFNGI *);
bool MDFN_MergeSettings(const MDFNSetting *);
bool MDFN_MergeSettings(const std::vector<MDFNSetting> &);
bool MDFN_SaveSettings(void);

// This should assert() or something if the setting isn't found, since it would
// be a totally tubular error!
uint64 MDFN_GetSettingUI(const char *name);
int64 MDFN_GetSettingI(const char *name);
double MDFN_GetSettingF(const char *name);
bool MDFN_GetSettingB(const char *name);
std::string MDFN_GetSettingS(const char *name);
#endif
