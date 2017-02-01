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
	int rotate_texture;
};

struct SpriterFolder {
	ArrayList files;
};

struct TextureInfo {
	char *name;
	float t_x;
	float t_y;
	float t_w;
	float t_h;
	int offset_x;
	int offset_y;
	int width;
	int height;
	int rotate_texture;
	int trim;
};

struct Atlas {
	char *name;
	Texture *texture;
	ArrayList textures;
};

enum AnimationStates {
	ANIMATION_STATE_IDLE = 0,
	ANIMATION_STATE_RUN,
	ANIMATION_STATE_JUMP_START,
	ANIMATION_STATE_JUMP_END,
	ANIMATION_STATE_IN_AIR,
	ANIMATION_STATE_HURT,
	ANIMATION_STATE_STUNNED,
	ANIMATION_STATE_ATTACK,
	ANIMATION_STATE_PICKUP,
	_ANIMATION_STATE_COUNT
};

enum AnimationWeapons {
	ANIMATION_WEAPON_HANDS = 0,
	ANIMATION_WEAPON_GUN,
	ANIMATION_WEAPON_SWORD,
	ANIMATION_WEAPON_BOMB,
	_ANIMATION_WEAPON_COUNT
};

struct SpriterCharacter {
	char *name;
	unsigned int id;
	ArrayList folders;
	ArrayList animations;
	Atlas *atlas;
	int animation_bindings[_ANIMATION_STATE_COUNT * _ANIMATION_WEAPON_COUNT];
};

struct SpriterInstance {
	SpriterCharacter *character;
	float offset_x;
	float offset_y;
	float scale_x;
	float scale_y;
	int flip;
	int active_animation;
	float animation_speed;
	float animation_time;
	Texture *in_hand;
};

void play_animation(SpriterInstance *si, int state, int weapon);

struct IntSet {
	int *elements;
	int count;
	int size;
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

//definito in entities.cpp
void int_set_init(IntSet *set, int initial_size);
int int_set_add(IntSet *set, int element);	//aggiunge solo se l'elemento non c'è già; ritorna se lo ha inserito
int int_set_contains(IntSet *set, int element);
void int_set_destroy(IntSet *set);