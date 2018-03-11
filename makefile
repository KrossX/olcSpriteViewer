#NMAKE

CC_FLAGS = /c /Ox /Os /GR- /MD /nologo /W3
LINKER_FLAGS = /SUBSYSTEM:WINDOWS /NOLOGO

all:
	@cl $(CC_FLAGS) src/olc_sprite_win32.c -Foolc_sprite.obj
	@link olc_sprite.obj user32.lib gdi32.lib opengl32.lib shell32.lib $(LINKER_FLAGS)
	@del *.obj

