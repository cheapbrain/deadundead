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

#include "ui.h"


void draw_label(Label* l, float x, float y, SpriteRenderer s, Font* f) {
	draw(&s, f, l->text, x + l->x, y + l->y);
}

void draw_arrow(Arrow* a, float x, float y, SpriteRenderer s) {
	draw(&s, a->t, x + a->x, y + a->y, a->w, a->h);
}

void draw_window(Window* w, SpriteRenderer s, Font* f) {
	draw(&s, w->t, w->x, w->y, w->w, w->h);
	/*
	for (int i = 0; i < w->label_n; i++) {
		draw_label(w->labels[i], w->x, w->y, s, f);
	}
	for (int i = 0; i < w->arrow_n; i++) {
		draw_arrow(w->arrows[i], w->x, w->y, s);
	}
	*/
}

Label* new_label(char* text) {
	Label* l = (Label*)malloc(sizeof(Label));
	l->x = 1.f;
	l->y = 1.f;
	l->text = text;
	return l;
}

Arrow* new_arrow() {
	Arrow* a = (Arrow*)malloc(sizeof(Arrow));
	a->x = 1.f;
	a->y = 1.f;
	a->w = 5.f;
	a->y = 5.f;
	a->t = load_texture("../images/test.png");
	return a;
}

Window* new_window() {
	Window* w = (Window*)malloc(sizeof(Window));
	w->arrow_n = 0;
	w->label_n = 0;
	w->x = 1.f;
	w->y = 1.f;
	w->w = 5.f;
	w->h = 5.f;
	w->t = load_texture("../images/test.png");
	return w;
}