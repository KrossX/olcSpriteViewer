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
	BOOL need_backup;
	BOOL changed;
};

struct olc_pixel
{
	WCHAR id;
	WORD col;
	u32 fg, bg;

	int u, v;
	float u1, v1;
	float u2, v2;
};

void get_pixel_data(struct olc_sprite *sprite, int pos, struct olc_pixel *pix)
{
	pix->id  = sprite->glyph[pos];
	pix->col = sprite->colour[pos];
	
	pix->fg = colour_palette[pix->col & 0xF];
	pix->bg = colour_palette[(pix->col>>4) & 0xF];
	
	set_char_pos(pix->id, &pix->u, &pix->v);
	
	pix->u1 = pix->u / 256.0f;
	pix->v1 = pix->v / 256.0f;
	pix->u2 = (pix->u + 8) / 256.0f;
	pix->v2 = (pix->v + 8) / 256.0f;
}

void gen_sprite_texture(struct olc_sprite *sprite)
{
	struct olc_pixel pix;
	int x, y, px, py;

	u32 *tex = sprite->tex;
	
	for(y = 0; y < sprite->height; y++)
	for(x = 0; x < sprite->width; x++)
	{
		int pos = y * sprite->width + x;
		get_pixel_data(sprite, pos, &pix);

		for(py = 0; py < 8; py++)
		for(px = 0; px < 8; px++)
		{
			int ppos = (y * 8 + py) * (sprite->width * 8) + (x * 8 + px);
			int tpos = (pix.v + py) * 256 + (pix.u + px);
			
			tex[ppos] = pix.id == ' ' ? 0 : font_8x8_256x256[tpos] ? pix.fg : pix.bg;
		}
	}
}

int save_sprite(struct olc_sprite *sprite, char *filename)
{
	HANDLE file;
	
	if(!sprite->changed)
		return 1;
	
	if(sprite->need_backup)
	{
		char backupname[MAX_PATH];
		wsprintf(backupname, "%s.bak", filename);
		
		CopyFile(filename, backupname, FALSE);
		sprite->need_backup = FALSE;
	}
	
	file = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(file != INVALID_HANDLE_VALUE)
	{
		
		DWORD bytes_written;
		DWORD section_bytes = sprite->width * sprite->height * 2;
		
		WriteFile(file, &sprite->width, 4, &bytes_written, NULL);
		WriteFile(file, &sprite->height, 4, &bytes_written, NULL);
		
		WriteFile(file, sprite->colour, section_bytes, &bytes_written, NULL);
		WriteFile(file, sprite->glyph, section_bytes, &bytes_written, NULL);
			
		sprite->changed = 0;
	
		CloseHandle(file);
		return 1;
	}

	return 0;
}

int save_check_msg(struct olc_sprite *sprite, char *filename)
{
	if(sprite->changed)
	{
		int msgret;
			
		char message[512];
		wsprintf(message, "Save changes to %s ?", filename);
					
		msgret = MessageBoxA(NULL, message, "olcSprite", MB_YESNOCANCEL);
		
		if(msgret == IDCANCEL) return 0;
		if(msgret == IDYES) save_sprite(sprite, filename);
	}

	return 1;
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
					sprite->loaded      = TRUE;
					sprite->need_backup = TRUE;
					sprite->changed     = FALSE;	
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
	
	glBindTexture(GL_TEXTURE_2D, font_texture);

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
	
	glBindTexture(GL_TEXTURE_2D, font_texture);
	
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
		struct olc_pixel pix;
		
		int pos = chary * sprite->width + charx;
		get_pixel_data(sprite, pos, &pix);
		
		x1 = 2.0f;
		y1 = (float)(wnd_height - 18);
		x2 = x1 + 16.0f;
		y2 = y1 + 16.0f;
		
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_STRIP);
			glColor3f(1, 1, 1);
			glTexCoord2f(pix.u1, pix.v1); glVertex2f(x1, y1);
			glTexCoord2f(pix.u2, pix.v1); glVertex2f(x2, y1);
			glTexCoord2f(pix.u1, pix.v2); glVertex2f(x1, y2);
			glTexCoord2f(pix.u2, pix.v2); glVertex2f(x2, y2);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		
		wsprintf(text, "0x%04X  BG:", pix.id);
		draw_text(x2 + 4.0f, (float)wnd_height - 6.0f, text);
		
		x1 += 13.5f * 8.0f;
		x2 += 13.5f * 8.0f;
		
		glBegin(GL_TRIANGLE_STRIP);
			glColor4ubv((u8*)&pix.bg);
			glVertex2f(x1, y1);
			glVertex2f(x2, y1);
			glVertex2f(x1, y2);
			glVertex2f(x2, y2);
		glEnd();
		
		wsprintf(text, "0x%X (%06X)  FG:", (pix.col >> 4) & 0xF, pix.bg & 0xFFFFFF);
		draw_text(x2 + 4.0f, (float)wnd_height - 6.0f, text);
		
		x1 += 19.5f * 8.0f;
		x2 += 19.5f * 8.0f;
		
		glBegin(GL_TRIANGLE_STRIP);
			glColor4ubv((u8*)&pix.fg);
			glVertex2f(x1, y1);
			glVertex2f(x2, y1);
			glVertex2f(x1, y2);
			glVertex2f(x2, y2);
		glEnd();
		
		wsprintf(text, "0x%X (%06X)", pix.col&0xF, pix.fg & 0xFFFFFF);
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
	
	x1 = wnd_width - 16.0f * prevs;
	y1 = 0;
	x2 = (float)wnd_width;
	y2 = 16.0f * prevs;
	
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
}

void draw_editor(struct olc_pixel *pix)
{
	float x1 = 0;
	float y1 = 0;
	float x2 = wnd_width * 0.5f;
	float y2 = 10.0f;
	
	glEnable(GL_BLEND);
	glBegin(GL_TRIANGLE_STRIP);
		glColor4f(1, 0, 0, 1); glVertex2f(x1, y1);
		glColor4f(1, 0, 0, 0); glVertex2f(x2, y1);
		glColor4f(1, 0, 0, 1); glVertex2f(x1, y2);
		glColor4f(1, 0, 0, 0); glVertex2f(x2, y2);
	glEnd();
	
	x1 = 0;
	y1 = 10.0f;
	x2 = x1 + 40.0f;
	y2 = y1 + 40.0f;
	
	glBegin(GL_TRIANGLE_STRIP);
		glColor4f(0, 0, 0, 0.8f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x1, y2);
		glVertex2f(x2, y2);
	glEnd();

	glDisable(GL_BLEND);
	
	x1 = 4;
	y1 = 14.0f;
	x2 = x1 + 32.0f;
	y2 = y1 + 32.0f;
	
	glBegin(GL_TRIANGLE_STRIP);
		glColor4ubv((u8*)&pix->bg);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x1, y2);
		glVertex2f(x2, y2);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, font_texture);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glBegin(GL_TRIANGLE_STRIP);
		glColor4ubv((u8*)&pix->fg);
		glTexCoord2f(pix->u1, pix->v1); glVertex2f(x1, y1);
		glTexCoord2f(pix->u2, pix->v1); glVertex2f(x2, y1);
		glTexCoord2f(pix->u1, pix->v2); glVertex2f(x1, y2);
		glTexCoord2f(pix->u2, pix->v2); glVertex2f(x2, y2);
	glEnd();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
	
	glColor3f(1.0f, 1.0f, 1.0f);
	draw_text(1.0f, 9.0f, "EDIT");
}

void draw_editor_palette(struct olc_pixel *pix)
{
	int x;
	
	float x1 = (float)pal_l;
	float y1 = (float)pal_t;
	float x2 = (float)pal_r;
	float y2 = (float)pal_b;
	
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.3f, 0.3f, 0.3f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x1, y2);
		glVertex2f(x2, y2);
	glEnd();
	
	x1 = (float)pal_l + 1.0f;
	y1 = (float)pal_t + 1.0f;
	x2 = x1 + 15.0f;
	y2 = y1 + 15.0f;
	
	for(x = 0; x < 16; x++)
	{
		glBegin(GL_TRIANGLE_STRIP);
			glColor4ubv((u8*)&colour_palette[x]);
			glVertex2f(x1, y1);
			glVertex2f(x2, y1);
			glVertex2f(x1, y2);
			glVertex2f(x2, y2);
		glEnd();
		
		x1 += 16.0f;
		x2 += 16.0f;
	}
	
	x1 = (float)pal_l + 1.0f;
	y1 = (float)pal_t + 17.0f;
	x2 = x1 + 15.0f;
	y2 = y1 + 15.0f;
	
	for(x = 0; x < 16; x++)
	{
		glBegin(GL_TRIANGLE_STRIP);
			glColor4ubv((u8*)&colour_palette[x]);
			glVertex2f(x1, y1);
			glVertex2f(x2, y1);
			glVertex2f(x1, y2);
			glVertex2f(x2, y2);
		glEnd();
		
		x1 += 16.0f;
		x2 += 16.0f;
	}
	
	x1 = (float)pal_l + 1.0f;
	y1 = (float)pal_t + 33.0f;
	x2 = x1 + 256.0f;
	y2 = y1 + 256.0f;
	
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 0); glVertex2f(x1, y1);
		glTexCoord2f(1, 0); glVertex2f(x2, y1);
		glTexCoord2f(0, 1); glVertex2f(x1, y2);
		glTexCoord2f(1, 1); glVertex2f(x2, y2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
	x1 = (float)pal_l + (pix->col & 0xF) * 16.0f;
	y1 = (float)pal_t + 1.0f;
	x2 = x1 + 16.0f;
	y2 = y1 + 16.0f;
	
	glLineWidth(2.0f);
	glBegin(GL_LINE_STRIP);
		glColor3f(1.0f, 0.5f, 0.0f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		glVertex2f(x1, y1);
	glEnd();
	
	x1 = (float)pal_l + ((pix->col >> 4) & 0xF) * 16.0f;
	y1 = (float)pal_t + 17.0f;
	x2 = x1 + 16.0f;
	y2 = y1 + 16.0f;
	
	glBegin(GL_LINE_STRIP);
		glColor3f(0.0f, 0.5f, 1.0f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		glVertex2f(x1, y1);
	glEnd();
	
	x1 = (float)pal_l + pix->u - 1.0f;
	y1 = (float)pal_t + 31.0f + pix->v;
	x2 = x1 + 12.0f;
	y2 = y1 + 12.0f;
	
	glBegin(GL_LINE_STRIP);
		glColor3f(0.5f, 1.0f, 0.0f);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		glVertex2f(x1, y1);
	glEnd();
	glLineWidth(1.0f);
}


void get_pixel(struct olc_sprite *sprite, struct olc_pixel *pix)
{
	int charx = (int)((mouse_x - offx) / scale);
	int chary = (int)((mouse_y - offy) / scale);
	
	if(charx >= 0 && charx < sprite->width &&
		chary >= 0 && chary < sprite->height)
	{
		int pos = chary * sprite->width + charx;
		get_pixel_data(sprite, pos, pix);
	}
}

void set_pixel(struct olc_sprite *sprite, struct olc_pixel *pix)
{
	int charx = (int)((mouse_x - offx) / scale);
	int chary = (int)((mouse_y - offy) / scale);
	
	u32 *tex = sprite->tex;
	
	if(charx >= 0 && charx < sprite->width &&
		chary >= 0 && chary < sprite->height)
	{
		int px, py;
		int pos = chary * sprite->width + charx;
		
		sprite->glyph[pos] = pix->id;
		sprite->colour[pos] = pix->col;

		for(py = 0; py < 8; py++)
		for(px = 0; px < 8; px++)
		{
			int ppos = (chary * 8 + py) * (sprite->width * 8) + (charx * 8 + px);
			int tpos = (pix->v + py) * 256 + (pix->u + px);
			
			tex[ppos] = pix->id == ' ' ? 0 : font_8x8_256x256[tpos] ? pix->fg : pix->bg;
		}
		
		sprite->changed = TRUE;
	}
}
