#pragma once
#include "renderer.h"
#include "utils.h"

extern Texture* textures[3];
void init_textures();

struct Label {
	float x, y;
	int visible;
	char* text;
};
Label* new_label(char* text);
Label* new_label(float x, float y, char* text);
void draw_label(Label* label, float x_window, float y_window, SpriteRenderer* renderer, Font* font);

struct Arrow {
	float x, y, w, h;
	int visible, enabled;
	Texture *t;
	int orientation;
};
Arrow* new_arrow();
Arrow* new_arrow(float x, float y, float w, float h);
void draw_arrow(Arrow* arrow, float x_window, float y_window, SpriteRenderer* renderer);

struct Window {
	float x, y, w, h;
	Texture *t;
	int visibility;
	Label **labels;
	int label_n;
	Arrow **arrows;
	int arrow_n;
};
Window* new_window();
Window* new_window(float x, float y, float w, float h);
void draw_window(Window* window, SpriteRenderer* renderer, Font* font);

struct Item {
	float x, y, w, h;
	Texture* t;
	int id;
};
Item* new_item();
Item* new_item(int id);
Item* new_item(int id, float x, float y, float w, float h);

struct Palette {
	float x, y, w, h;
	Texture *t;
	int visibility;
	int items_n;
	Item** items;

};
Palette* new_palette();
Palette* new_palette(float x, float y, float w, float h);
void draw_palette(Palette* palette, SpriteRenderer* renderer);
Item** new_item_list(Palette* palette);
int click_palette(Palette* palette, float x, float y, Item** i);

struct Map {
	float x, y, w, h;
	Texture *t;
	int visibility;
	int map_h, map_w, items_n;
	Item** items;
};
Map* new_map();
Map* new_map(float x, float y, float w, float h);
void draw_map(Map* map, SpriteRenderer* renderer);
int click_map(Map** map, float x, float y, Item* i, int button);
void save_map(Map* map);
void load_map(Map** map);