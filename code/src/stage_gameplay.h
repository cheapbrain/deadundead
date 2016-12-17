#pragma once

#include "utils.h"
#include "stage.h"
#include "Entities.h"


struct StageGameplay {
	Stage super;
	Shader *shader;
	World world;
};

void stage_gameplay_init(Stage *stage);

void stage_gameplay_enter(Stage *stage, int previous_stage_id);

void stage_gameplay_leave(Stage *stage, int next_stage_id);

void stage_gameplay_update(Stage *stage, double delta);

void stage_gameplay_render(Stage *stage, double delta);