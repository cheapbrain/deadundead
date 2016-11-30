#pragma once

typedef struct Stage Stage;

typedef void (*stage_init_func)(Stage *);
typedef void (*stage_enter_func)(Stage *, int);
typedef void (*stage_render_func)(Stage *, double);
typedef void (*stage_update_func)(Stage *, double);
typedef void (*stage_leave_func)(Stage *);


struct StageManager {
	Stage *active_stage;
	Stage *stages;
	int max_stage_id;
};

struct Stage {
	int stage_id;
	void *data;

	stage_init_func init;
	stage_enter_func enter;
	stage_update_func update;
	stage_render_func render;
	stage_leave_func leave;
};

void add_stage(int id, stage_init_func i_func, stage_enter_func e_func, stage_update_func u_func, stage_render_func r_func, stage_leave_func l_func);
void enter_stage(int id, int transition, double time);
void exit_stage();
