sexyal_SOURCES	=      sexyal/sexyal.c sexyal/convert.c

if HAVE_DIRECTSOUND
TMP_DIRECTSOUND	=	sexyal/drivers/dsound.c
endif

if HAVE_OSSDSP
TMP_OSSDSP	=	sexyal/drivers/oss.c
endif

if HAVE_JACK
TMP_JACK	=	sexyal/drivers/jack.c
endif

if HAVE_SDL
TMP_SDL		=	sexyal/drivers/sdl.c
endif

if HAVE_ALSA
TMP_ALSA	=	sexyal/drivers/alsa.c
endif

sexyal_SOURCES	+=	$(TMP_DIRECTSOUND) $(TMP_OSSDSP) $(TMP_JACK) $(TMP_SDL) $(TMP_ALSA)
