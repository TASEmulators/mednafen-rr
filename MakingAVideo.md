To produce an encode from a movie file you need to run Mednafen with specific arguments, it will generate a mmm file that will need to be transcoded.
The only way to transcode mmm file for now is through Linux, but the mmm file can be made from Windows.


## Generating MMM ##

Win32 ([revision249](https://code.google.com/p/mednafen-rr/source/detail?r=249))

Run Mednafen with the -mmm, -mmmfile and -mmmframes arguments.

exemple:

mednafen -mmm 1 -mmmfile c:\mymovie.mmm -mmmframes 5000 -play 1 -mov c:\mymovie.mc2

note: this assume you did set pce.cdbios in mednafen.cfg, if you didnt you need to specify -pce.cdbios path-to\bios.pce

When started Mednafen will prompt for a rom to open, select it then it will play movie "mymovie.mc2". You would record 5000 frames before stopping.
Do not stop the recording prematurely, it will break the recording. Mednafen will display "finished" when it reach the 5000th frames, then you can close Mednafen.

It will produce a file "mymovie.mmm". If the file is superior to 2gb it wont be exportable to Linux.


## Transcoding ##

The mmm file isn't useful yet. It needs to be transcoded with the mednafen-mmmencode program (linux only). It is in the svn; you'll have to compile it yourself.

mednafen-mmmencode mymovie.mmm

It will produce a test.mov file (quicktime) that can be encoded any way you want.

## Compiling mmmencode ##

You will need Linux; you can do this with vmware free player or microsoft virtual machines.

From a fresh ubuntu 9.04 installed & updated system you will need to get the following:
```
sudo apt-get install subversion build-essential automake autoconf libtool patchutils binutils libc6-dev bison flex gawk gettext imagemagick zlib1g-dev libsndfile-dev libquicktime-dev
```
Now get the source and compile mmmencode
```
svn checkout http://mednafen-rr.googlecode.com/svn/trunk/ mednafen-rr-read-only
cd mednafen-rr-read-only
cd mednafen-mmmencode
chmod +x autogen.sh
sudo ./autogen.sh
sudo ./configure
sudo make
sudo make install
```

Binary should be installed now.