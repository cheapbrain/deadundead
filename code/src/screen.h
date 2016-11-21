#pragma once

#include "utils.h"
#include "renderer.h"

struct Screen {
	int width;
	int height;
	void *window;
};

void init(Screen *screen);

void dispose(Screen *screen);

void update(Screen *screen);

int should_close(Screen *screen);

void clear(Screen *screen);

void set_clear_color(Screen *screen, Color *color);

double get_time();
