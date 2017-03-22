#include <stdlib.h>
#include <string.h>

#include "stage_menu.h"
#include "screen.h"
#include "renderer.h"
#include "utils.h"



void stage_menu_init(Stage *stage) {
	StageMenu *menu = (StageMenu *)stage;
	menu->shader = load_shader("../data/shaders/default.vert", "../data/shaders/default.frag", SHADER_SPRITE);
	menu->m = (Mat3 *)malloc(sizeof(Mat3));
	orthographic(menu->m, 10, 0, 0);
	menu->x = 1.5f;
	menu->y = 8.f;
	menu->border = load_texture("../images/white.png");
}

void stage_menu_enter(Stage *stage, int previous_stage_id) {
	StageMenu *menu = (StageMenu *)stage;
}

void stage_menu_leave(Stage *stage, int next_stage_id) {
	
}

void stage_menu_update(Stage *stage, double delta) {
	StageMenu *menu = (StageMenu*)stage;
	FieldList *l = &menu->list;
	Field *f = &(l->fields[l->selected]);

	for (int i = 0; i < game.input.count; i++) {
		if (button_state(B_PAUSE, i)) game_close();
		if (button_released(B_LEFT, i)) f->active = (f->n + f->active - 1) % f->n;
		if (button_released(B_RIGHT, i)) f->active = (f->n + f->active + 1) % f->n;
		if (button_released(B_UP, i)) l->selected = (l->n + l->selected - 1) % l->n;
		if (button_released(B_DOWN, i)) l->selected = (l->n + l->selected + 1) % l->n;
	}
	
}

void stage_menu_render(Stage *stage, double delta) {
	StageMenu *menu = (StageMenu *)stage;
	set_shader(&game.renderer, menu->shader);

	set_matrix(&game.renderer, menu->m);

	game.debug_font->drawh = .5f;
	set_color(&game.renderer, &white);
	draw(&game.renderer, menu->border, menu->x, menu->y, .05f, -menu->list.n - .2f);
	draw(&game.renderer, menu->border, menu->x, menu->y, 5.f, .05f);
	draw(&game.renderer, menu->border, menu->x, menu->y - menu->list.n - .2f, 5.f, .05f);
	draw(&game.renderer, menu->border, menu->x + 4.95f, menu->y, .05f, -menu->list.n - .2f);
	draw(&game.renderer, menu->border, menu->x + .2f, menu->y - menu->list.selected - .3f, .3f, -.3f);
	for (int i = 0; i < menu->list.n; i++) {
		float x = menu->x + .7f;
		float y = menu->y - i - .2f;
		int active = menu->list.fields[i].active;
		draw(&game.renderer, game.debug_font, menu->list.fields[i].options[active], x, y);
	}

	flush(&game.renderer);
}

Field new_field(char *o) {
	Field f;
	f.active = 0;
	char* p=(char*)malloc(strlen(o));
	strcpy(p, o);
	int i = 1;
	for (unsigned int j = 0; j < strlen(o); j++)
		if (p[j] == ',') i++;
	f.options = (char**)malloc(sizeof(char*)*i);
	f.n = i;
	i = 0;
	char *t = strtok(p, ",");
	while (t) {
		f.options[i] = (char*)malloc(strlen(t));
		strcpy(f.options[i], t);
		i++;
		t = strtok(NULL, ",");
	}
	free(p);
	free(t);
	return f;
}

FieldList new_field_list(char *o) {
	FieldList f;
	f.selected = 0;
	char *p = (char*)malloc(strlen(o));
	strcpy(p, o);
	int i = 1;
	for (unsigned int j = 0; j < strlen(o); j++)
		if (p[j] == ';') i++;
	f.fields = (Field*)malloc(sizeof(Field)*i);
	f.n = i;
	i = 0;
	char **tmp = (char**)malloc(sizeof(char*)*i);
	char *t = strtok(p, ";");
	while (t) {
		tmp[i] = (char*)malloc(strlen(t));
		strcpy(tmp[i], t);
		i++;
		t = strtok(NULL, ";");
	}
	for (int i = 0; i < f.n; i++) {
		f.fields[i] = new_field(tmp[i]);
	}
	free(p);
	free(t);
	return f;
}