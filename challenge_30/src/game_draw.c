/* Copyright (c) 2019 KrossX <krossx@live.com>
* License: http://www.opensource.org/licenses/mit-license.html  MIT License
*/

u32 piece_color[TETROMINO_TOTAL] = // 80 100 60 90
{
	0x00000000, //{0.0f, 0.0f, 0.0f}, // NONE - BLACK
	0x00E0E55B, //	054, 255, 247, //{0.0f, 1.0f, 1.0f}, // I    - CYAN
	0x00E5A75B, // 054, 165, 255, //{0.0f, 0.0f, 1.0f}, // J    - BLUE
	0x005B97E5,  // 255, 140, 054, //{1.0f, 0.5f, 0.0f}, // L    - ORANGE
	0x005BE5E3, // 252, 255, 054, //{1.0f, 1.0f, 0.0f}, // O    - YELLOW
	0x005BE567, //071, 255, 054, //{0.0f, 1.0f, 0.0f}, // S    - GREEN
	0x00E55BE5, //255, 054, 225, //{1.0f, 0.0f, 1.0f}, // T    - PURPLE
	0x005B5EE5  //255, 056, 054  //{1.0f, 0.0f, 0.0f}  // Z    - RED
};

u8 font[] = {0x60, 0xA0, 0xA0, 0xA0, 0xC0, // 0
			 0x40, 0xC0, 0x40, 0x40, 0xE0, // 1
			 0xC0, 0x20, 0x40, 0x80, 0xE0, // 2
			 0xC0, 0x20, 0x40, 0x20, 0xC0, // 3
			 0x20, 0xA0, 0xE0, 0x20, 0x20, // 4
			 0xE0, 0x80, 0xC0, 0x20, 0xC0, // 5
			 0x40, 0x80, 0xC0, 0xA0, 0x40, // 6
			 0xE0, 0x20, 0x60, 0x40, 0x40, // 7
			 0x40, 0xA0, 0x40, 0xA0, 0x40, // 8
			 0x40, 0xA0, 0x60, 0x20, 0x40}; // 9
			
union rbga {
	struct {u8 r,g,b,a;};
	u32 data;
};

float clampxf(float val, float min, float max)
{
	return val < min ? min : val > max ? max : val;
}

u32 color_lerp(u32 c1, u32 c2, float factor)
{
	union rbga uc1, uc2;
	float r, g, b;
	
	uc1.data = c1;
	uc2.data = c2;
	
	factor = clampxf(factor, 0, 1);
	
	r = (1.0f-factor)*uc1.r + factor*uc2.r;
	g = (1.0f-factor)*uc1.g + factor*uc2.g;
	b = (1.0f-factor)*uc1.b + factor*uc2.b;
	
	uc1.r = (u8)clampxf(r, 0, 255);
	uc1.g = (u8)clampxf(g, 0, 255);
	uc1.b = (u8)clampxf(b, 0, 255);

	return uc1.data;
}

void draw_digit(int posx, int posy, int digit)
{
	int x, y;
	u32 col = 0;
	float factor = 0.1f;
	
	if(levelup_animation > 0) {
		factor = (float)levelup_animation * 2.0f;
		factor = 1.0f - factor;
		factor = 1.0f - factor * factor + 0.1f;

		col = (int)(0xFF * clampxf(factor, 0, 1));
		col = col | col << 8 | col << 16;
	}
	
	for(y = 0; y < 5; y++) {
		u8 fbyte = font[digit * 5 + y];
		for(x = 0; x < 4; x++) {
			u8 pix = (fbyte >> (x + 4))&1;
			if(pix) fbuf[posy-y][posx-x] = color_lerp(fbuf[posy-y][posx-x], col, factor);
		}
	}
}

void game_draw(void)
{
	static double time_accum;
	int x, y, xmin = 999, xmax = -999;
	u32 color;
	
	time_accum += time_diff;
	
	for(y = 0; y < 30; y++) {
		int i = (int)(128 + sin(y * 0.1 + time_accum * (user_level+2) * 0.3) * 32);
		int *line = fbuf[y];
		for(x = 0; x < 30; x++) line[x] = (i << 16) | 0x3F3F;
	}
	
	draw_digit(25, 28, user_level / 10);
	draw_digit(29, 28, user_level % 10);
	
	if(levelup_animation > 0)
		levelup_animation -= time_diff;
	
	for(y = 0; y < 4; y++)
	for(x = 0; x < 4; x++) {
		int px = x + pghost.posx;
		int py = y + pghost.posy;
		int cell = pghost.data[y][x];
		
		if(cell && px>=0 && px<30 && py>=0 && py<30) {
			if(px < xmin) xmin = px;
			if(px > xmax) xmax = px;
		}
	}
	
	for(y = 0; y < 30; y++)
	for(x = xmin; x <= xmax; x++) {
		u32 col = fbuf[y][x];
		fbuf[y][x] = color_lerp(col, 0, 0.05f);
	}
	
	for(y = 0; y < 30; y++)
	for(x = 0; x < 30; x++) {
		int cell = tgrid[y][x];
		if(cell) fbuf[y][x] = piece_color[cell];
	}
	
	if(hdrop_animation > 0) {
		float factor = (float)hdrop_animation * 4.0f;
		factor = 1.0f - factor;
		factor = 1.0f - factor * factor;

		color = piece_color[phdrop.type];
		color = color_lerp(color, -1, factor);
		
		for(y = 0; y < 4; y++)
		for(x = 0; x < 4; x++) {
			int px = x + phdrop.posx;
			int py = y + phdrop.posy;
			int cell = phdrop.data[y][x];
			if(cell && px>=0 && px<30 && py>=0 && py<30)
				fbuf[py][px] = color;
		}
		
		hdrop_animation -= time_diff;
	}

	color = piece_color[pcur.type];
	
	for(y = 0; y < 4; y++)
	for(x = 0; x < 4; x++) {
		int px = x + pghost.posx;
		int py = y + pghost.posy;
		int cell = pghost.data[y][x];
		
		if(cell && px>=0 && px<30 && py>=0 && py<30) 
			fbuf[py][px] = color_lerp(fbuf[py][px], color, 0.2f);
	}
	
	for(y = 0; y < 4; y++)
	for(x = 0; x < 4; x++) {
		int px = x + pcur.posx;
		int py = y + pcur.posy;
		int cell = pcur.data[y][x];
		if(cell && px>=0 && px<30 && py>=0 && py<30)
			fbuf[py][px] = color;
	}

	if(line_animation > 0) {
		float factor = (float)line_animation * 2.0f;
		factor = 1.0f - factor;
		factor = 1.0f - factor * factor;
		
		for(y = 0; y < 4; y++){
			int ly = line_cleared[y];
			if(ly) {
				u32 *line = fbuf[ly-1];
				for(x = 0; x < 30; x++)
					line[x] = color_lerp(line[x], -1, factor);
			}
		}

		line_animation -= time_diff;
	}
}
