#pragma once

#include <stdlib.h>

#include "renderer.h"
#include "utils.h"

typedef struct Stage Stage;

typedef void (*stage_init_func)(Stage *);
typedef void (*stage_enter_func)(Stage *, int);
typedef void (*stage_leave_func)(Stage *, int);
typedef void (*stage_render_func)(Stage *, double);
typedef void (*stage_update_func)(Stage *, double);

enum StageType {
	STAGE_GAMEPLAY = 0,
	STAGE_MENU,
	STAGE_EDITOR,
	STAGE_LOGO,
	_STAGE_COUNT
};

enum TransitionType {
	TRANSITION_NONE = 0,
	_TRANSITION_COUNT
};

struct StageManager {
	Stage **stages;
	Stage *active_stage;
	int max_stage_id;
};

struct Stage {
	int id;
	stage_init_func init;
	stage_enter_func enter;
	stage_update_func update;
	stage_render_func render;
	stage_leave_func leave;
};

void stage_manager_add(int stage_size, int id, stage_init_func i_func, stage_enter_func e_func, stage_leave_func l_func, stage_update_func u_func, stage_render_func r_func);
void stage_manager_enter(int id, int transition, double time);
void stage_manager_dispose();
