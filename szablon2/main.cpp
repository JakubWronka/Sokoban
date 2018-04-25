#define _USE_MATH_DEFINES
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define SIZE 32
#define BACKGROUND 1
#define WALL 2
#define PLAYER 3
#define CHEST 4
#define ENDBLOCK 5
#include<math.h>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}


struct block
{
	int x = 0;
	int y = 0;
	int type;
};

void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};

void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};

void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

int offset(int a)
{
	return (SIZE*a + SIZE / 2);
}

void buildMap(block** gameSurface)
{
	for (int i = 0; i < SCREEN_WIDTH / SIZE; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT / SIZE; j++)
		{
			if ((i == 0) || (i == 1) || (i == 2) || (i == SCREEN_WIDTH / SIZE - 3) || (i == SCREEN_WIDTH / SIZE - 2) || (i == SCREEN_WIDTH / SIZE - 1))
			{
				gameSurface[i][j].type = WALL;
				gameSurface[i][j].x = offset(i);
				gameSurface[i][j].y = offset(j);
			}
			else if ((j == 0) || (j == 1) || (j == 2) || (j == SCREEN_HEIGHT / SIZE - 3) || (j == SCREEN_HEIGHT / SIZE - 2) || (j == SCREEN_HEIGHT / SIZE - 1))
			{
				gameSurface[i][j].type = WALL;
				gameSurface[i][j].x = offset(i);
				gameSurface[i][j].y = offset(j);
			}
			else if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
			{
				gameSurface[i][j].type = ENDBLOCK;
				gameSurface[i][j].x = offset(i);
				gameSurface[i][j].y = offset(j);
			}
			else if ((i == 5) && (j == 10))
			{
				gameSurface[i][j].type = PLAYER;
				gameSurface[i][j].x = offset(i);
				gameSurface[i][j].y = offset(j);
			}
			else if (((i == 10) && (j == 9)) || ((i == 12)&&(j == 6)))
			{
				gameSurface[i][j].type = CHEST;
				gameSurface[i][j].x = offset(i);
				gameSurface[i][j].y = offset(j);
			}
			else
			{
				gameSurface[i][j].type = BACKGROUND;
				gameSurface[i][j].x = offset(i);
				gameSurface[i][j].y = offset(j);
			}
		}
	}
}

void DrawGameSurface(block** gameSurface, SDL_Surface *player, SDL_Surface *wall, SDL_Surface *chest, SDL_Surface* endblock, SDL_Surface *screen)
{
	for (int i = 0; i < SCREEN_WIDTH / SIZE; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT / SIZE; j++)
		{
			if (gameSurface[i][j].type == WALL)
			{
				DrawSurface(screen, wall, offset(i), offset(j));
			}
			else if (gameSurface[i][j].type == ENDBLOCK)
			{
				DrawSurface(screen, endblock, offset(i), offset(j));
			}
			else if (gameSurface[i][j].type == PLAYER)
			{
				DrawSurface(screen, player, offset(i), offset(j));
			}
			else if (gameSurface[i][j].type == CHEST)
			{
				DrawSurface(screen, chest, offset(i), offset(j));
			}
			else;
		}
	}
}

void moveUp(block** gameSurface)
{
	for (int i = 0; i < SCREEN_WIDTH / SIZE; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT / SIZE; j++)
		{
			if (gameSurface[i][j].type == PLAYER)
			{
				if (gameSurface[i][j - 1].type != WALL)
				{
					if (gameSurface[i][j - 1].type == CHEST)
					{
						if ((gameSurface[i][j - 2].type != CHEST) && (gameSurface[i][j - 2].type != WALL))
						{
							gameSurface[i][j - 2].type = CHEST;
							gameSurface[i][j - 1].type = PLAYER;
							if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
							{
								gameSurface[i][j].type = ENDBLOCK;
							}
							else
								gameSurface[i][j].type = BACKGROUND;
							return;
						}
						else;
					}
					else
					{
						gameSurface[i][j - 1].type = PLAYER;
						if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
						{
							gameSurface[i][j].type = ENDBLOCK;
						}
						else
							gameSurface[i][j].type = BACKGROUND;
						return;
					}
				}
			}
		}
	}
}

void moveDown(block** gameSurface)
{
	for (int i = 0; i < SCREEN_WIDTH / SIZE; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT / SIZE; j++)
		{
			if (gameSurface[i][j].type == PLAYER)
			{
				if (gameSurface[i][j + 1].type != WALL)
				{
					if (gameSurface[i][j + 1].type == CHEST)
					{
						if ((gameSurface[i][j + 2].type != CHEST) && (gameSurface[i][j + 2].type != WALL))
						{
							gameSurface[i][j + 2].type = CHEST;
							gameSurface[i][j + 1].type = PLAYER;
							if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
							{
								gameSurface[i][j].type = ENDBLOCK;
							}
							else
								gameSurface[i][j].type = BACKGROUND;
							return;
						}
						else;
					}

					else
					{
						gameSurface[i][j + 1].type = PLAYER;
						if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
						{
							gameSurface[i][j].type = ENDBLOCK;
						}
						else
							gameSurface[i][j].type = BACKGROUND;
						return;
					}
				}
			}
		}
	}
}

void moveLeft(block** gameSurface)
{
	for (int i = 0; i < SCREEN_WIDTH / SIZE; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT / SIZE; j++)
		{
			if (gameSurface[i][j].type == PLAYER)
			{
				if (gameSurface[i-1][j].type != WALL)
				{
					if (gameSurface[i - 1][j].type == CHEST)
					{
						if ((gameSurface[i - 2][j].type != CHEST) && (gameSurface[i - 2][j].type != WALL))
						{
							gameSurface[i - 2][j].type = CHEST;
							gameSurface[i - 1][j].type = PLAYER;
							if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
							{
								gameSurface[i][j].type = ENDBLOCK;
							}
							else
								gameSurface[i][j].type = BACKGROUND;
							return;
						}
						else;
					}

					else
					{
						gameSurface[i - 1][j].type = PLAYER;
						if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
						{
							gameSurface[i][j].type = ENDBLOCK;
						}
						else
							gameSurface[i][j].type = BACKGROUND;
						return;
					}
				}
			}
		}
	}
}

void moveRight(block** gameSurface)
{
	for (int i = 0; i < SCREEN_WIDTH / SIZE; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT / SIZE; j++)
		{
			if (gameSurface[i][j].type == PLAYER)
			{
				if (gameSurface[i + 1][j].type != WALL)
				{
					if (gameSurface[i + 1][j].type == CHEST)
					{
						if ((gameSurface[i + 2][j].type != CHEST) && (gameSurface[i + 2][j].type != WALL))
						{
							gameSurface[i + 2][j].type = CHEST;
							gameSurface[i + 1][j].type = PLAYER;
							if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
							{
								gameSurface[i][j].type = ENDBLOCK;
							}
							else
								gameSurface[i][j].type = BACKGROUND;
							return;
						}
						else;
					}
					else
					{
						gameSurface[i + 1][j].type = PLAYER;
						if (((i == 7) && (j == 5)) || (i == 11) && (j == 4))
						{
							gameSurface[i][j].type = ENDBLOCK;
						}
						else
							gameSurface[i][j].type = BACKGROUND;
						return;
					}
				}
			}
		}
	}
}

bool ifEnd(block** gameSurface)
{
	if ((gameSurface[7][5].type == CHEST) && (gameSurface[11][4].type == CHEST))
		return true;
	else return false;
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti, *player, *wall, *chest, *endblock;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	block **gameSurface;
	gameSurface = (block**)malloc(SCREEN_WIDTH/SIZE * sizeof(block*));
	for (int i = 0; i < SCREEN_WIDTH/SIZE; i++)
	{
		gameSurface[i] = (block*)malloc(SCREEN_HEIGHT/SIZE * sizeof(block));
	}

	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Jakub Wronka 165156 Sokoban");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);

	player = SDL_LoadBMP("./player.bmp");
	if (player == NULL) {
		printf("SDL_LoadBMP(player.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	wall = SDL_LoadBMP("./wall.bmp");
	if (wall == NULL) {
		printf("SDL_LoadBMP(wall.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	chest = SDL_LoadBMP("./chest.bmp");
	if (chest == NULL) {
		printf("SDL_LoadBMP(chest.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	endblock = SDL_LoadBMP("./end.bmp");
	if (chest == NULL) {
		printf("SDL_LoadBMP(end.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x60, 0xC0, 0xC0);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	
	buildMap(gameSurface);
	
	while(!quit) {
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		if (ifEnd(gameSurface))
		{
			buildMap(gameSurface);
			t1 = SDL_GetTicks();
			worldTime = 0;
		}
		SDL_FillRect(screen, NULL, niebieski);
		DrawGameSurface(gameSurface, player, wall, chest, endblock, screen);
		// tekst informacyjny / info text
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 20, czerwony, niebieski);
		//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
		sprintf(text, "Jakub Wronka, projekt nr 2, czas gry = %.1lf s ", worldTime);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);


		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_n)
					{
						buildMap(gameSurface);
						t1 = SDL_GetTicks();
						worldTime = 0;
					}
					else if (event.key.keysym.sym == SDLK_UP)
					{
						moveUp(gameSurface);
					}
					else if (event.key.keysym.sym == SDLK_DOWN)
					{
						moveDown(gameSurface);
					}
					else if (event.key.keysym.sym == SDLK_RIGHT)
					{
						moveRight(gameSurface);
					}
					else if (event.key.keysym.sym == SDLK_LEFT)
					{
						moveLeft(gameSurface);
					}
					break;
				case SDL_KEYUP:
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
		//frames++;
		};


	for (int i = 0; i < SCREEN_WIDTH/SIZE; i++)
	{
		free(gameSurface[i]);
	}
	free(gameSurface);
	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
