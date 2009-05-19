AUTOMAKE_OPTIONS = subdir-objects
DEFS = -DLOCALEDIR=\"$(datadir)/locale\" @DEFS@
DEFAULT_INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/intl

bin_PROGRAMS    =       mednafen-mmmencode
mednafen_mmmencode_SOURCES        =       main.cpp mmmreader.cpp minilzo.c resize.cpp selblur.cpp video.cpp
mednafen_mmmencode_LDADD = @LIBINTL@ @LIBICONV@ -lm -lz
#mednafen-mmmencode_DEPENDENCIES   =       trio/libtrio.a

