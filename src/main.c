#define DEBUG 1
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* constants */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_X 21
#define GRID_Y 12
#define TOTAL_NUMBER_OF_POINTS GRID_X * GRID_Y
#define POINT_WIDTH SCREEN_WIDTH / GRID_X
#define POINT_HEIGHT SCREEN_HEIGHT / GRID_Y

/* global variables */
enum POINT_STATE	{ EMPTY, HEAD, TAIL, EGG };
enum DIRECTION		{ NONE, UP, DOWN, LEFT, RIGHT };
enum GAME_STATES	{ PAUSED, IN_PROGRESS, WIN };

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static unsigned int score = 0, highScore = 0;
static char scoreText[16];
static char highScoreText[16];

static int direction = NONE;
static int gamestate = IN_PROGRESS;
static Uint64 lastTime = 0, currentTime;
static double speed = 0.5f;

static struct point {
	int state;
	int lifespan;
} grid[GRID_X][GRID_Y];

int headPos[] = { GRID_X / 2, GRID_Y / 2};

void generateNewEgg();
void pause();
void resetGame();

void generateNewEgg()
{
	Sint32 eggPos[2];
	eggPos[0] = SDL_rand(GRID_X);
	eggPos[1] = SDL_rand(GRID_Y);
	while (grid[eggPos[0]][eggPos[1]].state == HEAD || 
			grid[eggPos[0]][eggPos[1]].state == TAIL) {
		eggPos[0] = SDL_rand(GRID_X);
		eggPos[1] = SDL_rand(GRID_Y);
	}
	grid[eggPos[0]][eggPos[1]].state = EGG;
	grid[eggPos[0]][eggPos[1]].lifespan = -1;
}

void pause()
{
	if (gamestate == IN_PROGRESS) {
		gamestate = PAUSED;
	}
	else if (gamestate == PAUSED) {
		gamestate = IN_PROGRESS;
	}
}

void resetGame()
{
	/* reset all variables except highScore */
	gamestate = IN_PROGRESS;
	direction = NONE;
	speed = 0.5f;
	headPos[0] = GRID_X / 2;
	headPos[1] = GRID_Y / 2;
	score = 0;

	int i, j;
	for (i=0; i<GRID_X; i++) {
		for (j=0; j<GRID_Y; j++) {
			grid[i][j].state = EMPTY; // empty all points
			grid[i][j].lifespan = score;
		}
	}

	grid[headPos[0]][headPos[1]].state = HEAD;	// set initial head point
	grid[headPos[0]][headPos[1]].lifespan = -1;	// set initial head point

	generateNewEgg();				// gen initial egg
}


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	/* Initialize game variables */
	int i, j;
	for (i=0; i<GRID_X; i++) {
		for (j=0; j<GRID_Y; j++) {
			grid[i][j].state = EMPTY; // empty all points
			grid[i][j].lifespan = score;
		}
	}

	grid[headPos[0]][headPos[1]].state = HEAD;	// set initial head point
	grid[headPos[0]][headPos[1]].lifespan = -1;	// set initial head point

	generateNewEgg();				// gen initial egg

	/* SDL Initialization */
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("csnake", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_SetRenderLogicalPresentation(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

#ifdef DEBUG
	SDL_Log("App initialized");
#endif
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	switch (event->type) {
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;
		case SDL_EVENT_KEY_DOWN: {
			SDL_Scancode scancode = event->key.scancode;
			if (scancode == SDL_SCANCODE_W || scancode==SDL_SCANCODE_UP) direction = UP;
			if (scancode == SDL_SCANCODE_A || scancode==SDL_SCANCODE_LEFT) direction = LEFT;
			if (scancode == SDL_SCANCODE_S || scancode==SDL_SCANCODE_DOWN) direction = DOWN;
			if (scancode == SDL_SCANCODE_D || scancode==SDL_SCANCODE_RIGHT) direction = RIGHT;
			if (scancode == SDL_SCANCODE_P) pause();
			break;
		}
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	int i, j;
	// add event when snake eats egg. Snake grows. Speed increases.
	// add event when head collides with body. Game ends. All states, pos, speed reset.
	currentTime = SDL_GetTicks();
	if (currentTime == lastTime + (Uint64)(1000*speed)) {
		if (gamestate == IN_PROGRESS) {
			// determine next position in grid
			int nextPos[2];
			nextPos[0] = headPos[0];
			nextPos[1] = headPos[1];
			switch (direction) {
				case UP:
					nextPos[1]--;
					if (nextPos[1] < 0) nextPos[1] = GRID_Y-1;
					break;
				case DOWN:
					nextPos[1]++;
					if (nextPos[1] > GRID_Y-1) nextPos[1] = 0;
					break;
				case LEFT:
					nextPos[0]--;
					if (nextPos[0] < 0) nextPos[0] = GRID_X-1;
					break;
				case RIGHT:
					nextPos[0]++;
					if (nextPos[0] > GRID_X-1) nextPos[0] = 0;
					break;
			}

			// Decrement tail unit lifespans. destroy tail end once lifespan runs out (based on score)
			for (i=0; i<GRID_X; i++) {
				for (j=0; j<GRID_Y; j++) {
					if (grid[i][j].state == TAIL) {
						grid[i][j].lifespan--;
						if (grid[i][j].lifespan <= 0) {
							grid[i][j].state = EMPTY;
							grid[i][j].lifespan = -1;
						}
					}
				}
			}


			// detect collisions
			switch (grid[nextPos[0]][nextPos[1]].state) {
				case TAIL:
					resetGame();
					break;
				case EGG:
					score++;
					if (score > highScore) highScore = score;
					SDL_snprintf(scoreText, 16, "SCORE: %u", score);
					SDL_snprintf(highScoreText, 16, "HIGH: %u", highScore);
					speed*=0.98f;
					generateNewEgg();
				case EMPTY:
					grid[headPos[0]][headPos[1]].state = TAIL;
					grid[headPos[0]][headPos[1]].lifespan = score;
					headPos[0] = nextPos[0];
					headPos[1] = nextPos[1];
					grid[headPos[0]][headPos[1]].state = HEAD;
					grid[headPos[0]][headPos[1]].lifespan = -1;
					break;
			}
		}
#ifdef DEBUG
		SDL_Log("Direction: %i\nSpeed: %f", direction, speed);
#endif
		lastTime = currentTime;
	}

	SDL_SetRenderDrawColor(renderer, 0x22, 0x1F, 0x23, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	// Draw egg, head, and tail tiles
	for (i=0; i<GRID_X; i++) {
		for (j=0; j<GRID_Y; j++) {
			switch (grid[i][j].state) {
				case HEAD: {
					SDL_SetRenderDrawColor(renderer, 0xBB, 0xCC, 0xDE, SDL_ALPHA_OPAQUE);
					SDL_FRect headRect = { i*POINT_WIDTH+2, j*POINT_HEIGHT+2, POINT_WIDTH-4, POINT_HEIGHT-4 };
					SDL_RenderFillRect(renderer, &headRect);
					break;
				}
				case TAIL: {
					SDL_SetRenderDrawColor(renderer, 0x85, 0x95, 0xA5, SDL_ALPHA_OPAQUE);
					SDL_FRect tailRect = { i*POINT_WIDTH+4, j*POINT_HEIGHT+4, POINT_WIDTH-8, POINT_HEIGHT-8 };
					SDL_RenderFillRect(renderer, &tailRect);
					break;
			 	}
				case EGG: { SDL_SetRenderDrawColor(renderer, 0xEA, 0xCE, 0xCA, SDL_ALPHA_OPAQUE);
					SDL_FRect tailRect = { i*POINT_WIDTH+6, j*POINT_HEIGHT+6, POINT_WIDTH-12, POINT_HEIGHT-12 };
					SDL_RenderFillRect(renderer, &tailRect);
					break;
				}
			}
		}
	}

	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180);
	SDL_RenderDebugText(renderer, 10.0f, 10.0f, scoreText);
	SDL_RenderDebugText(renderer, 10.0f, 20.0f, highScoreText);

	SDL_RenderPresent(renderer);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
#ifdef DEBUG
	SDL_Log("App executed");
#endif
}
