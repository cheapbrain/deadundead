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

struct ArrayList {
	void *array;
	int element_size;
	int count;
};

struct Texture {
	char *name;
	unsigned int id;
	int width;
	int height;
};

struct Shader {
	char *name;
	unsigned int id;
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
	char *name;
	unsigned int id;
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

struct SpriterObjectRef {
	int timeline;
	int parent; // -1 = no parent
	int key;
};

struct SpriterAnimationKey {
	int time;
	ArrayList bones;
	ArrayList images;
};

struct SpriterTimelineKey {
	int time;
	int spin;
	float x;
	float y;
	float scale_x;
	float scale_y;
	float angle;
	float a;
	int folder;
	int file;
};

enum SpriterObjectType {
	SPRITER_BONE = 0,
	SPRITER_IMAGE = 1
};

struct SpriterTimeline {
	int type;
	char *name;
	ArrayList keys;
};

struct SpriterAnimation {
	char *name;
	int length;
	int interval;
	ArrayList animation_keys;
	ArrayList timelines;
};

struct SpriterTexture {
	char *name;
	Texture *texture;
	float width;
	float height;
	float pivot_x;
	float pivot_y;
	float t_x;
	float t_y;
	float t_w;
	float t_h;
};

struct SpriterFolder {
	ArrayList files;
};

struct SpriterCharacter {
	char *name;
	unsigned int id;
	ArrayList folders;
	ArrayList animations;
};

struct SpriterPlayer {
	SpriterCharacter *character;
	float x;
	float y;
	float scale_x;
	float scale_y;
	int active_animation;
	float animation_speed;
};

void identity(Mat3 *mat);

void orthographic(Mat3 *mat, float height, float left, float bottom); // width / height = 17 / 10, 

void log_error(char *msg);

void log_string(char *msg);

void init_asset_manager();

Texture *load_texture(char *path);

#define SHADER_SPRITE 0
#define SHADER_PARTICLE 1
#define SHADER_FONT 2
Shader *load_shader(char *vpath, char *fpath, int shader_type);

Font *load_font(char *path);

SpriterCharacter *load_spriter_character(char *path);

void dispose_texture(Texture *texture);

void dispose_shader(Shader *shader);

void dispose_font(Font *font);

void dispose_all();

extern char log_buff[10000];
extern char log_wbuff[100];

void log(int a);

void log(char a);

void log(float a);

void log(void *a);

void log(char *a);

void list_init(ArrayList *list, int element_size, int count);

void list_set(ArrayList *list, int index, void *value);

void *list_get(ArrayList *list, int index);