## Command Line Options ##

Playing bonk3.pce while designating the movie file to record to and play back from:

`mednafen -mov C:\mednafen\mymovie.mov C:\mednafen\bonk3.pce`

You need to use full paths in Windows.  Do not use relative paths.  If you do not designate a movie file Mednafen will default to a movie with the extension 666 in your movie folder.  It will overwrite this movie if you record to it again.

Example of running a PCE CD game "Aku," with the PCE bios "3.pce" with a 1:1 window scaling:

`mednafen -pce.xscale 1 -pce.yscale 1  -pce.cdbios ~/Desktop/3.pce ~/Desktop/Aku/Aku.cue`

Other useful things:
```
 Argument     Type      Default    Function
-lagadvance x boolean   0          Automatically advance lag frames if 1. Stored in config.
-allowlrud x  boolean   0          Enable/Disable button exclusion. L/R and U/D allowed if 1. Stored in config.
-readonly x   boolean   1          Start readonly if 1, read+write if 0. Stored in config.
-loadstate "" string    ""         Load the specified state on start. Not stored in config.
-pause x      boolean   0          Start the emulator paused if set to 1. Stored in config.
-mov ""       string    ""         Designate a movie to play or record to.  Not stored in config.
-play x       boolean   0          Play the movie designated by -mov if 1. Not stored in config.
-author ""    string    ""         The author's name to be written to the movie file, no spaces allowed.  Stored in config.

-ffspeed x    integer   2          Specifies the speed multiplier to be used in fast-forward mode.
-fftoggle x   boolean   0          Treat the fast-forward button as a toggle.
-ffnosound x  boolean   0          Silence sound output when fast-forwarding.
-mmm x        boolean   0          Record an mmm multimedia video if 1. See the MakingAVideo wiki page.
-mmmfile x  string   ""            The path and name to the mmm file that will be created.
-mmmframes x  integer   0          Record x number of frames in the mmm.
```

More command line arguments: http://mednafen.sourceforge.net/documentation/#using-cli