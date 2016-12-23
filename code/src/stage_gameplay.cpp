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
	
	World *world = &gameplay->world;
	world_init(world, 100);

	if (game.input.count > 0) {
		Entity *player = world_new_entity(world, RENDER | UPDATE | DYNAMIC_COLLIDE);
		player->x = 8.1f;
		player->y = 3;
		player->speed_x = 0;
		player->speed_y = 0;
		player->width = .5f;
		player->height = .82f;
		player->texture = load_texture("../images/player.png");
		player->update = player_update;
		player->render = player_render;
		player->is_on_floor = 0;
		player->player_id = 0;
	}

	if (game.input.count > 1) {
		Entity *player = world_new_entity(world, RENDER | UPDATE | DYNAMIC_COLLIDE);
		player->x = 4.1f;
		player->y = 3;
		player->speed_x = 0;
		player->speed_y = 0;
		player->width = .4f;
		player->height = .8f;
		player->texture = load_texture("../images/player2.png");
		player->update = player_update;
		player->render = player_render;
		player->is_on_floor = 0;
		player->player_id = 1;
	}

	Entity *bg = world_new_entity(world, RENDER_BACK);
	bg->x = 0;
	bg->y = 0;
	bg->width = 17;
	bg->height = 10;
	bg->texture = load_texture("../images/bg.png");
	bg->render = wall_render;

	for (int x = 0; x < 17; x++) {
		for (int y = 0; y < 10; y++) {
			int floorx = x & 3;
			int floory = y & 3;
			if (x > 0 && x < 16 && y > 0 && y < 9)
				if (floorx > 1 || floorx != floory || y == 1) 
					continue;
			Entity *wall = world_new_entity(world, RENDER_BACK | STATIC_COLLIDE);
			wall->x = (float)x;
			wall->y = (float)y;
			wall->width = 1;
			wall->height = 1;
			wall->texture = load_texture("../images/wall.png");
			wall->render = wall_render;
			wall->bounce_coeff = 0;
		}
	}

	Entity *wall = world_new_entity(world, RENDER_BACK | STATIC_COLLIDE);
	wall->x = 5;
	wall->y = 2;
	wall->width = 6;
	wall->height = .2f;
	wall->texture = load_texture("../images/wall.png");
	wall->render = wall_render;
	wall->bounce_coeff = .25f;
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
	
	//draw(gameplay->renderer, gameplay->texture, 1.f, 1.f, 4.f, 4.f, 3.f, 3.f);
	world_render(&gameplay->world, delta);

	game.debug_font->drawh = .2f;
	set_color(&game.renderer, &white);
	draw(&game.renderer, game.debug_font, log_buff, 0.1f, 9.f);

	flush(&game.renderer);
}