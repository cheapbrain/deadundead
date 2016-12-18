#include <GLFW/glfw3.h>

#include "stage_editor.h"
#include "renderer.h"
#include "screen.h"

double mouse_x, mouse_y;
int mouse_left, mouse_middle, mouse_right;
int changed_left, changed_middle, changed_right;

void fill_rect(float x, float y, float w, float h, float r, float g, float b, float a) {
	set_color(&game.renderer, r, g, b, a);
	draw(&game.renderer, game.renderer.default_texture, x, y, w, h);
}

int button(char *text, float x, float y, float w, float h, int focus) {
	int inside
	set_color(&game.renderer, 1, 0, 0, 1);
	if (focus)
		draw(&game.renderer, game.renderer.default_texture, x, y, w, h);
	if (mouse_left)
		set_color(&game.renderer, .2f, .4f, .6f, 1);
	else
		set_color(&game.renderer, .1f, .1f, .1f, 1);
	draw(&game.renderer, game.renderer.default_texture, x + 0.026f, y + 0.026f, w - 0.052f, h - 0.052f);
	set_color(&game.renderer, &white);
	draw(&game.renderer, game.debug_font, text, x + 0.04f, y + h - 0.04f);
	return !mouse_left && changed_left;
}

void stage_editor_init(Stage *stage) {
}

void stage_editor_enter(Stage *stage, int previous_stage_id) {
	mouse_left = 0;
	mouse_right = 0;
	mouse_middle = 0;
}

void stage_editor_leave(Stage *stage, int next_stage_id) {

}

void stage_editor_update(Stage *stage, double delta) {
	GLFWwindow *window = (GLFWwindow *)game.window;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	int temp_left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int temp_middle = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	int temp_right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	changed_left = mouse_left != temp_left;
	changed_middle = mouse_middle != temp_middle;
	changed_right = mouse_right != temp_right;
	mouse_left = temp_left;
	mouse_middle = temp_middle;
	mouse_right = temp_right;
}

void stage_editor_render(Stage *stage, double delta) {
	if (button("test", 1, 1, 1, .5f, 0)) {

	}
}