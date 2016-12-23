#pragma once

#include "utils.h"
#include "stage.h"
#include "Entities.h"


struct StageEditor {
	Stage super;
	Shader *shader;
	World world;
};

void stage_editor_init(Stage *stage);

void stage_editor_enter(Stage *stage, int previous_stage_id);

void stage_editor_leave(Stage *stage, int next_stage_id);

void stage_editor_update(Stage *stage, double delta);

void stage_editor_render(Stage *stage, double delta);