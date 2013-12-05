#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL\SDL.h>
#include <SDL\SDL_ttf.h>

#define SIZE_WIN	(900)

#define BLUE_STATE	0
#define WHITE_STATE	1

#define PXSIZE_FONT 56

int			*_map, _hit;
size_t		_size_map, _size_map, _size_pixel, _size;
SDL_Event	_event;
float		_elapsed_time = 0.f;

typedef struct ressources_s {
	SDL_Surface		*screen;
	SDL_Surface		*square;
	SDL_Surface		*text_time;
	TTF_Font		*font;
}ressource_t;

ressource_t	_res = {NULL, NULL};

void	res_load(void) {
	_res.screen		= SDL_SetVideoMode(SIZE_WIN, SIZE_WIN, 32, SDL_DOUBLEBUF | SDL_HWSURFACE);
	_res.font		= TTF_OpenFont("fonts/consola.ttf", PXSIZE_FONT);
}

void	res_unload(void) {
	SDL_FreeSurface(_res.screen);
	SDL_FreeSurface(_res.square);
	SDL_FreeSurface(_res.text_time);
	
	TTF_CloseFont(_res.font);
}

void	map_new(void) {
	int x, y;

	for (x = 0; x < _size; x++) {
		for (y = 0; y < _size; y++) {
			_map[_size * x + y] = WHITE_STATE;
		}
	}

	_elapsed_time = 0;
	_hit = 0;
}

void	map_rand(void) {
	int x, y;

	map_new();

	for (x = 0; x < _size; x++) {
		for (y = 0; y < _size; y++) {
			if ((rand() % 5) == 2)
				_map[_size * x + y] = BLUE_STATE;
		}
	}
}

void	map_create(int size) {
	_size		= size;
	_size_map	= (size * size);
	_hit		= 0;

	if (size > 1) {
		_map = (int *)malloc(_size_map * sizeof(int));

		_size_pixel = (SIZE_WIN / size);
	}
	else {
		exit(1);
	}

	map_new();
}

void	map_blit(void) {
	SDL_Rect	rect_square;
	int			y,
				x;

	_res.square = SDL_CreateRGBSurface(SDL_HWSURFACE, _size_pixel - 1, _size_pixel - 1, 32, 0, 0, 0, 0);

	for (x = 0; x < _size; x++) {
		for (y = 0; y < _size; y++) {
			if (_map[_size * x + y] == BLUE_STATE) {
				SDL_FillRect(_res.square, NULL, 0x0000FF);
			}
			else {
				SDL_FillRect(_res.square, NULL, 0xFFFFFF);
			}

			if (x == (_event.motion.x / _size_pixel) && y == (_event.motion.y / _size_pixel))
				SDL_FillRect(_res.square, NULL, 0x3A9D23);

			rect_square.x = x * _size_pixel;
			rect_square.y = y * _size_pixel;

			SDL_BlitSurface(_res.square, NULL, _res.screen, &rect_square);
		}
	}
}

void	map_add_square(int x, int y) {

	_map[_size * x + y] = !_map[_size * x + y];

	if (x > 0)
		_map[_size * (x - 1) + y] = !_map[_size * (x - 1) + y];
	if (y > 0)
		_map[_size * x + y - 1] = !_map[_size * x + y - 1];
	if (x < _size - 1)
		_map[_size * (x + 1) + y] = !_map[_size * (x + 1) + y];
	if (y < _size - 1)
		_map[_size * x + y + 1] = !_map[_size * x + y + 1];
}

int		check_win(void) {
	int x, y, good_square = 0;

	for (x = 0; x < _size; x++)
		for (y = 0; y < _size; y++)
			if (_map[_size * x + y] == BLUE_STATE)
				good_square++;
	if (good_square == _size_map)
		return 1;
	return 0;
}

void	map_free(void) {
	free(_map);
}

void	handle_event_map(void) {
	switch (_event.type) {
	case SDL_MOUSEBUTTONUP:
		if (_event.button.button == SDL_BUTTON_LEFT) {
			_hit++;
			map_add_square((_event.motion.x / _size_pixel), (_event.motion.y / _size_pixel));
		}
		break;
	}
}

void	time_blit(int blit_time) {
	static int		act_time, old_time = 0;
	static char		time[50] = { 0 };
	size_t			size_text;
	SDL_Rect		rect;
	SDL_Color		color = { 255, 0, 0, 0 };

	if (blit_time) {
		size_text = strlen(time);
		rect.x = rect.y = (SIZE_WIN >> 1) - size_text - PXSIZE_FONT;
		_res.text_time = TTF_RenderText_Blended(_res.font, time, color);
	}

	act_time = SDL_GetTicks();

	if (act_time - old_time >= 10) {
		_elapsed_time += 10;
		if (blit_time)
			sprintf	(time, "%.2f", _elapsed_time / 1000);
		old_time = act_time;
	}
	if (blit_time)
		SDL_BlitSurface(_res.text_time, NULL, _res.screen, &rect);
}

void	help(void) {
	printf("\n[HELP]:\n");
	printf("\t- 1 for a generated random map\n");
	printf("\t- R for restart\n");
}

int main(int argc, char *argv[]) {
	int			running = 1, print_timer = 0;
	TTF_Font	*font = NULL;
	SDL_Surface	*text = NULL;
	SDL_Color	color = { 255, 255, 255, 0 };
	SDL_Rect	pos_text_win;
	char		text_char[256];
	
	srand(time(NULL));

	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	TTF_Init();
	atexit(TTF_Quit);
	
	SDL_WM_SetCaption("Blue and White", NULL);

	printf("[PixelCode Team] Blue and White - Version 1.0\n\n");
	printf("Press H for help.\nSize of the map (<= 100): ");
	scanf("%d", &_size_map);

	res_load();

	if (_size_map <= 100)
		map_create(_size_map);
	else {
		_size_map = 100;
		map_create(_size_map);
	}

	pos_text_win.x = (SIZE_WIN >> 1) - strlen(text_char) - PXSIZE_FONT;
	pos_text_win.y = (SIZE_WIN >> 1) - PXSIZE_FONT;

	time_blit(0);

	while (running) {
		while (SDL_PollEvent(&_event)) {
			switch (_event.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch (_event.key.keysym.sym) {
				case SDLK_r:
					map_new();
					break;
				case SDLK_1:
					map_rand();
					break;
				case SDLK_t:
					print_timer = 1;
					break;
				case SDLK_h:
					help();
					break;
				}
				break;
			case SDL_KEYUP:
				switch (_event.key.keysym.sym) {
				case SDLK_t:
					print_timer = 0;
					break;
				}
				break;
			}
			handle_event_map();
		}

		SDL_FillRect(_res.screen, NULL, 0x000000);

		map_blit();

		if (print_timer)
			time_blit(1);
		else
			time_blit(0);

		if (check_win()) {
			sprintf(text_char, "You won in %d hits !", _hit);
			text = TTF_RenderText_Blended(_res.font, text_char, color);
			SDL_BlitSurface(text, NULL, _res.screen, &pos_text_win);
		}

		SDL_Flip(_res.screen);
	}

	res_unload();

	SDL_FreeSurface(text);

	return EXIT_SUCCESS;
}
