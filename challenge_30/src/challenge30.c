#pragma warning(push, 1)
#define WINVER         0x0400
#define _WIN32_WINNT   0x0400
/* Copyright (c) 2019 KrossX <krossx@live.com>
* License: http://www.opensource.org/licenses/mit-license.html  MIT License
*/

#define _WIN32_WINDOWS 0x0400
#define _WIN32_IE      0x0400

#include <windows.h>
#include <gl/gl.h>
#pragma warning(pop)

typedef __int8  s8;
typedef __int16 s16;
typedef __int32 s32;
typedef __int64 s64;

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

enum {
	SND_FALL,
	SND_MOVE,
	SND_ROTATE,
	SND_DOWN,
	SND_HDROP,
	SND_LINE,
	SND_TETRIS,
	SND_LEVELUP
};

enum {
	BGM_TRACK1,
	BGM_TRACK2,
	BGM_TRACK3
};

void play_sound(int snd_id);
void play_music(int bgm_id);

double time_accum;
double time_diff;

u8 keyboard[256];

u32 tgrid[32][30];
#include "game.c"

u32 fbuf[30][30];
#include "game_draw.c"

#include "bass.h"

HSTREAM music[3];
HSAMPLE sndfx[8];
HCHANNEL sndch[8];

//nVIDIA / AMD Performance Hints
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

typedef BOOL wglSwapInterval_t(int interval);
wglSwapInterval_t *wglSwapInterval;

HDC   dev_ctx;
HGLRC ren_ctx;

GLfloat fb_offx, fb_offy, fb_scale;

void toggle_fullscreen(HWND wnd)
{
	static WINDOWPLACEMENT prev = {sizeof(WINDOWPLACEMENT)};
	
	DWORD style = GetWindowLong(wnd, GWL_STYLE);
	
	if(style & WS_OVERLAPPEDWINDOW)
	{
		int width  = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);
		
		if(GetWindowPlacement(wnd, &prev))
		{
			ShowCursor(FALSE);
			SetWindowLong(wnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(wnd, HWND_TOP, 0, 0, width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);				
		}
	}
	else
	{
		ShowCursor(TRUE);
		SetWindowLong(wnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(wnd, &prev);
		SetWindowPos(wnd, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}	
}

void window_resize(int width, int height)
{
	int scale_x = (int)(width / 30.0f);
	int scale_y = (int)(height / 30.0f);

	fb_scale = (GLfloat)(scale_x < scale_y ? scale_x : scale_y);
	fb_offx  = (GLfloat)((width - (int)fb_scale * 30) / 2);
	fb_offy  = (GLfloat)((height - (int)fb_scale * 30) / 2);
	
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 100);
	glMatrixMode(GL_MODELVIEW);
}

int set_pixel_format(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	int pf;
	
	pfd.nSize           = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion        = 1;
    pfd.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType      = PFD_TYPE_RGBA;
    pfd.cColorBits      = 32;
    pfd.cRedBits        = 0;
    pfd.cRedShift       = 0;
    pfd.cGreenBits      = 0;
    pfd.cGreenShift     = 0;
    pfd.cBlueBits       = 0;
    pfd.cBlueShift      = 0;
    pfd.cAlphaBits      = 0;
    pfd.cAlphaShift     = 0;
    pfd.cAccumBits      = 0;
    pfd.cAccumRedBits   = 0;
    pfd.cAccumGreenBits = 0;
    pfd.cAccumBlueBits  = 0;
    pfd.cAccumAlphaBits = 0;
    pfd.cDepthBits      = 0;
    pfd.cStencilBits    = 0;
    pfd.cAuxBuffers     = 0;
    pfd.iLayerType      = PFD_MAIN_PLANE;
    pfd.bReserved       = 0;
    pfd.dwLayerMask     = 0;
    pfd.dwVisibleMask   = 0;
    pfd.dwDamageMask    = 0;
	
	pf = ChoosePixelFormat(dev_ctx, &pfd);
	if(!pf) return 0;
	
	return SetPixelFormat(dev_ctx, pf, &pfd);
}

LRESULT CALLBACK wndproc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_CREATE:
			dev_ctx = GetDC(wnd);
			
			if(!set_pixel_format())
			{
				MessageBox(NULL, "Could not set pixel format", "Error", MB_OK);
				return -1;
			}
			
			ren_ctx = wglCreateContext(dev_ctx);
			
			if(!ren_ctx)
			{
				MessageBox(NULL, "Could not create GL render context.", "Error", MB_OK);
				return -1;
			}

			wglMakeCurrent(dev_ctx, ren_ctx);
			ShowWindow(wnd, SW_SHOW);
			return 0;
			
		case WM_LBUTTONDBLCLK:
			toggle_fullscreen(wnd);
			return 0;

		case WM_SYSCHAR:
			//ding ding ding
			return 0;
			
		case WM_SYSKEYDOWN:
			keyboard[wparam] = 1;
			if(wparam == VK_RETURN && (lparam & (1<<29)))
				toggle_fullscreen(wnd);
			return 0;
			
		case WM_SYSKEYUP:
			keyboard[wparam] = 0;
			return 0;
		
		case WM_KEYDOWN:
			keyboard[wparam] = 1;
			if(wparam == VK_ESCAPE)
				PostMessage(wnd, WM_CLOSE, 0, 0);
			return 0;
			
		case WM_KEYUP:
			keyboard[wparam] = 0;
			return 0;
			
		case WM_SIZE:
			window_resize(LOWORD(lparam), HIWORD(lparam));
			return 0;
		
		case WM_CLOSE:
			wglMakeCurrent(NULL, NULL);
			
			if(ren_ctx) wglDeleteContext(ren_ctx);
			if(dev_ctx) ReleaseDC(wnd, dev_ctx);
			
			ren_ctx = NULL;
			dev_ctx = NULL;
		
			DestroyWindow(wnd);
			return 0;
			
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	
	return DefWindowProc(wnd, msg, wparam, lparam);
}

HWND create_window(HINSTANCE inst, int width, int height)
{
	RECT wnd_size;
	int wnd_width, wnd_height;
	
	char wnd_class[] = "CHALLENGE30";
	char wnd_title[] = "Challenge #30: 30x30";
	
	WNDCLASS wc;
	
	wc.style         = CS_DBLCLKS;
    wc.lpfnWndProc   = wndproc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = inst;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = wnd_class;
	
	RegisterClass(&wc);
	
	wnd_size.left   = 0;
	wnd_size.top    = 0;
	wnd_size.right  = width;
	wnd_size.bottom = height;
	
	AdjustWindowRect(&wnd_size, WS_OVERLAPPEDWINDOW, FALSE);
	
	wnd_width  = wnd_size.right - wnd_size.left;
	wnd_height = wnd_size.bottom - wnd_size.top;

	return CreateWindow(wnd_class, wnd_title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, wnd_width, wnd_height,
		NULL, NULL, inst, NULL);
}

void play_sound(int snd_id)
{
	BASS_ChannelPlay(sndch[snd_id], TRUE);
}

void play_music(int bgm_id)
{
	static int prev_id = 0;
	BASS_ChannelStop(music[prev_id]);
	BASS_ChannelPlay(music[bgm_id], TRUE);
	prev_id = bgm_id;
}

void load_sound(int snd_id, char *filename)
{
	sndfx[snd_id] = BASS_SampleLoad(FALSE, filename, 0, 0, 1, 0);
	sndch[snd_id] = BASS_SampleGetChannel(sndfx[snd_id], FALSE);
}

void load_music(int bgm_id, char *filename)
{
	music[bgm_id] = BASS_StreamCreateFile(FALSE, filename, 0, 0, 0);
	BASS_ChannelFlags(music[bgm_id], BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, char *cmdline, int cmdshow)
{
	s64 perf_freq, perf_count, perf_old;
	GLuint fbtex;
	MSG msg;

	HWND wnd = create_window(inst, 480, 480);
	if(!wnd) return -1;
	
	wglSwapInterval = (wglSwapInterval_t*)wglGetProcAddress("wglSwapIntervalEXT");
	if(!wglSwapInterval) return -2;
	
	wglSwapInterval(1);
	
	if (HIWORD(BASS_GetVersion())!=BASSVERSION || 
		!BASS_Init(-1, 44100, BASS_DEVICE_MONO, wnd, NULL))
		return -3;
		
	//BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC,  9500);
	BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, 3500);
	BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 5500);
		
	load_sound(SND_FALL, "snd/snd_fall.wav");
	load_sound(SND_MOVE, "snd/snd_move.wav");
	load_sound(SND_ROTATE, "snd/snd_rotate.wav");
	load_sound(SND_DOWN, "snd/snd_down.wav");
	load_sound(SND_HDROP, "snd/snd_hdrop.wav");
	load_sound(SND_LINE, "snd/snd_line.wav");
	load_sound(SND_TETRIS, "snd/snd_tetris.wav");
	load_sound(SND_LEVELUP, "snd/snd_levelup.wav");
	
	load_music(BGM_TRACK1, "snd/bgm1.m4a");
	load_music(BGM_TRACK2, "snd/bgm2.m4a");
	load_music(BGM_TRACK3, "snd/bgm3.m4a");
	
	play_music(BGM_TRACK1);
	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &fbtex);
	glBindTexture(GL_TEXTURE_2D, fbtex);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 30, 30, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&perf_freq);
	
	while(1)
	{	
		glClear(GL_COLOR_BUFFER_BIT);

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if(msg.message == WM_QUIT)
			break;
		
		QueryPerformanceCounter((LARGE_INTEGER*)&perf_count);
		time_diff = (double)(perf_count - perf_old) / (double)perf_freq;
		if(time_diff > 0.1) time_diff = 0.1;
		perf_old = perf_count;
		
		game_input();
		game_step();
		game_draw();
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 30, 30, GL_RGBA, GL_UNSIGNED_BYTE, fbuf);
		
		glPushMatrix();
		glTranslatef(fb_offx, fb_offy, 0.0f);
		glScalef(fb_scale, fb_scale, 1.0f);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f, 0.0f); glVertex2f( 0.0f,  0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(30.0f,  0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f( 0.0f, 30.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(30.0f, 30.0f);
		glEnd();
		glPopMatrix();

		SwapBuffers(dev_ctx);
		
		if(user_level >= 30) {
			MessageBox(NULL, "You win!", "Challenge #30", MB_OK);
			break;
		}
	}
	
	BASS_Free();

	return ERROR_SUCCESS;
}
