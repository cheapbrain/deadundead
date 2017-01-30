#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>

#include "stage_editor.h"
#include "stage_gameplay.h"
#include "renderer.h"
#include "screen.h"
#include "Entities.h"

float mouse_x, mouse_y;
int mouse_left, mouse_middle, mouse_right;
int changed_left, changed_middle, changed_right;
int show_menu;
int spacebar_state;
int selected;
Texture *bg;
SpriterInstance instance;


Entity null_entity = {0};

#define PARSER_KEY 0
#define PARSER_SEPARATOR 1
#define PARSER_VALUE 2
#define PARSER_SKIP 3

static int _hash(const unsigned char *key, int len) {
  const unsigned int m = 0x5bd1e995;
  const int r = 24;
  unsigned int h = 19991 ^ len;
  while(len >= 4) {
    unsigned int k = *(unsigned int*)key;
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
    key += 4;
    len -= 4;
  }
  switch(len) {
  case 3: h ^= key[2] << 16;
  case 2: h ^= key[1] << 8;
  case 1: h ^= key[0]; h *= m;
  };
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
} 

EntityNode *_get_recipe(EntityArray *entity_array, char *name) {
	int hash = _hash((const unsigned char *)name, strlen(name)) & entity_array->mask;
	EntityNode **recipe = &entity_array->table[hash];
	for(;;) {
		if (*recipe == NULL) {
			*recipe = (EntityNode *)malloc(sizeof(EntityNode));
			(*recipe)->name = name;
			(*recipe)->next = NULL;
			break;
		} else if (!strcmp(name, (*recipe)->name)) {
			break;
		} else {
			recipe = &((*recipe)->next);
		}
	}
	return *recipe;
}

void load_palette(EntityArray *palette) {
	if (palette->names != NULL) {
		for (int i = 0; i < palette->count; i++) {
			free(palette->names[i]);
		}
		free(palette->entities);
		free(palette->names);
	}
	int capacity = 10;
	palette->entities = (Entity *)malloc(sizeof(Entity) * capacity);
	palette->names = (char **)malloc(sizeof(char *) * capacity);
	palette->capacity = capacity;
	palette->count = 0;

	FILE *fp = fopen("../data/entities.dat", "rb");
	char *buffer = (char *)malloc(8192);
	int i = 0;
	int buffer_size = 0;

	char key[100];
	int key_length = 0;
	char value[1000];
	int value_length = 0;
	int parser_state = PARSER_KEY;

	for (;;) {
		if (i == buffer_size) {
			buffer_size = fread(buffer, 1, 8192, fp);
			if (buffer_size == 0) break;
			i = 0;
		}
		char c = buffer[i++];

		if (c == '\n') {

			if (key_length > 0 && value_length > 0) {
				key[key_length] = '\0';
				value[value_length] = '\0';
				if (!strcmp(key, "name")) {
					if (palette->count == palette->capacity) {
						palette->capacity *= 2;
						palette->entities = (Entity *)realloc(palette->entities, sizeof(Entity) * palette->capacity);
						palette->names = (char **)realloc(palette->names, sizeof(char *) * palette->capacity);
					}
					char *name = (char *)malloc(value_length + 1);
					memcpy(name, value, value_length + 1);
					palette->names[palette->count] = name;
					palette->entities[palette->count] = null_entity;
					palette->count++;

				} else if (!strcmp(key, "texture")) {
					palette->entities[palette->count-1].texture = load_texture(value);
				} else if (!strcmp(key, "x")) {
					palette->entities[palette->count-1].x = (float)atof(value);
				} else if (!strcmp(key, "y")) {
					palette->entities[palette->count-1].y = (float)atof(value);
				} else if (!strcmp(key, "w")) {
					palette->entities[palette->count-1].width = (float)atof(value);
				} else if (!strcmp(key, "h")) {
					palette->entities[palette->count-1].height = (float)atof(value);
				} else if (!strcmp(key, "h")) {
					palette->entities[palette->count-1].height = (float)atof(value);
				} else if (!strcmp(key, "flags")) {
					int tag = 0;
					char *word = value;
					char *word_end = value + 1;
					int parsing = 1;
					while (parsing) {
						switch(*word_end) {
							case '\0':
							parsing = 0;
							case ',':
							*word_end = '\0';
							if (!strcmp(word, "UPDATE")) tag = tag | UPDATE;
							else if (!strcmp(word, "RENDER")) tag = tag | RENDER;
							else if (!strcmp(word, "RENDER_BACK")) tag = tag | RENDER_BACK;
							else if (!strcmp(word, "RENDER_FRONT")) tag = tag | RENDER_FRONT;
							else if (!strcmp(word, "DYNAMIC_COLLIDE")) tag = tag | DYNAMIC_COLLIDE;
							else if (!strcmp(word, "STATIC_COLLIDE")) tag = tag | STATIC_COLLIDE;
							else if (!strcmp(word, "HITTABLE")) tag = tag | HITTABLE;
							else if (!strcmp(word, "ACTIVE_EVENT")) tag = tag | ACTIVE_EVENT;
							else if (!strcmp(word, "PASSIVE_EVENT")) tag = tag | PASSIVE_EVENT;
							word = word_end + 1;
						}
						word_end++;
					}
					palette->entities[palette->count-1].tag = tag;

				} else if (!strcmp(key, "player_id")) {
					palette->entities[palette->count-1].player_id = (int)atoi(value);
				} else if (!strcmp(key, "bounce")) {
					palette->entities[palette->count-1].bounce_coeff = (float)atof(value);
				} else if (!strcmp(key, "health")) {
					palette->entities[palette->count-1].height = (float)atof(value);
				} else if (!strcmp(key, "update")) {
					for (int i = 0; i < _UPDATE_FUNCTION_COUNT; i++) {
						if (!strcmp(value, update_func_names[i])) {
							palette->entities[palette->count-1].update = update_functions[i];
							break;
						}
					}
				} else if (!strcmp(key, "render")) {
					for (int i = 0; i < _RENDER_FUNCTION_COUNT; i++) {
						if (!strcmp(value, render_func_names[i])) {
							palette->entities[palette->count-1].render = render_functions[i];
							break;
						}
					}
				} else if (!strcmp(key, "collide")) {

				} else if (!strcmp(key, "enter")) {

				} else if (!strcmp(key, "hit")) {

				} else if (!strcmp(key, "interact")) {

				}
			}

			key_length = 0;
			value_length = 0;
			parser_state = PARSER_KEY;
			continue;
		}

		if (c == '#') {
			parser_state = PARSER_SKIP;
			continue;
		}

		switch(parser_state) {
			case PARSER_KEY:
				if (c > 32 && c != 61) {
					key[key_length++] = c;
				} else {
					parser_state = PARSER_SEPARATOR;
				}
				break;
			case PARSER_SEPARATOR:
				if (c > 32 && c != 61) {
					value[value_length++] = c;
					parser_state = PARSER_VALUE;
				}
				break;
			case PARSER_VALUE:
				if (c > 32) {
					value[value_length++] = c;
				}
				break;
			default:
				break;
		}
	}

	free(buffer);
	fclose(fp);

	int table_size = 2;
	while (table_size <= palette->count) table_size *= 2;
	palette->mask = table_size - 1;

	palette->table = (EntityNode **)calloc(table_size, sizeof (EntityNode *));

	for (int i = 0; i < palette->count; i++) {
		EntityNode *recipe = _get_recipe(palette, palette->names[i]);
		recipe->entity = palette->entities + i;
	}

	selected = 0;
}

void save_map(char *path, EditorWorld *map) {
	char buffer[1000];
	FILE *fp = fopen(path, "wb");

	for (int i = 0; i < map->count; i++) {
		EditorEntity *e = map->entities + i;
		sprintf(buffer, 
				"name = %s\n"
				"x = %f\n"
				"y = %f\n\n",
				e->name, e->x, e->y);
		fwrite(buffer, 1, strlen(buffer), fp);
	}

	fclose(fp);
}

EditorEntity *_add_editor_entity(EntityArray *palette, EditorWorld *map, char *name) {
	if (map->count == map->capacity) {
		map->capacity *= 2;
		map->entities = (EditorEntity *)realloc(map->entities, sizeof(EditorEntity) * map->capacity);
	}
	map->entities[map->count].name = name;
	return map->entities + map->count++;
}

void load_map(char *path, EntityArray *palette, EditorWorld *map) {
	map->count = 0;

	FILE *fp = fopen("../data/map.dat", "rb");
	char *buffer = (char *)malloc(8192);
	int i = 0;
	int buffer_size = 0;

	char key[100];
	int key_length = 0;
	char value[100];
	int value_length = 0;
	int parser_state = PARSER_KEY;

	EditorEntity *ee = NULL;

	for (;;) {
		if (i == buffer_size) {
			buffer_size = fread(buffer, 1, 8192, fp);
			if (buffer_size == 0) break;
			i = 0;
		}
		char c = buffer[i++];

		if (c == '\n') {
			if (key_length > 0 && value_length > 0) {
				key[key_length] = '\0';
				value[value_length] = '\0';
				if (!strcmp(key, "name")) {
					EntityNode *en = _get_recipe(palette, value);
					ee = _add_editor_entity(palette, map, en->name);
					ee->original = en->entity;
				} else if (!strcmp(key, "x")) {
					ee->x = (float)atof(value);
				} else if (!strcmp(key, "y")) {
					ee->y = (float)atof(value);
				}
			}

			key_length = 0;
			value_length = 0;
			parser_state = PARSER_KEY;
			continue;
		}

		if (c == '#') {
			parser_state = PARSER_SKIP;
			continue;
		}

		switch(parser_state) {
			case PARSER_KEY:
				if (c > 32 && c != 61) {
					key[key_length++] = c;
				} else {
					parser_state = PARSER_SEPARATOR;
				}
				break;
			case PARSER_SEPARATOR:
				if (c > 32 && c != 61) {
					value[value_length++] = c;
					parser_state = PARSER_VALUE;
				}
				break;
			case PARSER_VALUE:
				if (c > 32) {
					value[value_length++] = c;
				}
				break;
			default:
				break;
		}
	}

	free(buffer);
	fclose(fp);
}

void load_world(char *path, EntityArray *palette, World *world) {
	world->entity_count = 0;
	for (int i = 0; i <_LIST_COUNT; i++) {
		world->lists[i].count = 0;
	}

	EditorWorld ew;
	ew.entities = (EditorEntity *)malloc(sizeof(EditorEntity) * 10);
	ew.capacity = 10;
	ew.count = 0;

	load_map(path, palette, &ew);

	for (int i = 0; i < ew.count; i++) {
		EditorEntity *ee = ew.entities + i;
		Entity *t_e = ee->original;
		Entity *e = world_new_entity(world, t_e);
		e->x += ee->x;
		e->y += ee->y;
	}
	

	free(ew.entities);
}

EntityArray palette;
EditorWorld map;

void fill_rect(float x, float y, float w, float h, float r, float g, float b, float a) {
	set_color(&game.renderer, r, g, b, a);
	draw(&game.renderer, game.renderer.default_texture, x, y, w, h);
}

int button(char *text, float x, float y, float w, float h, int focus) {
	set_shader(&game.renderer, game.renderer.default_shader);
	int inside = mouse_x >=x && mouse_x <= x + w && mouse_y >= y && mouse_y <= y + h;
	set_color(&game.renderer, 1.f, .4f, .1f, 1);
	if (focus)
		draw(&game.renderer, game.renderer.default_texture, x, y, w, h);
	set_color(&game.renderer, .2f, .2f, .2f, 1);
	if (inside) set_color(&game.renderer, .3f, .35f, .4f, 1);
	if (mouse_left && inside) set_color(&game.renderer, .2f, .4f, .6f, 1);
	draw(&game.renderer, game.renderer.default_texture, x + 0.026f, y + 0.026f, w - 0.052f, h - 0.052f);
	set_color(&game.renderer, &white);
	game.debug_font->drawh = .25f;
	draw(&game.renderer, game.debug_font, text, x + 0.1f, y + h - 0.04f);
	return !mouse_left && changed_left && inside;
}

void stage_editor_init(Stage *stage) {
	SpriterCharacter *sc = load_spriter_character("../animazioni/Ibi/Ibi.scon");

	palette.entities = NULL;
	palette.names = NULL;
	map.entities = (EditorEntity *)malloc(sizeof(EditorEntity) * 10);
	map.capacity = 10;
	map.count = 0;
	bg = load_texture("../images/bg.png");
	load_palette(&palette);

	instance.character = sc;
	instance.x = 4;
	instance.y = 1;
	instance.scale_x = 1.f/100;
	instance.scale_y = 1.f/100;
	instance.active_animation = 7;
	instance.animation_speed = 1000;
	instance.animation_time = 0;
}

void stage_editor_enter(Stage *stage, int previous_stage_id) {
	mouse_left = 0;
	mouse_right = 0;
	mouse_middle = 0;
	show_menu = 1;
	selected = 0;
}

void stage_editor_leave(Stage *stage, int next_stage_id) {

}

void stage_editor_update(Stage *stage, double delta) {
	GLFWwindow *window = (GLFWwindow *)game.window;
	int temp_left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int temp_middle = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	int temp_right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	changed_left = mouse_left != temp_left;
	changed_middle = mouse_middle != temp_middle;
	changed_right = mouse_right != temp_right;
	mouse_left = temp_left;
	mouse_middle = temp_middle;
	mouse_right = temp_right;
	double raw_x, raw_y;

	glfwGetCursorPos(window, &raw_x, &raw_y);
	mouse_x = (float)raw_x - game.x_offset;
	if (mouse_x < 0) mouse_x = 0;
	if (mouse_x >= game.width) mouse_x = (float)game.width - 1;
	mouse_x = mouse_x / (float)game.width * 17.f;

	mouse_y = (float)raw_y - game.y_offset;
	if (mouse_y < 0) mouse_y = 0;
	if (mouse_y >= game.height) mouse_y = (float)game.height - 1;
	mouse_y = 10.f - mouse_y / (float)game.height * 10.f;

	int temp_spacebar = glfwGetKey(window, GLFW_KEY_SPACE);
	if (temp_spacebar != spacebar_state && spacebar_state)
		show_menu = !show_menu;
	spacebar_state = temp_spacebar;
}

void stage_editor_render(Stage *stage, double delta) {
	set_shader(&game.renderer, game.renderer.default_shader);
	if (show_menu) {
		float menu_x = 5.0f;
		float menu_y = 9.5f;
		if (button("salva", menu_x += 1.5f, menu_y, 1.0f, .3f, 0)) {
			save_map("../data/map.dat", &map);
		}
		if (button("carica", menu_x += 1.5f, menu_y, 1.0f, .3f, 0)) {
			load_map("../data/map.dat", &palette, &map);
		}
		if (button("gioca", menu_x += 1.5f, menu_y, 1.0f, .3f, 0)) {
			StageGameplay *stage = (StageGameplay *)game.stage_manager.stages[STAGE_GAMEPLAY];
			load_world("../data/map.dat", &palette, &stage->world);
			stage_manager_enter(STAGE_GAMEPLAY, TRANSITION_NONE, 0);

		}
		float start_y = 9.f;
		float elem_x = .1f;
		float elem_y = start_y;
		for (int i = 0; i < palette.count; i++) {
			if (elem_y < .2f) {
				elem_y = start_y;
				elem_x += 2.f;
			}
			if (button(palette.names[i], elem_x, elem_y, 2.f, .3f, i == selected)) {
				selected = i;
			}

			elem_y -= .3f;
		}
	} else {
		if (mouse_left && changed_left) {
			EditorEntity *ee = _add_editor_entity(&palette, &map, palette.names[selected]);
			ee->x = (float)(int)mouse_x;
			ee->y = (float)(int)mouse_y;
			ee->original = palette.entities + selected;
		}
		if (mouse_right && changed_right) {
			int i = 0;
			while (i < map.count) {
				EditorEntity * ee = map.entities + i;
				Entity *e = ee->original;
				if (mouse_x >= e->x + ee->x && mouse_x <= e->x + ee->x + e->width && mouse_y >= e->y + ee->y && mouse_y <= e->y + ee->y + e->height) {
					if (i < map.count - 1) {
						map.entities[i] = map.entities[map.count - 1];
					}
					map.count--;
				} else i++;
			}
		}

		draw(&game.renderer, bg, 0, 0, 17, 10);

		for (int i = 0; i < map.count; i++) {
			EditorEntity * ee = map.entities + i;
				Entity *e = ee->original;
			draw(&game.renderer, e->texture, ee->x + e->x, ee->y + e->y, e->width, e->height);
		}

		float line_size = 0.026f;
		set_color(&game.renderer, 1.f, 0.f, 1.f, .5f);
		for (int x = 0; x <= 17; x++)
			draw(&game.renderer, game.renderer.default_texture, x - .013f, 0, line_size, 10);
		for (int y = 0; y <= 10; y++)
			draw(&game.renderer, game.renderer.default_texture, 0, y - .013f, 17, line_size);
	}

	if (button_released(B_EMOTE1, 0)) {
		instance.active_animation = (instance.active_animation + 1) % instance.character->animations.count;
	}

	set_shader(&game.renderer, game.renderer.default_shader);
	set_color(&game.renderer, &white);
	instance.animation_time += (float)(instance.animation_speed * delta);
	draw(&game.renderer, &instance);

	char text[100];
	sprintf(text, "e: %d", map.count);
	game.debug_font->drawh = .4f;
	set_color(&game.renderer, &white);
	draw(&game.renderer, game.debug_font, text, 0.1f, 0.5f);

	game.debug_font->drawh = .2f;
	set_color(&game.renderer, &white);
	draw(&game.renderer, game.debug_font, log_buff, 0.1f, 9.f);
}