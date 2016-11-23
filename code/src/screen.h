#pragma once

#include "utils.h"
#include "renderer.h"

struct Screen {
	int width;
	int height;
	void *window;
};

#define B_LEFT 0
#define B_RIGHT 1
#define B_UP 2
#define B_DOWN 3
#define B_JUMP 4
#define B_PUNCH 5
#define B_PICKUP 6
#define B_SKILL 7
#define B_PAUSE 8
#define B_EMOTE1 9
#define B_EMOTE2 10
#define B_EMOTE3 11
#define B_EMOTE4 12
#define B_COUNT 13

//code for keyboard refer to glfw documentation
//code for joystick <100 for buttons < 200 for +axis < 300 for -axis
struct PlayerInput {
	int id;
	short code[B_COUNT];
	char state[B_COUNT];
	char pressed[B_COUNT];
	char released[B_COUNT];
};

struct Input {
	int count;
	PlayerInput player[4];
};

void init(Screen *screen);

void dispose(Screen *screen);

void update(Screen *screen);

int should_close(Screen *screen);

void clear(Screen *screen);

void set_clear_color(Screen *screen, Color *color);

double get_time();

void init(Input *input, Screen *screen);

void update(Input *input, Screen *screen);

int state_anyone(Input *input, char button);

int state(Input *input, char button, char player);