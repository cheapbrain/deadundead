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

FILE *logger_stream;
#define LOGGER_INIT logger_stream = fopen("log.txt", "w");
#define LOGGER_CLOSE fclose(logger_stream);

static void error_callback(int error, const char* description) {
	fprintf(LOGGER_STREAM, "%d %s\n", error, description);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	LOGGER_INIT
	Screen screen;
	init(&screen);
	Input input;
	init(&input, &screen);
	Color color = {.1f, .1f, .1f, 1.f};
	set_clear_color(&screen, &color);

	Shader *shader = load_shader("../data/shaders/default.vert", "../data/shaders/default.frag", SHADER_SPRITE);

	Texture * texture = load_texture("../images/test.png");	

	Font *font = load_font("../data/fonts/font.fnt");
	font->drawh = .4f;
	font->smoothing = .05f * 1080.f / screen.height;
	font->thickness = .5;

	Mat3 transform;
	orthographic(&transform, 9, 0, 0);

	SpriteRenderer renderer;
	init(&renderer, texture, shader);
	set_matrix(&renderer, &transform);

	double last_log_time = get_time();
	double last_frame_time = get_time();
	int frames = 0;
	char fps_text[128];
	sprintf(fps_text, "FPS: %d", 0);

	while(!should_close(&screen)) {
		glDisable(GL_SCISSOR_TEST);
		clear(&screen);
		glEnable(GL_SCISSOR_TEST);

		double time = get_time();
		double delta = time - last_frame_time;
		last_frame_time = time;
		if (time - last_log_time >= 1.0) {
			sprintf(fps_text, "FPS: %d", frames);
			last_log_time += 1;
			frames = 0;
		}

		set_shader(&renderer, shader);
		
		set_color(&renderer, &white);
		draw(&renderer, texture, 1.f, 1.f, 6.f, 6.f);
		set_color(&renderer, &Color{1, 1, 1, 1});
		draw(&renderer, font, fps_text, .1f, 8.9f);
		for (int j = 0; j < input.count; j++) {
			char jtext[128];
			jtext[0] = '\0';
			for (int k = 0; k < B_COUNT; k++) {
				char jbuff[20];
				sprintf(jbuff, " %d", input.player[j].state[k]);
				strcat(jtext, jbuff);
			}
			draw(&renderer, font, (char *)jtext, .1f, 8.4f - 0.5f * j);
		}
		flush(&renderer);

		update(&screen);
		update(&input, &screen);
		frames++;
	}
	dispose(&screen);
	LOGGER_CLOSE
	return 0;
}

void init(Screen *screen) {
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
	float pref_ratio = 1920.f / 1080.f;
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
	screen->width = width;
	screen->height = height;
	screen->window = (void *)window;
}

void dispose(Screen *screen) {
	glfwDestroyWindow((GLFWwindow *)screen->window);
	glfwTerminate();
}

void update(Screen *screen) {
	glfwSwapBuffers((GLFWwindow *)screen->window);
	glfwPollEvents();
}

int should_close(Screen *screen) {
	return glfwWindowShouldClose((GLFWwindow *)screen->window);
}

void clear(Screen *screen) {
	glClear(GL_COLOR_BUFFER_BIT);
}

void set_clear_color(Screen *screen, Color *color) {
	glClearColor(color->r, color->g, color->b, color->a);
}

void log_error(char *msg) {
	fprintf(LOGGER_STREAM, "Error: %s\n", msg);
	LOGGER_CLOSE
	exit(EXIT_FAILURE);
}

double get_time() {
	return glfwGetTime();
}

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

const float null_axes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const unsigned char null_buttons[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void init(Input *in, Screen *screen) {
	*in = {};

	int id = 0;
	in->count = 0;
	while (id < 16 && in->count < 4) {
		if (glfwJoystickPresent(id)) {
			in->player[in->count].id = id;
			memcpy(in->player[in->count].code, gamepad_layout, sizeof(short) * B_COUNT);
			in->count++;
		}
		id++;
	}

	if (in->count < 4) {
		in->player[in->count].id = -1;
		memcpy(in->player[in->count].code, keyboard_layout, sizeof(short) * B_COUNT);
		in->count++;
	}
}

void update(Input *input, Screen *screen) {
	for (int i = 0; i < input->count; i++) {
		PlayerInput *player = &(input->player[i]);
		if (player->id < 0) {
			for (int j = 0; j < B_COUNT; j++) {
				char last = player->state[j];
				char state = glfwGetKey((GLFWwindow *)screen->window, player->code[j]);
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
}

int state_anyone(Input *input, char button) {
	for (int i = 0; i < input->count; i++)
		if (input->player[i].state[button]) return 1;
	return 0;
}

int state(Input *input, char button, char player) {
	return input->player[player].state[button];
}