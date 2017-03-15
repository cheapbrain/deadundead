#pragma once

#include "utils.h"
#include "stage.h"
#include "Entities.h"

struct Field {
	char **options;
	int active;
	int n;
};
struct FieldList {
	Field *fields;
	int selected;
	int n;
};
struct StageMenu {
	Stage super;
	Shader *shader;
	FieldList list;
	Mat3 *m;
	float x, y;
	Texture *border;
};

Field new_field(char *options);
FieldList new_field_list(char *options);

void stage_menu_init(Stage *stage);

void stage_menu_enter(Stage *stage, int previous_stage_id);

void stage_menu_leave(Stage *stage, int next_stage_id);

void stage_menu_update(Stage *stage, double delta);

void stage_menu_render(Stage *stage, double delta);