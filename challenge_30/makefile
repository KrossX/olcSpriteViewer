#NMAKE

CFLAGS = /Ox /Os /GR- /MD /nologo /W3
LINKFLAGS = /SUBSYSTEM:WINDOWS /NOLOGO
LINKLIBS = bass.lib user32.lib gdi32.lib opengl32.lib

all: 
	@cl $(CFLAGS) src/challenge30.c $(LINKLIBS) /link $(LINKFLAGS)
	@del challenge30.exp
	@del challenge30.lib
	@del challenge30.obj
