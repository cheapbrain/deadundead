#include "stage.h"
#include "screen.h"

void stage_manager_add(int stage_size, int id, stage_init_func init_func, stage_enter_func enter_func, stage_leave_func leave_func, stage_update_func update_func, stage_render_func render_func) {
	Stage *stage = (Stage *)malloc(stage_size);
	stage->id = id;
	stage->init = init_func;
	stage->enter = enter_func;
	stage->update = update_func;
	stage->render = render_func;
	stage->leave = leave_func;
	stage->init(stage);
	game.stage_manager.stages[id] = stage;
}

void stage_manager_enter(int id, int transition, double time) {
	Stage *active_stage = game.stage_manager.active_stage;
	Stage *next_stage = game.stage_manager.stages[id];
	if (active_stage == NULL) {
		next_stage->enter(next_stage, -1);
		game.stage_manager.active_stage = next_stage;
	} else {
		active_stage->leave(active_stage, next_stage->id);
		next_stage->enter(next_stage, active_stage->id);
		game.stage_manager.active_stage = next_stage;
	}
}

void stage_manager_dispose() {
	
}
