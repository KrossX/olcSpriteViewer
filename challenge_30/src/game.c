/* Copyright (c) 2019 KrossX <krossx@live.com>
* License: http://www.opensource.org/licenses/mit-license.html  MIT License
*/

enum
{
	TETROMINO_NONE,
	TETROMINO_I,
	TETROMINO_J,
	TETROMINO_L,
	TETROMINO_O,
	TETROMINO_S,
	TETROMINO_T,
	TETROMINO_Z,
	TETROMINO_TOTAL
};

int tetromino_start[TETROMINO_TOTAL][2] =
{
	{ 0, -1}, // NONE
	{ 3, 18}, // I
	{ 3, 19}, // J
	{ 3, 19}, // L
	{ 3, 19}, // O
	{ 3, 19}, // S
	{ 3, 19}, // T
	{ 3, 19}  // Z
};

int tetromino[TETROMINO_TOTAL][4][4][4] = 
{
	//TETROMINO_NONE
	{
		{{0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}}
	},

	//TETROMINO_I
	{
		{{0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {1, 1, 1, 1},
		 {0, 0, 0, 0}},

		{{0, 0, 1, 0},
		 {0, 0, 1, 0},
		 {0, 0, 1, 0},
		 {0, 0, 1, 0}},

		{{0, 0, 0, 0},
		 {1, 1, 1, 1},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 0, 0}}
	},

	//TETROMINO_J
	{
		{{0, 0, 0, 0},
		 {2, 2, 2, 0},
		 {2, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 2, 0, 0},
		 {0, 2, 0, 0},
		 {0, 2, 2, 0},
		 {0, 0, 0, 0}},

		{{0, 0, 2, 0},
		 {2, 2, 2, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{2, 2, 0, 0},
		 {0, 2, 0, 0},
		 {0, 2, 0, 0},
		 {0, 0, 0, 0}}
	},

	//TETROMINO_L
	{
		{{0, 0, 0, 0},
		 {3, 3, 3, 0},
		 {0, 0, 3, 0},
		 {0, 0, 0, 0}},

		{{0, 3, 3, 0},
		 {0, 3, 0, 0},
		 {0, 3, 0, 0},
		 {0, 0, 0, 0}},

		{{3, 0, 0, 0},
		 {3, 3, 3, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 3, 0, 0},
		 {0, 3, 0, 0},
		 {3, 3, 0, 0},
		 {0, 0, 0, 0}}
	},

	//TETROMINO_O
	{
		{{0, 0, 0, 0},
		 {0, 4, 4, 0},
		 {0, 4, 4, 0},
		 {0, 0, 0, 0}},

		{{0, 0, 0, 0},
		 {0, 4, 4, 0},
		 {0, 4, 4, 0},
		 {0, 0, 0, 0}},

		{{0, 0, 0, 0},
		 {0, 4, 4, 0},
		 {0, 4, 4, 0},
		 {0, 0, 0, 0}},

		{{0, 0, 0, 0},
		 {0, 4, 4, 0},
		 {0, 4, 4, 0},
		 {0, 0, 0, 0}}
	},

	//TETROMINO_S
	{
		{{0, 0, 0, 0},
		 {5, 5, 0, 0},
		 {0, 5, 5, 0},
		 {0, 0, 0, 0}},

		{{0, 0, 5, 0},
		 {0, 5, 5, 0},
		 {0, 5, 0, 0},
		 {0, 0, 0, 0}},

		{{5, 5, 0, 0},
		 {0, 5, 5, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 5, 0, 0},
		 {5, 5, 0, 0},
		 {5, 0, 0, 0},
		 {0, 0, 0, 0}}
	},

	//TETROMINO_T
	{
		{{0, 0, 0, 0},
		 {6, 6, 6, 0},
		 {0, 6, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 6, 0, 0},
		 {0, 6, 6, 0},
		 {0, 6, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 6, 0, 0},
		 {6, 6, 6, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 6, 0, 0},
		 {6, 6, 0, 0},
		 {0, 6, 0, 0},
		 {0, 0, 0, 0}}
	},

	//TETROMINO_Z
	{
		{{0, 0, 0, 0},
		 {0, 7, 7, 0},
		 {7, 7, 0, 0},
		 {0, 0, 0, 0}},

		{{0, 7, 0, 0},
		 {0, 7, 7, 0},
		 {0, 0, 7, 0},
		 {0, 0, 0, 0}},

		{{0, 7, 7, 0},
		 {7, 7, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		{{7, 0, 0, 0},
		 {7, 7, 0, 0},
		 {0, 7, 0, 0},
		 {0, 0, 0, 0}}
	}
};

struct piece
{
	int posx, posy, rot, type;
	int (*data)[4];
};

struct piece pzero  = {0,-2, 0, 0, tetromino[0][0]};
struct piece pcur   = {0,-2, 0, 0, tetromino[0][0]};
struct piece pghost = {0,-2, 0, 0, tetromino[0][0]};
struct piece phdrop = {0,-2, 0, 0, tetromino[0][0]};

int counter_reset;
int user_level;
int lines_cleared;
int line_gravity;
int line_cleared[4];

double line_animation;
double hdrop_animation;
double levelup_animation;

int check_collision(struct piece *p)
{
	int x, y;
	
	if(p->posy < -2) return 1;
	
	for(y = 0; y < 4; y++)
	for(x = 0; x < 4; x++) {
		int cell = p->data[y][x];
		if(cell) {
			int posx = p->posx + x;
			int posy = p->posy + y;
			
			if(posx >= 0 && posx < 30 && posy >= 0 && posy < 32) {
				if(tgrid[posy][posx]) return 1;
			} else {
				return 1;
			}
		}
	}

	return 0;
}

void update_ghost(void)
{
	pghost = pcur;
	
	while(!check_collision(&pghost))
		pghost.posy--;
	
	pghost.posy++;	
}

void next_piece(void)
{
	static int order[14] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
	static int get = 14;
	
	if(get > 13)	{
		int i;
		for(i = 13; i > 0; i--) {
			int j = (rand() * (i + 1) - 1) / RAND_MAX;
			int t = order[i];
			order[i] = order[j];
			order[j] = t;
		}
		
		get = 0;
	}
	
	pcur.posx = 13;
	pcur.posy = 28;
	pcur.rot  = 0;
	pcur.type = order[get];
	pcur.data = tetromino[order[get]][0];

	get++;
}

void gravity(void)
{
	int i, j;

	for(i = 0; i < 4; i++) {
		if(line_cleared[i]) {
			for(j = line_cleared[i] - 1; j < 19; j++)
				CopyMemory(tgrid[j], tgrid[j+1], sizeof(int) * 30);

			for(j = 19; j < 32; j++)
				ZeroMemory(tgrid[j], sizeof(int) * 30);
	
			line_cleared[i] = 0;
		}
	}

	next_piece();
	update_ghost();
}

void piece_land(void)
{
	int i, j, z = 0;
	int ymin = 999, ymax = -999;
	int lines = 0;
	
	for(j = 0; j < 4; j++)
	for(i = 0; i < 4; i++) {
		int cell = pcur.data[j][i];

		if(cell) {
			int posx = pcur.posx + i;
			int posy = pcur.posy + j;
			
			if(posy < ymin) ymin = posy;
			if(posy > ymax) ymax = posy;
			
			tgrid[posy][posx] = cell;
		}
	}
	
	if(ymax > 29) {
		// GAME OVER!
		levelup_animation = 0.5;
		lines_cleared = 0;
		user_level    = 0;
		ZeroMemory(tgrid, sizeof(tgrid));
		next_piece();
		update_ghost();
		play_music(BGM_TRACK1);
		return;
	}
	
	for(j = ymax; j >= ymin; j--) {
		int filled = 1;
		
		for(i = 0; i < 30; i++) {
			if(!tgrid[j][i]) {
				filled = 0;
				break;
			}
		}
		
		if(filled) {
			ZeroMemory(tgrid[j], sizeof(int) * 30);
			line_cleared[z] = j + 1;
			lines++;
		} else {
			line_cleared[z] = 0;
		}

		z++;
	}
	
	if(lines) {
		line_animation = 0.5;
		line_gravity = 1;
		
		pcur   = pzero;
		pghost = pzero;
		
		switch(lines) {
			case 1: play_sound(SND_LINE); break;
			case 2: play_sound(SND_LINE); break;
			case 3: play_sound(SND_LINE); break;
			case 4: play_sound(SND_TETRIS); break;
		}
		
		lines_cleared += lines;
		
		if(lines_cleared/5 > user_level) {
			if(user_level ==  9) play_music(BGM_TRACK2);
			if(user_level == 19) play_music(BGM_TRACK3);

			play_sound(SND_LEVELUP);
			levelup_animation = 0.5;
		}

		user_level = lines_cleared/5;
		
	} else {
		next_piece();
		update_ghost();
	}
}

void game_step(void)
{
	static double dt;

	double delay = (10.0 - user_level * 0.4) * 0.1;
	if(delay < 0.1) delay = 0.1;
	
	dt += time_diff;

	if(line_animation <= 0 && line_gravity) {
		line_gravity = 0;
		gravity();
	} else  if(dt >= delay) {
		dt -= delay;
		pcur.posy--;
		
		if(check_collision(&pcur)) {
			pcur.posy++;
			piece_land();
			play_sound(SND_DOWN);
		} else {
			play_sound(SND_FALL);
		}
	}
}

void game_input(void)
{
	if(keyboard[VK_LEFT]) {
		keyboard[VK_LEFT] = 0;
		pcur.posx--;
		if(check_collision(&pcur)) {
			pcur.posx++;
		} else {
			play_sound(SND_MOVE);
			update_ghost();
		}
	}
	
	if(keyboard[VK_RIGHT]) {
		keyboard[VK_RIGHT] = 0;
		pcur.posx++;
		if(check_collision(&pcur)) {
			pcur.posx--;
		} else {
			play_sound(SND_MOVE);
			update_ghost();
		}
	}
	
	if(keyboard[VK_DOWN]) {
		keyboard[VK_DOWN] = 0;
		pcur.posy--;
		if(check_collision(&pcur)) {
			pcur.posy++;
		} else {
			play_sound(SND_FALL);
			update_ghost();
		}
	}
	
	if(keyboard[VK_UP]) {
		keyboard[VK_UP] = 0;

		while(!check_collision(&pcur))
			pcur.posy--;
		
		pcur.posy++;
		phdrop = pcur;
		piece_land();
		play_sound(SND_HDROP);
		hdrop_animation = 0.25;
	}
	
	if(keyboard['Z']) { // rot CCW
		keyboard['Z'] = 0;
		
		pcur.rot = (pcur.rot-1)&3;
		pcur.data = tetromino[pcur.type][pcur.rot];
		
		if(check_collision(&pcur)) {
			pcur.rot = (pcur.rot+1)&3;
			pcur.data = tetromino[pcur.type][pcur.rot];
		} else {
			play_sound(SND_ROTATE);
			update_ghost();
		}
	}
	
	if(keyboard['X']) { // rot CW
		keyboard['X'] = 0;
		
		pcur.rot = (pcur.rot+1)&3;
		pcur.data = tetromino[pcur.type][pcur.rot];
		
		if(check_collision(&pcur)) {
			pcur.rot = (pcur.rot-1)&3;
			pcur.data = tetromino[pcur.type][pcur.rot];
		} else {
			play_sound(SND_ROTATE);
			update_ghost();
		}
	}
	
	
}
