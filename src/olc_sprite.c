/* Copyright (c) 2018 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 *
 * SPR file format by Javidx9, Copyright (c) 2018 Javidx9
 * License: https://www.gnu.org/licenses/gpl-3.0.en.html GNU GPLv3
 * https://github.com/OneLoneCoder/videos
 */

u32 colour_palette[16] = // 0xAABBGGRR
{
	0xFF000000, // BLACK
	0xFF800000, // DARK_BLUE
	0xFF008000, // DARK_GREEN
	0xFF808000, // DARK_CYAN
	0xFF000080, // DARK_RED
	0xFF800080, // DARK_MAGENTA
	0xFF008080, // DARK_YELLOW
	0xFFC0C0C0, // GREY
	0xFF808080, // DARK_GREY
	0xFFFF0000, // BLUE
	0xFF00FF00, // GREEN
	0xFFFFFF00, // CYAN
	0xFF0000FF, // RED
	0xFFFF00FF, // MAGENTA
	0xFF00FFFF, // YELLOW
	0xFFFFFFFF  // WHITE
};

struct olc_sprite
{
	int width;
	int height;
	WORD *colour;
	WCHAR *glyph;
	void *tex;
	
	BOOL loaded;
};

void gen_sprite_texture(struct olc_sprite *sprite)
{
	int x, y, px, py;
	
	u32 *tex = sprite->tex;
	
	for(y = 0; y < sprite->height; y++)
	for(x = 0; x < sprite->width; x++)
	{
		int pos = y * sprite->width + x;
		
		int id = sprite->glyph[pos];
		u32 COL = sprite->colour[pos];
		u32 FG = colour_palette[COL & 0xF];
		u32 BG = colour_palette[(COL>>4) & 0xF];
		
		int u, v;
		set_char_pos(id, &u, &v);
		
		for(py = 0; py < 8; py++)
		for(px = 0; px < 8; px++)
		{
			int ppos = (y * 8 + py) * (sprite->width * 8) + (x * 8 + px);
			int tpos = (v + py) * 256 + (u + px);
			
			tex[ppos] = id == ' ' ? 0 : font_8x8_256x256[tpos] ? FG : BG;
		}
	}
}

void clear_sprite_data(struct olc_sprite *sprite)
{
	if(sprite->colour) virtual_free(sprite->colour);
	if(sprite->glyph)  virtual_free(sprite->glyph);
	if(sprite->tex)    virtual_free(sprite->tex);
	
	sprite->width  = 0;
	sprite->height = 0;
	sprite->colour = NULL;
	sprite->glyph  = NULL;
	sprite->tex    = NULL;
}

int load_sprite(struct olc_sprite *sprite, char *filename)
{
	HANDLE file = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(sprite->loaded)
		clear_sprite_data(sprite);

	sprite->loaded = FALSE;
	
	if(file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	else
	{
		DWORD filesize = GetFileSize(file, NULL);
		
		if(filesize > 8)
		{
			DWORD bytes_read;
			int width  = 0;
			int height = 0;
			
			ReadFile(file, &width, 4, &bytes_read, NULL);
			ReadFile(file, &height, 4, &bytes_read, NULL);
			
			if(width > 0 && height > 0)
			{
				DWORD section_bytes = width * height * 2;

				sprite->width  = width;
				sprite->height = height;
				sprite->colour = virtual_alloc(section_bytes);
				sprite->glyph  = virtual_alloc(section_bytes); 
				sprite->tex    = virtual_alloc(section_bytes * 2 * 8 * 8);

				if(sprite->colour && sprite->glyph && sprite->tex)
				{
					ReadFile(file, sprite->colour, section_bytes, &bytes_read, NULL);
					ReadFile(file, sprite->glyph,  section_bytes, &bytes_read, NULL);
					gen_sprite_texture(sprite);
					sprite->loaded = TRUE;
				}
				else
				{
					MessageBoxA(NULL, "Could not allocate memory.", "Error!", MB_OK);
					clear_sprite_data(sprite);
				}
			}
		}
		
		CloseHandle(file);
		return sprite->loaded ? 1 : 0;
	}
}

void draw_text(float x, float y, char *text)
{
	float x1 = x;
	float y1 = y - 8.0f;
	
	float x2 = x + 8.0f;
	float y2 = y;
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	
	while(*text)
	{
		int u, v;
		float u1, v1, u2, v2;
		
		set_char_pos(*text, &u, &v);
		
		u1 = u / 256.0f;
		v1 = v / 256.0f;
		u2 = (u + 8) / 256.0f;
		v2 = (v + 8) / 256.0f;
		
		glBegin(GL_TRIANGLE_STRIP);
			glColor3f(1, 1, 1);
			glTexCoord2f(u1, v1); glVertex2f(x1, y1);
			glTexCoord2f(u2, v1); glVertex2f(x2, y1);
			glTexCoord2f(u1, v2); glVertex2f(x1, y2);
			glTexCoord2f(u2, v2); glVertex2f(x2, y2);
		glEnd();
		
		x1 += 8.0f;
		x2 += 8.0f;
		
		text++;
	}
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
}

void draw_infobar(struct olc_sprite *sprite)
{
	char text[32];
	int charx, chary;
	float x1, y1, x2, y2;
	
	x1 = 0.0f;
	y1 = (float)(wnd_height - 20);
	x2 = (float)(wnd_width);
	y2 = (float)(wnd_height);
	
	glEnable(GL_BLEND);
	glBegin(GL_TRIANGLE_STRIP);
		glColor4f(0, 0, 0, 0.8f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x1, y2);
		glVertex2f(x2, y2);
	glEnd();
	glDisable(GL_BLEND);
	
	charx = (int)((mouse_x - offx) / scale);
	chary = (int)((mouse_y - offy) / scale);
	
	if(charx >= 0 && charx < sprite->width &&
		chary >= 0 && chary < sprite->height)
	{
		int u, v;
		float u1, v1, u2, v2;
		
		int pos = chary * sprite->width + charx;
		int id  = sprite->glyph[pos];
		int COL = sprite->colour[pos];
		u32 BG  = colour_palette[(COL >> 4) & 0xF];
		u32 FG  = colour_palette[COL&0xF];
		
		x1 = 2.0f;
		y1 = (float)(wnd_height - 18);
		x2 = x1 + 16.0f;
		y2 = y1 + 16.0f;
		
		set_char_pos(id, &u, &v);
		
		u1 = u / 256.0f;
		v1 = v / 256.0f;
		u2 = (u + 8) / 256.0f;
		v2 = (v + 8) / 256.0f;

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_STRIP);
			glColor3f(1, 1, 1);
			glTexCoord2f(u1, v1); glVertex2f(x1, y1);
			glTexCoord2f(u2, v1); glVertex2f(x2, y1);
			glTexCoord2f(u1, v2); glVertex2f(x1, y2);
			glTexCoord2f(u2, v2); glVertex2f(x2, y2);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		
		wsprintf(text, "0x%04X  BG:", id);
		draw_text(x2 + 4.0f, (float)wnd_height - 6.0f, text);
		
		x1 += 13.5f * 8.0f;
		x2 += 13.5f * 8.0f;
		
		glBegin(GL_TRIANGLE_STRIP);
			glColor4ubv((u8*)&BG);
			glVertex2f(x1, y1);
			glVertex2f(x2, y1);
			glVertex2f(x1, y2);
			glVertex2f(x2, y2);
		glEnd();
		
		wsprintf(text, "0x%X (%06X)  FG:", (COL >> 4) & 0xF, BG & 0xFFFFFF);
		draw_text(x2 + 4.0f, (float)wnd_height - 6.0f, text);
		
		x1 += 19.5f * 8.0f;
		x2 += 19.5f * 8.0f;
		
		glBegin(GL_TRIANGLE_STRIP);
			glColor4ubv((u8*)&FG);
			glVertex2f(x1, y1);
			glVertex2f(x2, y1);
			glVertex2f(x1, y2);
			glVertex2f(x2, y2);
		glEnd();
		
		wsprintf(text, "0x%X (%06X)", COL&0xF, FG & 0xFFFFFF);
		draw_text(x2 + 4.0f, (float)wnd_height - 6.0f, text);
	}
	
	wsprintf(text, "(%d, %d)", charx, chary);
	draw_text((float)wnd_width - 100.0f, (float)wnd_height - 6.0f, text);
}

void draw_segment(struct olc_sprite *sprite)
{
	float x1 = offx + prevx * scale;
	float y1 = offy + prevy * scale;
	float x2 = x1 + 16.0f * scale;
	float y2 = y1 + 16.0f * scale;
	
	float u1 = prevx / sprite->width;
	float v1 = prevy / sprite->height;
	float u2 = u1 + 16.0f / sprite->width;
	float v2 = v1 + 16.0f / sprite->height;
	
	glBegin(GL_LINE_STRIP);
		glColor3f(0.3f, 0.3f, 0.1f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		glVertex2f(x1, y1);
	glEnd();
	
	x1 = wnd_width - 16.0f * 8.0f;
	y1 = 0;
	x2 = (float)wnd_width;
	y2 = 16.0f * 8.0f;
	
	glEnable(GL_BLEND);
	glBegin(GL_TRIANGLE_STRIP);
		glColor4f(0, 0, 0, 0.8f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x1, y2);
		glVertex2f(x2, y2);
	glEnd();
	
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(u1, v1); glVertex2f(x1, y1);
		glTexCoord2f(u2, v1); glVertex2f(x2, y1);
		glTexCoord2f(u1, v2); glVertex2f(x1, y2);
		glTexCoord2f(u2, v2); glVertex2f(x2, y2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	glBegin(GL_LINE_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		glVertex2f(x1, y1);
	glEnd();
}

void draw_sprite(struct olc_sprite *sprite)
{
	int x, y;
	
	float x1 = offx;
	float y1 = offy;
	float x2 = x1 + sprite->width * scale;
	float y2 = y1 + sprite->height * scale;
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, sprite_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scale > 24.0f ? GL_NEAREST : GL_LINEAR);
	
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(x1, y1);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(x2, y1);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(x1, y2);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(x2, y2);
	glEnd();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
	
	if(scale > 16.0f)
	{
		float alpha = scale < 32.0f ? (scale - 16.0f) / 64.0f : 0.25f;

		glBegin(GL_LINES);
			glColor4f(0.1f, 0.1f, 0.1f, alpha);
			
			for(y = 0; y < sprite->height; y++)
			{
				float posy = y * scale + offy;
				float posx1 = offx;
				float posx2 = offx + sprite->width * scale;
				
				glVertex2f(posx1, posy);
				glVertex2f(posx2, posy);
			}				
			
			for(x = 0; x < sprite->width; x++)
			{
				float posx = x * scale + offx;
				float posy1 = offy;
				float posy2 = offy + sprite->height * scale;
				
				glVertex2f(posx, posy1);
				glVertex2f(posx, posy2);
			}
		glEnd();
	}
	
	glDisable(GL_BLEND);
	
	glBegin(GL_LINE_STRIP);
		glColor3f(0.2f, 0.2f, 0.2f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		glVertex2f(x1, y1);
	glEnd();
	
	draw_segment(sprite);
	
	glBindTexture(GL_TEXTURE_2D, font_texture);
	draw_infobar(sprite);
}