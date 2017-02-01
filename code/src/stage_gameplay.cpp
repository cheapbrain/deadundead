#include <stdlib.h>
#include <string.h>

#include "stage_gameplay.h"
#include "screen.h"
#include "renderer.h"
#include "utils.h"

void stage_gameplay_init(Stage *stage) {
	log_buff[0] = 0;
	StageGameplay *gameplay = (StageGameplay *)stage;
	//test->texture = load_texture("D:/Download/GreyGuy/head/p_head_hurt.png");
	gameplay->shader = load_shader("../data/shaders/default.vert", "../data/shaders/default.frag", SHADER_SPRITE);
	
	gameplay->bg = load_texture("../images/bg.png");
	World *world = &gameplay->world;
	world_init(world, 100);
}

void stage_gameplay_enter(Stage *stage, int previous_stage_id) {
	
}

void stage_gameplay_leave(Stage *stage, int next_stage_id) {
	
}

void stage_gameplay_update(Stage *stage, double delta) {
	StageGameplay *gameplay = (StageGameplay *)stage;

	for (int i = 0; i < game.input.count; i++)
		if (button_state(B_PAUSE, i)) stage_manager_enter(STAGE_EDITOR, TRANSITION_NONE, 0);
	//->angle += (float)(gameplay->angle_speed * delta);
	world_update(&gameplay->world, delta);
}

void stage_gameplay_render(Stage *stage, double delta) {
	StageGameplay *gameplay = (StageGameplay *)stage;
	
	set_shader(&game.renderer, gameplay->shader);
	draw(&game.renderer, gameplay->bg, 0, 0, 17, 10);
	
	//draw(gameplay->renderer, gameplay->texture, 1.f, 1.f, 4.f, 4.f, 3.f, 3.f);
	world_render(&gameplay->world, delta);

	game.debug_font->drawh = .2f;
	set_color(&game.renderer, &white);
	draw(&game.renderer, game.debug_font, log_buff, 0.1f, 9.f);

	flush(&game.renderer);
}