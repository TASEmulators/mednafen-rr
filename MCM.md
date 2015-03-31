#Specification for the .mcm movie file format

# MCM #

mcm is the movie capture format of Mednafen, a TurboGrafx 16 (PC Engine), PC-FX, Neo Geo Pocket (Color), WonderSwan (Color),and Atari Lynx emulator.  In addition it also supports NES,SMS,GG,GB,GBA.

The following are only available on the Linux version:
PC-FX, GB, GBA, Sega MS, Game Gear

## Savestate/Movie Info ##
### Savestate ###
Re-recording is bulletproof since the movie data is loaded from the appropriate file when a state is loaded. This prevents ruining a movie by loading states out of order while recording.

Savestates made from movies are seperate and do not contain the movies within them. For example, the savestate "filename.nc0" made from the movie file "filename.10.mcm" is seperate from "filename.10.mcm" and don't contain the input data from "filename.10.mcm".

Movie files have their associated savestates separate from "non-movie" savestates, so those savestates cannot be accidentally overwritten while no movie is being replayed.

TODO: More details

### Movie File Format ###
```
     000 8-byte    "MDFNMOVI" signature
     008 uint32     Mednafen Version (Current is 0A 08)
     00C uint32     Movie Format Version (Current is 01)
     010 32-byte    MD5 of the ROM used
     030 64-byte    Filename of the ROM used (with extension)
     070 uint32     Re-record Count
     074 5-byte     Console indicator (pce, ngp, pcfx, wswan)
     079 32-byte    Author name
     099 103-byte   Padding 0s
     100 variable   Input data
```
TODO: Verify if NTSC/PAL mode used for the movie can be detected or not.

### Input Data Formats ###
The input stream consists of 1 byte for power-on and reset, and then X bytes per each input port per frame.

|Console Type|Input Ports|Bytes per Port|Frame Size (with Control Byte)|Buttons (Ordered)|
|:-----------|:----------|:-------------|:-----------------------------|:----------------|
|Lynx|1 |2 |3 |  |
|WonderSwan|1 |2 |3 |  |
|NG Pocket|1 |1 |2 |  |
|PC Engine|5 |2 |11|1, 2, Select, Run, Up, Right, Down, Left|
|PC-FX|2 |2 |5 |  |
|SMS|2 |1 |3 |  |
|NES|4 |1 |? |A, B, Select, Start, Up, "Down, Left, Right|

Normally, NES receives from 5 input ports, where the first 4 have a length of 1 byte, and the last has a length of 0. For the sake of simplicity, it is interpreted as 4 ports of 1 byte length for re-recording.