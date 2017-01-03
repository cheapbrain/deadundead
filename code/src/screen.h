#pragma once

#include "utils.h"
#include "renderer.h"
#include "stage.h"

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

#define INPUT_PLAYER0 0
#define INPUT_PLAYER1 1
#define INPUT_PLAYER2 2
#define INPUT_PLAYER3 3

//code for keyboard refer to glfw documentation
//code for joystick <100 for buttons < 200 for +axis < 300 for -axis
struct PlayerInput {
	char id;
	short code[B_COUNT];
	char state[B_COUNT];
	char pressed[B_COUNT];
	char released[B_COUNT];
};

struct Input {
	int count;
	PlayerInput player[4];
};

enum AssetType {
	TEXTURE = 0,
	SHADER,
	FONT,
	SPRITER_CHARACTER
};

struct Asset {
	AssetType type;
	char *name;
	void *data;
	Asset *next;
};

struct AssetManager {
	Asset **assets;
	int count;
};

struct Game {
	int window_width;
	int window_height;
	int x_offset;
	int y_offset;
	int width;
	int height;
	double time;
	double delta;
	void *window;
	Input input;
	StageManager stage_manager;
	SpriteRenderer renderer;
	AssetManager asset_manager;
	Font *debug_font;
};

extern Game game;

void game_init();

void game_dispose();

void game_update();

void game_close();

void game_clear_color(Color * color);

void game_clear_color(float r, float g, float b, float a);

void game_clear();

int game_should_close();

int button_state(char button, char player);

int button_pressed(char button, char player);

int button_released(char button, char player);
