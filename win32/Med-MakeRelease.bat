upx ..\output\mednafen.exe
cd ..\output
del /s mednafen-win32.zip
..\win32\zip -X -9 -r ..\output\mednafen-win32.zip mednafen.exe
..\win32\zip -X -9 -r ..\output\mednafen-win32.zip Mednafen-Front.exe