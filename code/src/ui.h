#pragma once

struct Label {
	float x, y;
	int visible;
	char* text;
};
Label* new_label(char*);
void draw_label(Label*, float, float, SpriteRenderer, Font*);

struct Arrow {
	float x, y, w, h;
	int visible, enabled;
	Texture *t;
	int orientation;
};
Arrow* new_arrow();
void draw_arrow(Arrow*, float, float, SpriteRenderer);

struct Window {
	float x, y, w, h;
	Texture *t;
	float opacity;
	Label **labels;
	int label_n;
	Arrow **arrows;
	int arrow_n;
};
Window* new_window();
void draw_window(Window*, SpriteRenderer, Font*);