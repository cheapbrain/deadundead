#pragma once

#include <stdio.h>

extern FILE *logger_stream;
#define LOGGER_STREAM logger_stream

struct Vec2 {
	float x;
	float y;
};

struct Rectangle {
	struct Vec2 pos;//angolo in basso a sx
	struct Vec2 size;
};

struct Mat3 {
	float m[9];
};

struct Color {
	float r;
	float g;
	float b;
	float a; 
};

extern Color white;
extern Color black;

struct Texture {
	int id;
	int width;
	int height;
};

struct Shader {
	int id;
	int u_transform;
};

struct Character {
	short x;
	short y;
	unsigned char w;
	unsigned char h;
	signed char xadv;
	signed char xoff;
	signed char yoff;
};

struct Glyph {
	float x, y;
	float w, h;
	float tx, ty;
	float tw, th;
};

struct Kerning {
	signed char amount = 0;
};

struct Font {
	//info
	char size;
	char lpad, rpad, tpad, bpad;
	char xspacing, yspacing;
	//common
	char height;
	char base;
	short scaleW, scaleH;
	//other
	float drawh;
	float smoothing;
	float thickness;
	//shader
	int u_smoothing;
	int u_thickness;
	// text
	Glyph *glyphs;
	float textw, texth;
	// data
	Shader* shader;
	Texture* texture;
	Character* chars;
	Kerning* kerns;
};

void identity(Mat3 *mat);

void orthographic(Mat3 *mat, float height, float left, float bottom); // width / height = 16 / 9, 

void log_error(char *msg);

Texture *load_texture(char *path);

#define SHADER_SPRITE 0
#define SHADER_PARTICLE 1
#define SHADER_FONT 2
Shader *load_shader(char *vpath, char *fpath, int shader_type);

Font *load_font(char *path);