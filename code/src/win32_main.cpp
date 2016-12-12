#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "screen.h"
#include "renderer.h"
#include "utils.h"
#include "stage.h"
#include "stage_gameplay.h"

Game game;

FILE *logger_stream;
#define LOGGER_INIT logger_stream = fopen("log.txt", "w");
#define LOGGER_CLOSE fclose(logger_stream);
#define MIN_FPS 30
#define MAX_DELTA (1.0/(double)MIN_FPS)

const short keyboard_layout[B_COUNT] = {
	GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_SPACE,
	GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L,
	GLFW_KEY_ESCAPE, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4
};

const short gamepad_layout[B_COUNT] = {
	300, 200, 301, 201, 2,
	3, 1, 0,
	8, 4, 5, 6, 7
};

const short xbox_layout[B_COUNT] = {
	300, 200, 201, 301, 0,
	2, 1, 3,
	6, 4, 5, 204, 205
};

const float null_axes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const unsigned char null_buttons[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static void error_callback(int error, const char* description) {
	fprintf(LOGGER_STREAM, "%d %s\n", error, description);
}

struct StageTest {
	Stage super;
	SpriteRenderer *renderer;
	Texture *texture;
	Shader *shader;
	Mat3 *transform;
	float angle;
	float angle_speed;
};

void stage_init_test(Stage *stage) {
	StageTest *test = (StageTest *)stage;
	test->renderer = &game.renderer;
	test->texture = load_texture("D:/Download/GreyGuy/head/p_head_hurt.png");
	test->shader = load_shader("../data/shaders/default.vert", "../data/shaders/default.frag", SHADER_SPRITE);
	test->transform = (Mat3 *)malloc(sizeof(Mat3));
	orthographic(test->transform, 9, 0, 0);
	test->angle = 0.f;
	test->angle_speed = 1.f;
}

void stage_enter_test(Stage *stage, int previous_stage_id) {

}

void stage_leave_test(Stage *stage, int next_stage_id) {

}

void stage_update_test(Stage *stage, double delta) {
	StageTest *test = (StageTest *)stage;

	if (button_state(B_PAUSE, 0)) game_close();
	test->angle += (float)(test->angle_speed * delta);
}

void stage_render_test(Stage *stage, double delta) {
	StageTest *test = (StageTest *)stage;
	
	set_matrix(test->renderer, test->transform);
	set_shader(test->renderer, test->shader);
	set_color(test->renderer, &white);
	draw(test->renderer, test->texture, 1.f, 1.f, 4.f, 4.f, 3.f, 3.f, test->angle, 0, 0, 1.f, 1.f);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	LOGGER_INIT

	game_init();
	game_clear_color(&black);

	Shader *default_shader = load_shader("../data/shaders/default.vert", "../data/shaders/default.frag", SHADER_SPRITE);
	Texture * default_texture = load_texture("../images/test.png");	
	init(&game.renderer, default_texture, default_shader);

	Font *font = load_font("../data/fonts/font.fnt");
	font->drawh = .4f;
	font->smoothing = .05f * 1080.f / game.height;
	font->thickness = .5;

	game.debug_font = font;

	double time_passed = 1.0;
	char fps_text[128];
	int frames = 0;

	stage_manager_add(sizeof(StageGameplay), STAGE_GAMEPLAY, stage_gameplay_init, stage_gameplay_enter, stage_gameplay_leave, stage_gameplay_update, stage_gameplay_render);
	
	stage_manager_enter(STAGE_GAMEPLAY, TRANSITION_NONE, 0);

	while(!game_should_close()) {
		game_update();
		if (game.delta == 0) continue;
		
		glDisable(GL_SCISSOR_TEST);
		game_clear();
		glEnable(GL_SCISSOR_TEST);

		time_passed += game.delta;
		if (time_passed >= 1.0) {
			sprintf(fps_text, "FPS: %d", frames);
			time_passed -= 1.0;
			frames = 0;
		}

		Stage *active_stage = game.stage_manager.active_stage;
		active_stage->update(active_stage, game.delta);
		active_stage->render(active_stage, game.delta);

		font->drawh = .4f;
		set_color(&game.renderer, &white);
		draw(&game.renderer, font, fps_text, .1f, 9.9f);
		flush(&game.renderer);

		frames++;
	}
	game_dispose();
	LOGGER_CLOSE
	return 0;
}

void game_init() {
	char *title = "title";
	int vsync = 1;
	int width, height;
	
	GLFWmonitor *active_monitor;
	GLFWvidmode *active_mode;
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		log_error("Failed to initialize GLFW");
	}
	active_monitor = glfwGetPrimaryMonitor();
	active_mode = (GLFWvidmode *)glfwGetVideoMode(active_monitor);
	width = active_mode->width;
	height = active_mode->height;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 0);
	glfwWindowHint(GLFW_RED_BITS, active_mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, active_mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, active_mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, active_mode->refreshRate);
	window = glfwCreateWindow(width, height, title, active_monitor, NULL);
	//window = glfwCreateWindow(1366, 768, title, NULL, NULL);
	if (!window) {
		glfwTerminate();
		log_error("Failed to open GLFW window");
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(vsync);
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		glfwTerminate();
		log_error((char *)glewGetErrorString(err));
	}
	char *gl_version = (char *)glGetString(GL_VERSION);
	char *glsl_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
	if (strcmp(gl_version, "3.1") < 0) {
		glfwTerminate();
		log_error("Opengl 3.1 not supported by your video driver.");
	}

	glfwGetFramebufferSize(window, &width, &height);
	float pref_ratio = 17.f / 10.f;
	float ratio = (float) width / height;
	int x_offset, y_offset;
	if (ratio > pref_ratio) {
		int new_width = (int)roundf(height * pref_ratio);
		x_offset = (width - new_width) >> 1;
		y_offset = 0;
		width = new_width;
	} else {
		int new_height = (int)roundf(width / pref_ratio);
		x_offset = 0;
		y_offset = (height - new_height) >> 1;
		height = new_height;
	}

	glViewport(x_offset, y_offset, width, height);
	glScissor(x_offset, y_offset, width, height);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	fprintf(LOGGER_STREAM, "%s %s\n", gl_version, glsl_version);
	game.width = width;
	game.height = height;
	game.time = glfwGetTime();
	game.delta = 0;
	game.window = (void *)window;

	Input *in = &(game.input);
	*in = {};

	int id = 0;
	in->count = 0;
	while (id < 16 && in->count < 4) {
		if (glfwJoystickPresent(id)) {
			const char *joystick_name = glfwGetJoystickName(id);
			const short *layout;
			if (!strcmp(joystick_name, "Xbox 360 Controller")) layout = xbox_layout;
			else layout = gamepad_layout;
			in->player[in->count].id = id;
			memcpy(in->player[in->count].code, layout, sizeof(short) * B_COUNT);
			in->count++;
		}
		id++;
	}

	if (in->count < 4) {
		in->player[in->count].id = -1;
		memcpy(in->player[in->count].code, keyboard_layout, sizeof(short) * B_COUNT);
		in->count++;
	}

	StageManager *stage_manager = &(game.stage_manager);
	*stage_manager = {};

	stage_manager->stages = (Stage **)calloc(_STAGE_COUNT, sizeof(Stage *));
	stage_manager->max_stage_id = _STAGE_COUNT - 1;
	stage_manager->active_stage = NULL;
}

void game_dispose() {
	glfwDestroyWindow((GLFWwindow *)game.window);
	glfwTerminate();
}

void game_update() {
	glfwSwapBuffers((GLFWwindow *)game.window);
	glfwPollEvents();

	if (glfwGetWindowAttrib((GLFWwindow *)game.window, GLFW_ICONIFIED)) {
		game.delta = 0;
		return;
	}

	for (int i = 0; i < game.input.count; i++) {
		PlayerInput *player = &(game.input.player[i]);
		if (player->id < 0) {
			for (int j = 0; j < B_COUNT; j++) {
				char last = player->state[j];
				char state = glfwGetKey((GLFWwindow *)game.window, player->code[j]);
				char diff = last ^ state;
				player->pressed[j] = diff & state;
				player->released[j] = diff & last;
				player->state[j] = state;
			}
		} else {
			int acount, bcount;
			const float *axes = glfwGetJoystickAxes(player->id, &acount);
			const unsigned char *buttons = glfwGetJoystickButtons(player->id, &bcount);

			if (axes == NULL) axes = null_axes;
			if (buttons == NULL) buttons = null_buttons;

			for (int j = 0; j < B_COUNT; j++) {
				char state;
				short code = player->code[j];
				short id = code % 100;
				if (code < 100) {
					state = buttons[id];
				} else {
					float sign = code < 300 ? 1.f : -1.f;
					state = sign * axes[id] > .5f;
				}
				char last = player->state[j];
				char diff = last ^ state;
				player->pressed[j] = diff & state;
				player->released[j] = diff & last;
				player->state[j] = state;
			}
		}
	}

	double time = glfwGetTime();
	double delta = time - game.time;
	game.delta = delta < MAX_DELTA ? delta : MAX_DELTA;
	game.time = time;
}

void game_close() {
	glfwSetWindowShouldClose((GLFWwindow *)game.window, 1);
}

int game_should_close() {
	return glfwWindowShouldClose((GLFWwindow *)game.window);
}

void game_clear() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void game_clear_color(Color *color) {
	glClearColor(color->r, color->g, color->b, color->a);
}

void game_clear_color(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

int button_state(char button, char player) {
	return game.input.player[player].state[button];
}

int button_pressed(char button, char player) {
	return game.input.player[player].pressed[button];
}

int button_released(char button, char player) {
	return game.input.player[player].released[button];
}

void log_error(char *msg) {
	fprintf(LOGGER_STREAM, "Error: %s\n", msg);
	LOGGER_CLOSE
	exit(EXIT_FAILURE);
}