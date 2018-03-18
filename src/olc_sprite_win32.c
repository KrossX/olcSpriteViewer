/* Copyright (c) 2018 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 *
 * SPR file format by Javidx9, Copyright (c) 2018 Javidx9
 * License: https://www.gnu.org/licenses/gpl-3.0.en.html GNU GPLv3
 * https://github.com/OneLoneCoder/videos
 */

#ifdef UNICODE
#error Disable UNICODE!
#endif

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "shell32.lib")

#pragma warning(push, 1)
#define WINVER         0x0400
#define _WIN32_WINNT   0x0400
#define _WIN32_WINDOWS 0x0400
#define _WIN32_IE      0x0400

#include <windows.h>
#include <gl/gl.h>
#pragma warning(pop)

#define virtual_alloc(size) VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#define virtual_free(addr)  VirtualFree(addr, 0, MEM_RELEASE);

typedef unsigned __int8   u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

typedef __int8    s8;
typedef __int16  s16;
typedef __int32  s32;
typedef __int64  s64;

typedef BOOL  (WINAPI wglSwapInterval_t) (int interval);
wglSwapInterval_t *wglSwapInterval;

//==============================================================================

int keyboard[256];
int mouse_x, mouse_y;
int mouse_dx, mouse_dy;

int wnd_width;
int wnd_height;

int edit_mode;

float dt_ms;
float offx, offy, scale = 8.0f;
float prevx, prevy;

GLuint font_texture, sprite_texture;

#include "font_8x8.c"
#include "font_8x8_256x256.c"
#include "olc_sprite.c"

struct olc_sprite main_sprite;
struct olc_pixel edit_pixel;

char **file_list;
size_t file_list_count;
size_t file_list_index;

HDC   dev_ctx;
HGLRC ren_ctx;

LARGE_INTEGER perf_freq, perf_count, perf_old;

//==============================================================================

void cheap_fullpath(char *dst, char *directory, char *filename)
{
	while (*directory)
	{
		*dst++ = *directory++;
	}

	while (*filename)
	{
		*dst++ = *filename++;
	}

	*dst = 0;
}

void cheap_strcpy(char *dst, char *src)
{
	while (*src)
	{
		*dst++ = *src++;
	}
}

int cheap_strcmp(char *str1, char *str2)
{
	int value = 0;

	while (*str1 && *str2 && value == 0)
	{
		value = *str1++ - *str2++;
	}

	if (!value)
		value = *str1 - *str2;

	return value;
}

char* cheap_last_of(char *str, char c)
{
	char *last = NULL;

	while(*str)
	{
		if (*str == c) last = str;
		str++;
	}

	return last;
}

size_t cheap_strlen(char *str)
{
	size_t len = 0;
	while (*str++)
		len++;
	return len;
}

//==============================================================================

void update_sprite_texture(void)
{
	glBindTexture(GL_TEXTURE_2D, sprite_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, main_sprite.width * 8, main_sprite.height * 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, main_sprite.tex);

	//generate mipmap?
}

void update_title(HWND wnd)
{
	char buffer[128];
	char *filename = cheap_last_of(file_list[file_list_index], '\\');
	
	filename = filename? filename + 1 : file_list[file_list_index];
	
	wsprintf(buffer, "olcSprite Viewer - %s", filename);
	SetWindowText(wnd, buffer);
}

void change_preview(float x, float y)
{
	if(main_sprite.width < 16 || main_sprite.height < 16)
		return;

	prevx += x * 16.0f;
	prevy += y * 16.0f;
	
	if((prevx + 16.0f) > main_sprite.width) prevx = 0;
	else if(prevx < 0) prevx = ((main_sprite.width-1)/16) * 16.0f;

	if((prevy + 16.0f) > main_sprite.height) prevy = 0;
	else if(prevy < 0) prevy = ((main_sprite.height-1)/16) * 16.0f;
}

void zoom_in(float pointx, float pointy)
{
	float distx = (pointx - offx) / (main_sprite.width * scale);
	float disty = (pointy - offy) / (main_sprite.height * scale);
	
	scale *= 1.1f;
	
	offx = pointx - (distx * main_sprite.width * scale);
	offy = pointy - (disty * main_sprite.height * scale);
}

void zoom_out(float pointx, float pointy)
{
	float distx = (pointx - offx) / (main_sprite.width * scale);
	float disty = (pointy - offy) / (main_sprite.height * scale);
	
	scale /= 1.1f;
	
	offx = pointx - (distx * main_sprite.width * scale);
	offy = pointy - (disty * main_sprite.height * scale);
}

void reset_zoom_pos(void)
{
	offx = 0;
	offy = 0;
	scale = 8.0f;
	
	prevx = 0;
	prevy = 0;
}

void next_file(HWND wnd)
{
	if(!file_list_count)
		return;
	
	if(!save_check_msg(&main_sprite, file_list[file_list_index]))
		return;
	
	file_list_index++;
	if(file_list_index >= file_list_count) file_list_index = 0;
	
	load_sprite(&main_sprite, file_list[file_list_index]);
	update_sprite_texture();
	update_title(wnd);
	reset_zoom_pos();
}

void prev_file(HWND wnd)
{
	if(!file_list_count) return;
	
	if(!save_check_msg(&main_sprite, file_list[file_list_index]))
		return;
	
	if(file_list_index == 0) file_list_index = file_list_count - 1;
	else file_list_index--;
	
	load_sprite(&main_sprite, file_list[file_list_index]);
	update_sprite_texture();
	update_title(wnd);
	reset_zoom_pos();
}

void file_list_find(char *filename)
{
	size_t i;
	
	for (i = 0; i < file_list_count; i++)
	{
		if (cheap_strcmp(file_list[i], filename) == 0)
		{
			file_list_index = i;
			break;
		}
	}
}

void load_directory(char *filename)
{
	static char last_dir[MAX_PATH+1];
	
	char  filename_backup[2];
	char *filename_path;
	
	WIN32_FIND_DATAA find_data;
	HANDLE find;
	
	if(GetFileAttributesA(filename) & FILE_ATTRIBUTE_DIRECTORY)
	{
		size_t pos = cheap_strlen(filename);
		filename[pos] = '\\';
		filename[pos + 1] = 0;
	}
	
	filename_path = cheap_last_of(filename, '\\') + 1;
	
	filename_backup[0] = filename_path[0];
	filename_backup[1] = filename_path[1];

	filename_path[0] = 0;
	filename_path[1] = 0;
	
	if (cheap_strcmp(filename, last_dir) == 0)
	{
		filename_path[0] = filename_backup[0];
		filename_path[1] = filename_backup[1];
		return;
	}
	
	ZeroMemory(last_dir, MAX_PATH);
	cheap_strcpy(last_dir, filename);
	
	filename_path[0] = '*';
	
	find = FindFirstFileA(filename, &find_data);
	
	if (find != INVALID_HANDLE_VALUE)
	{
		size_t buffer_count = 0;
		
		if (file_list_count)
		{
			size_t i;
			
			for (i = 0; i < file_list_count; i++)
			{
				virtual_free(file_list[i]);
				file_list[i] = NULL;
			}

			virtual_free(file_list);
			file_list = NULL;
		}

		file_list_count = 0;
		file_list_index = 0;
		
		while (FindNextFileA(find, &find_data))
		{
			char *ext;
			
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			
			ext = cheap_last_of(find_data.cFileName, '.');
			
			if(!ext || *ext != '.') continue; ext++;
			if(!ext || !(*ext == 's' || *ext == 'S')) continue; ext++;
			if(!ext || !(*ext == 'p' || *ext == 'P')) continue; ext++;
			if(!ext || !(*ext == 'r' || *ext == 'R')) continue;
			
			if (file_list_count >= buffer_count)
			{
				size_t i = 0;
				char **new_list;
				
				buffer_count = buffer_count ? buffer_count << 1 : 1;

				new_list = virtual_alloc(sizeof(void*) * buffer_count);

				for (; i < file_list_count; i++)
				{
					new_list[i] = file_list[i];
				}

				for (; i < buffer_count; i++)
				{
					new_list[i] = NULL;
				}

				virtual_free(file_list);
				file_list = new_list;
			}
			
			file_list[file_list_count] = virtual_alloc(MAX_PATH+1);
			
			cheap_fullpath(file_list[file_list_count], last_dir, find_data.cFileName);

			file_list_count++;	
		}
		
		FindClose(find);
	}
	
	filename_path[0] = filename_backup[0];
	filename_path[1] = filename_backup[1];
}

void update_time(void)
{
	perf_old = perf_count;
	QueryPerformanceCounter(&perf_count);
	dt_ms = (u32)(((perf_count.QuadPart - perf_old.QuadPart) * 1000000) / perf_freq.QuadPart) / 1000.0f;
}

void load_font_texture(void)
{
	glGenTextures(1, &font_texture);
	glBindTexture(GL_TEXTURE_2D, font_texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, font_8x8_256x256);
}

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
			SetWindowLong(wnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(wnd, HWND_TOP, 0, 0, width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else
	{
		SetWindowLong(wnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(wnd, &prev);
		SetWindowPos(wnd, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}	
}

void window_resize(int width, int height)
{
	wnd_width = width;
	wnd_height = height;
	
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 100);
	glMatrixMode(GL_MODELVIEW);
}

int set_pixel_format(void)
{
	PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	int pf = ChoosePixelFormat(dev_ctx, &pfd);
	if(!pf) return 0;
	
	return SetPixelFormat(dev_ctx, pf, &pfd);
}

LRESULT CALLBACK wndproc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_CREATE:
			dev_ctx = GetDC(wnd);
			if(!set_pixel_format()) return -1;
	
			ren_ctx = wglCreateContext(dev_ctx);
			if(!ren_ctx) return -1;
			
			wglMakeCurrent(dev_ctx, ren_ctx);	
			return 0;
			
		case WM_SYSCHAR:
			//ding ding ding
			return 0;
			
		case WM_SYSKEYDOWN:
			if(wparam == VK_RETURN && (lparam & (1<<29)))
				toggle_fullscreen(wnd);
			return 0;
			
		case WM_KEYDOWN:
			switch(wparam)
			{
				case VK_ESCAPE: PostMessage(wnd, WM_CLOSE, 0, 0); break;
				case VK_PRIOR: prev_file(wnd); break;
				case VK_NEXT: next_file(wnd); break;
				case VK_ADD: zoom_in(main_sprite.width * scale * 0.5f + offx, main_sprite.height * scale * 0.5f + offy); break;
				case VK_SUBTRACT: zoom_out(main_sprite.width * scale * 0.5f + offx, main_sprite.height * scale * 0.5f + offy); break;
				case VK_MULTIPLY: reset_zoom_pos(); break;
				case VK_LEFT: change_preview(-1.0f, 0.0f); break;
				case VK_RIGHT: change_preview( 1.0f, 0.0f); break;
				case VK_UP: change_preview(0.0f, -1.0f); break;
				case VK_DOWN: change_preview(0.0f, 1.0f); break;
				case VK_TAB: edit_mode ^= 1; break;
				case 'S': if(keyboard[VK_CONTROL]) save_sprite(&main_sprite, file_list[file_list_index]);  break;
			}
			
			keyboard[wparam] = 1;
			return 0;	
			
		case WM_KEYUP:
			keyboard[wparam] = 0;
			return 0;
			
		case WM_LBUTTONDOWN:
			if(edit_mode && !keyboard[VK_SPACE])
			{
				if(wparam & MK_CONTROL)
				{
					get_pixel(&main_sprite, &edit_pixel);
				}
				else
				{
					set_pixel(&main_sprite, &edit_pixel);
					update_sprite_texture();
				}
			}
			return 0;
			
		case WM_MOUSEWHEEL:
			if((short)HIWORD(wparam) > 0) zoom_in((float)mouse_x, (float)mouse_y);
			if((short)HIWORD(wparam) < 0) zoom_out((float)mouse_x, (float)mouse_y);
			return 0;
			
		case WM_MOUSEMOVE:
			mouse_dx = LOWORD(lparam) - mouse_x;
			mouse_dy = HIWORD(lparam) - mouse_y;
			mouse_x  = LOWORD(lparam);
			mouse_y  = HIWORD(lparam);
			
			if(edit_mode)
			{
				if((wparam & MK_LBUTTON) && keyboard[VK_SPACE] || (wparam & MK_MBUTTON))
				{
					offx += mouse_dx;
					offy += mouse_dy;
				}
			}
			else if(wparam & MK_LBUTTON)
			{
				offx += mouse_dx;
				offy += mouse_dy;
			}

			return 0;

		case WM_SIZE:
			window_resize(LOWORD(lparam), HIWORD(lparam));
			return 0;
			
		case WM_DROPFILES:
			{
				HDROP drop = (HDROP)wparam;
				char filename[MAX_PATH + 1];

				DragQueryFileA(drop, 0, filename, MAX_PATH);
				DragFinish(drop);
				
				if(file_list_count)
				{
					if(!save_check_msg(&main_sprite, file_list[file_list_index]))
						return 0;
				}

				load_directory(filename);

				if (file_list_count)
				{
					file_list_find(filename);
					load_sprite(&main_sprite, file_list[file_list_index]);
					reset_zoom_pos();
					update_sprite_texture();
					update_title(wnd);
				}
			}
			return 0;
			
		case WM_CLOSE:
			if(!file_list_count || save_check_msg(&main_sprite, file_list[file_list_index]))
				DestroyWindow(wnd);
			return 0;
			
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	
	return DefWindowProc(wnd, msg, wparam, lparam);
}

HWND create_window(HINSTANCE inst)
{
	char wnd_title[] = "olcSprite Viewer";
	char wnd_class[] = "OLC_SPRITE_CLASS";
	
	WNDCLASS wc =
	{
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		wndproc,
		0,
		0,
		inst, 
		LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW),
		NULL,
		NULL,
		wnd_class
	};
	
	RegisterClass(&wc);
	
	return CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES, wnd_class, wnd_title,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, inst, NULL);
}

#define GL_GENERATE_MIPMAP                0x8191
#define GL_GENERATE_MIPMAP_HINT           0x8192

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, char *cmdline, int cmdshow)
{
	HWND wnd;
	MSG msg;
	int not_quit = 1;

	if(cmdline[0])
	{
		load_directory(cmdline);
		
		if(file_list_count)
		{
			file_list_find(cmdline);
			load_sprite(&main_sprite, file_list[file_list_index]);
		}
	}
	else
	{
		char buffer[MAX_PATH];
		
		if(GetCurrentDirectory(MAX_PATH, buffer))
			load_directory(buffer);
		
		if(file_list_count)
			load_sprite(&main_sprite, file_list[file_list_index]);
	}
	
	if(!QueryPerformanceFrequency(&perf_freq))
		return -1;
	
	wnd = create_window(inst);
	if(!wnd) return -1;
	
	if(main_sprite.loaded)
		update_title(wnd);
	
	wglSwapInterval = (wglSwapInterval_t*)wglGetProcAddress("wglSwapIntervalEXT");
	if (!wglSwapInterval) return -1;
	
	wglSwapInterval(1);
	
	load_font_texture();
	update_time();
	
	// sprite texture
	glGenTextures(1, &sprite_texture);
	glBindTexture(GL_TEXTURE_2D, sprite_texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, TRUE);
	
	if(main_sprite.loaded)
		update_sprite_texture();
	
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glAlphaFunc(GL_GREATER, 0.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	while(not_quit)
	{
		update_time();
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
			not_quit = msg.message != WM_QUIT;
		}
		
		if(main_sprite.loaded)
		{
			draw_sprite(&main_sprite);
			draw_segment(&main_sprite);
			draw_infobar(&main_sprite);
		}
		
		if(edit_mode)
		{
			draw_editor(&edit_pixel);
		}
		
		SwapBuffers(dev_ctx);
	}

	
	return ERROR_SUCCESS;
}