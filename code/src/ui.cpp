#include <malloc.h>
#include "ui.h"
#include "entities.h"

Texture* textures[3];
void init_textures() {
	textures[0] = load_texture("../images/platform.png");
	textures[1] = load_texture("../images/sofa.png");
	textures[2] = load_texture("../images/torch.png");
}

void draw_label(Label* l, float x, float y, SpriteRenderer* s, Font* f) {
	draw(s, f, l->text, x + l->x, y + l->y);
}

void draw_arrow(Arrow* a, float x, float y, SpriteRenderer* s) {
	draw(s, a->t, x + a->x, y + a->y, a->w, a->h);
}

void draw_window(Window* w, SpriteRenderer* s, Font* f) {
	draw(s, w->t, w->x, w->y, w->w, w->h);
	for (int i = 0; i < w->label_n; i++) {
		draw_label(w->labels[i], w->x, w->y, s, f);
	}
	for (int i = 0; i < w->arrow_n; i++) {
		draw_arrow(w->arrows[i], w->x, w->y, s);
	}
}

void draw_palette(Palette* p, SpriteRenderer* s) {
	if (p->visibility) {
		draw(s, p->t, p->x, p->y, p->w, p->h);
		for (int j = 0; j < p->items_n; j++) {
			Item* i = p->items[j];
			draw(s, i->t, p->x+i->x, p->y+i->y, i->w, i->h);
		}
	}
}

void draw_map(Map* m, SpriteRenderer* s) {
	if (m->visibility) {
		draw(s, m->t, m->x, m->y, m->w, m->h);
		float scale_w=m->w/m->map_w, scale_h=m->h/m->map_h;
		for (int j = 0; j < m->items_n; j++) {
			Item* i = m->items[j];
			draw(s, i->t, i->x*scale_w, i->y*scale_h, 
				i->w*scale_w, i->h*scale_h);
		}
	}
}


Label* new_label(char* text) {
	return new_label(1.f, 1.f, text);
}
Label* new_label(float x, float y, char* text) {
	Label* l = (Label*)malloc(sizeof(Label));
	l->x = x;
	l->y = y;
	l->text = text;
	return l;
}
Arrow* new_arrow() {
	return new_arrow(1.f, 1.f, 1.f, 1.f);
}
Arrow* new_arrow(float x, float y, float w, float h) {
	Arrow* a = (Arrow*)malloc(sizeof(Arrow));
	a->x = x;
	a->y = y;
	a->w = w;
	a->h = h;
	a->t = load_texture("../images/test.png");
	return a;
}

Window* new_window() {
	return new_window(1.f,1.f,5.f,5.f);
}

Window* new_window(float x, float y, float w, float h) {
	Window* window = (Window*)malloc(sizeof(Window));
	window->arrow_n = 0;
	window->label_n = 0;
	window->x = x;
	window->y = y;
	window->h = h;
	window->w = w;
	window->t = load_texture("../images/window.png");
	return window;
}

Item* new_item() {
	return new_item(0);
}
Item* new_item(int id, float x, float y, float w, float h) {
	Item* i = new_item(id);
	i->x = x;
	i->y = y;
	i->w = w;
	i->h = h;
	return i;
}
Item* new_item(int id) {
	if (id > -1) {
		Item* i = (Item*)malloc(sizeof(Item));
		i->id = id;
		i->t = textures[i->id];
		return i;
	}
	else return NULL;
}

Palette* new_palette() {
	return new_palette(0.f, 0.f, 3.f, 10.f);
}
Palette* new_palette(float x, float y, float w, float h) {
	Palette* p = (Palette*)malloc(sizeof(Palette));
	p->x = x;
	p->y = y;
	p->h = h;
	p->w = w;
	p->visibility = 1;
	p->items_n = 0;
	p->t = load_texture("../images/window.png");
	return p;
}
Item** new_item_list(Palette* p) {
	Item** items = (Item**)malloc(sizeof(Item*)*p->items_n);
	float scale = .7f;
	items[0] = new_item(0, p->w / 2.f - scale / 2 * 1, 1*scale, 1 * scale, 1 * scale);
	items[1] = new_item(1, p->w / 2.f - scale / 2 * 2, 2*scale, 2 * scale, 1 * scale);
	items[2] = new_item(2, p->w / 2.f - scale / 2 * 1, 3*scale, 1 * scale, 1 * scale);
	return items;
}

Map* new_map() {
	return new_map(0.f, 0.f, 16.f, 9.f);
}
Map* new_map(float x, float y, float w, float h) {
	Map* m = (Map*)malloc(sizeof(Map));
	m->x = x;
	m->y = y;
	m->h = h;
	m->w = w;
	m->visibility = 1;
	m->map_h = 18;
	m->map_w = 32;
	m->items_n = 0;
	m->t = load_texture("../images/white.png");
	m->items = (Item**)malloc(sizeof(Item*)*m->map_h*m->map_w);
	return m;
}

int click_palette(Palette* p, float x, float y, Item** item) {
	if (p->visibility && x > p->x && x < p->x + p->w) {
		for (int j = 0; j < p->items_n; j++) {
			Item* i = p->items[j];
			if (x > p->x + i->x && x<p->x + i->x + i->w && y>p->y + i->y && y < p->y + i->y + i->h) {
				*item = new_item(i->id,0,0,i->w/.7f,i->h/.7f);
			}
		}
		return 1;
	}
	else return 0;
}

float round(float a) {
	return (float)(int)(a);
}

int click_map(Map** map, float x, float y, Item* item, int button) {
	Map* m = *map;
	x *= m->map_w / (float)m->w;
	y *= m->map_h / (float)m->h;
	int b = 0, id=-1;
	for (int j = 0; j < m->items_n; j++) {
		Item* i = m->items[j];
		if (x > i->x && x<i->x + i->w && y>+i->y && y < i->y + i->h) {
			if (button == 0 && item!=NULL) 
				m->items[j] = new_item(item->id, round(x), round(y), item->w, item->h);
			if (button == 1)
				id = j;
			b = 1;
		}
	}
	if (button==0 && !b && item!=NULL) {
		m->items[m->items_n] = new_item(item->id, round(x), round(y), item->w, item->h);
		m->items_n++;
	}
	if (id>0) {
		for (int j = id; j < m->items_n; j++)
			m->items[j] = m->items[j + 1];
		m->items[m->items_n] = NULL;
		m->items_n--;
	}
	return 1;
}


void save_map(Map *m) {
	FILE *f=fopen("map.dat", "wb");
	for (int j = 0; j < m->items_n; j++) {
		Item* t = m->items[j];
		fwrite(new_entity(t->id,t->x,t->y,t->w,t->h), sizeof(Entity), 1, f);
	}
	fclose(f);
}
void load_map(Map** map) {
	Map* m = *map;
	free(m);
	m = new_map();
	FILE *f = fopen("map.dat", "rb");
	Entity* e = (Entity*)malloc(sizeof(Entity));
	while (fread(e, sizeof(Entity), 1, f)) {
		m->items[m->items_n] = new_item(e->type, e->x, e->y, e->w, e->h);
		m->items_n++;
	}
	fclose(f);
}