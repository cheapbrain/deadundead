
#include <string.h>
#include <stdlib.h>
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#include "utils.h"
#include "screen.h"
#include "jsmn.h"

#define TABLE_SIZE 0x10000
#define TABLE_MASK 0x0FFFF

Color white = {1.f, 1.f, 1.f, 1.f};
Color black = {0.f, 0.f, 0.f, 1.f};

void init_asset_manager() {
	game.asset_manager.assets = (Asset **)calloc(TABLE_SIZE, sizeof (Asset *));
}

static int _hash(const unsigned char *key, int len) {
  const unsigned int m = 0x5bd1e995;
  const int r = 24;
  unsigned int h = 19991 ^ len;
  while(len >= 4) {
    unsigned int k = *(unsigned int*)key;
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
    key += 4;
    len -= 4;
  }
  switch(len) {
  case 3: h ^= key[2] << 16;
  case 2: h ^= key[1] << 8;
  case 1: h ^= key[0]; h *= m;
  };
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
} 

Asset *_asset_location(char *name) {
	int hash = _hash((const unsigned char *)name, strlen(name)) & TABLE_MASK;
	Asset **asset = &game.asset_manager.assets[hash];
	for(;;) {
		if (*asset == NULL) {
			*asset = (Asset *)malloc(sizeof(Asset));
			(*asset)->name = NULL;
			break;
		} else if (!strcmp(name, (*asset)->name)) {
			break;
		} else {
			asset = &((*asset)->next);
		}
	}
	return *asset;
}

void *_get_asset(AssetType type, char *name) {
	Asset *asset = _asset_location(name);

	if (asset->name == 	NULL) {
		int size;
		switch(type) {
			case TEXTURE:
				size = sizeof Texture;
				break;
			case SHADER:
				size = sizeof Shader;
				break;
			case FONT:
				size = sizeof Font;
				break;
			case SPRITER_CHARACTER:
				size = sizeof SpriterCharacter;
				break;
		}
		int name_size = strlen(name) + 1;
		char *name_copy = (char *)malloc(name_size);
		memcpy(name_copy, name, name_size);
		void *data = malloc(size);
		((Texture *)data)->name = name_copy;
		((Texture *)data)->id = -1;
		asset->name = name_copy;
		asset->type = type;
		asset->data = data;
		asset->next = NULL;
		game.asset_manager.count++;
	}
	return asset->data;
}

void _remove_asset(char *name) {
	int hash = _hash((const unsigned char *)name, strlen(name)) & TABLE_MASK;
	Asset **asset = &game.asset_manager.assets[hash];
	for(;;) {
		if (*asset == NULL) {
			return;
		} else if (!strcmp(name, (*asset)->name)) {
			Asset *next = (*asset)->next;
			free((*asset)->data);
			free(*asset);
			*asset = next;
			game.asset_manager.count--;
			return;
		} else {
			asset = &((*asset)->next);
		}
	}
}

char* read_text_file(char *file) {
	FILE *fptr;
	long length;
	char *buf;

	fptr = fopen(file, "rb");
	if (!fptr) {
		fprintf(LOGGER_STREAM, "%s ", file);
		log_error("File not found.");
	}
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = '\0';

	return buf;
}

Texture *load_texture(char *path) {
	Texture *texture = (Texture *)_get_asset(TEXTURE, path);

	if (texture->id == -1) {
		int width, height, cpp;
		unsigned int id;
		unsigned char *data = stbi_load(path, &width, &height, &cpp, 4);
		if (data == NULL) {
			fprintf(LOGGER_STREAM, "%s ", path);
			log_error("File not found"); 
		}
		glGenTextures(1, &id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		stbi_image_free(data);

		texture->width = width;
		texture->height = height;
		texture->id = id;

		fprintf(LOGGER_STREAM, "Loaded texture: %s\n", path);
	}
	return texture;
}

GLuint compile_shader(char *path, GLenum type) {
	char *source = read_text_file(path);
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, 0);
	glCompileShader(shader);
	GLint isCompiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char *errorlog = (char *)malloc(sizeof(char)*maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, errorlog);
		fprintf(LOGGER_STREAM, "%s ", path);
		log_error(errorlog);
	}
	free(source);
	return shader;
}

GLuint link_shader(GLuint vertshader, GLuint fragshader) {
	GLuint shaderprogram = glCreateProgram();
	glAttachShader(shaderprogram, vertshader);
	glAttachShader(shaderprogram, fragshader);
	glBindAttribLocation(shaderprogram, 0, "i_pos_tex");
	glBindAttribLocation(shaderprogram, 1, "i_color");
	glBindFragDataLocation(shaderprogram, 0, "gl_FragColor");

	glLinkProgram(shaderprogram);
	glUseProgram(shaderprogram);

	GLint isLinked = 0;
	glGetProgramiv(shaderprogram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);
		char *errorlog = (char *)malloc(sizeof(char)*maxLength);
		glGetProgramInfoLog(shaderprogram, maxLength, &maxLength, errorlog);
		log_error(errorlog);
	}
	glDetachShader(shaderprogram, vertshader);
	glDetachShader(shaderprogram, fragshader);
	glDeleteShader(vertshader);
	glDeleteShader(fragshader);
	return shaderprogram;
}

Shader *load_shader(char *vpath, char *fpath, int shader_type) {
	Shader *shader = (Shader *)_get_asset(SHADER, vpath);
	if (shader->id == -1) {
		GLuint vertshader = compile_shader(vpath, GL_VERTEX_SHADER);
		GLuint fragshader = compile_shader(fpath, GL_FRAGMENT_SHADER);
		GLuint program = link_shader(vertshader, fragshader);

		GLint u_texture = glGetUniformLocation(program, "u_texture");
		glUniform1i(u_texture, 0);

		shader->id = program;
		shader->u_transform = glGetUniformLocation(program, "u_transform");
	}
	return shader;
}

Font *load_font(char *path) {
	
	Font *font = (Font *)_get_asset(FONT, path);

	if (font->id == -1) {

		char cfg_size;
		char cfg_lpad;
		char cfg_rpad;
		char cfg_tpad;
		char cfg_bpad;
		char cfg_xspacing;
		char cfg_yspacing;
		char cfg_height;
		char cfg_base;
		short cfg_scaleW;
		short cfg_scaleH;

		Character* chars = (Character *)calloc(256, sizeof(Character));
		Kerning* kers = (Kerning *)calloc(256*256, sizeof(Kerning));

		int charid = 0;

		int first, second;

		char left[256];
		left[0] = 0;
		int left_len = 0;
		char right[256];
		right[0] = 0;
		int right_len = 0;

		int tag = 0;
		int status = 0;
		int count = 0;
		int ignore = 0;

		char ch;
		char *buffer = (char *)malloc(4096);
		int buffer_size = 4096;
		int buffer_i = 4096;
		FILE *fp = fopen(path, "rb");
		while (1) {
			if (buffer_i == buffer_size) {
				if (buffer_size < 4096) break;
				buffer_size = fread(buffer, 1, 4096, fp);
				if (buffer_size == 0) break;
				buffer_i = 0;
			}
			ch = buffer[buffer_i];
			buffer_i++;

			switch (status)  {
			case 0:
				if (ch == ' ' || ch == '\n') {
					if (0 == strcmp(left, "info")) tag = 0;
					else if (0 == strcmp(left, "common")) tag = 1;
					else if (0 == strcmp(left, "page")) tag = 2;
					else if (0 == strcmp(left, "chars")) tag = 3;
					else if (0 == strcmp(left, "char")) tag = 4;
					else if (0 == strcmp(left, "kernings")) tag = 5;
					else if (0 == strcmp(left, "kerning")) tag = 6;
					left[0] = 0;
					left_len = 0;
				}
				else if (ch == '=') {
					status = 1;
				} else {
					left[left_len] = ch;
					left[++left_len] = 0;
				}
				break;
			case 1:
				if ((ch == ' ' && !ignore) || ch == '\n') {
					if (0 == strcmp(left, "size")) cfg_size = atoi(right);
					else if (0 == strcmp(left, "padding"))
						switch (count) {
						case 0: cfg_tpad = atoi(right); break;
						case 1: cfg_lpad = atoi(right); break;
						case 2: cfg_bpad = atoi(right); break;
						case 3: cfg_rpad = atoi(right); break;
						}
					else if (0 == strcmp(left, "spacing"))
						switch (count) {
						case 0: cfg_xspacing = atoi(right); break;
						case 1: cfg_yspacing = atoi(right); break;
						}
					else if (0 == strcmp(left, "lineHeight")) cfg_height = atoi(right);
					else if (0 == strcmp(left, "base")) cfg_base = atoi(right);
					else if (0 == strcmp(left, "scaleW")) cfg_scaleW = atoi(right);
					else if (0 == strcmp(left, "scaleH")) cfg_scaleH = atoi(right);
					//else if (0 == strcmp(left, "pages")) cfg_pages = atoi(right);
					//else if (0 == strcmp(left, "file")) paths.push_back(folder + right);

					else if (0 == strcmp(left, "id") && tag == 4) charid = atoi(right);
					else if (0 == strcmp(left, "x")) chars[charid].x = atoi(right);
					else if (0 == strcmp(left, "y")) chars[charid].y = atoi(right);
					else if (0 == strcmp(left, "width")) chars[charid].w = atoi(right);
					else if (0 == strcmp(left, "height")) chars[charid].h = atoi(right);
					else if (0 == strcmp(left, "xoffset")) chars[charid].xoff = atoi(right);
					else if (0 == strcmp(left, "yoffset")) chars[charid].yoff = atoi(right);
					else if (0 == strcmp(left, "xadvance")) chars[charid].xadv = atoi(right);
					//else if (0 == strcmp(left, "page")) chars[charid].page = atoi(right);
					//else if (0 == strcmp(left, "chnl")) chars[charid].chnl = atoi(right);

					else if (0 == strcmp(left, "first")) first = atoi(right);
					else if (0 == strcmp(left, "second")) second = atoi(right);
					else if (0 == strcmp(left, "amount")) kers[first + second * 256].amount = atoi(right);

					status = 0;
					count = 0;
					left[0] = 0;
					left_len = 0;
					right[0] = 0;
					right_len = 0;
				}
				else if (ch == ',') {
					if (0 == strcmp(left, "padding"))
						switch (count) {
						case 0: cfg_tpad = atoi(right); break;
						case 1: cfg_lpad = atoi(right); break;
						case 2: cfg_bpad = atoi(right); break;
						case 3: cfg_rpad = atoi(right); break;
						}
					else if (0 == strcmp(left, "spacing"))
						switch (count) {
						case 0: cfg_xspacing = atoi(right); break;
						case 1: cfg_yspacing = atoi(right); break;
						}
					count++;
					right[0] = 0;
					right_len = 0;
				}
				else if (ch == '"') 
					ignore = !ignore;
				else {
					right[right_len] = ch;
					right[++right_len] = 0;
				}
			}
		}
		fclose(fp);
		free(buffer);

		Shader *shader = load_shader("../data/shaders/font.vert", "../data/shaders/font.frag", SHADER_SPRITE);
		Texture *texture = load_texture("../data/fonts/font.png");

		GLint u_smoothing = glGetUniformLocation(shader->id, "u_smoothing");
		GLint u_thickness = glGetUniformLocation(shader->id, "u_thickness");

		font->size = cfg_size;
		font->lpad = cfg_lpad;
		font->rpad = cfg_rpad;
		font->tpad = cfg_tpad;
		font->bpad = cfg_bpad;
		font->xspacing = cfg_xspacing;
		font->yspacing = cfg_yspacing;
		font->height = cfg_height;
		font->base = cfg_base;
		font->scaleW = cfg_scaleW;
		font->scaleH = cfg_scaleH;
		font->drawh = 1.f;
		font->smoothing = .1f;
		font->thickness = .5f;
		font->u_smoothing = u_smoothing;
		font->u_thickness = u_thickness;
		font->shader = shader;
		font->texture = texture;
		font->chars = chars;
		font->kerns = kers;
		font->id = 0;
	}
	return font;
}

static jsmntok_t *_skip_tokens(jsmntok_t *tok) {
	int obj_size;
	int array_size;
	switch(tok->type) {
		case JSMN_OBJECT:
			obj_size = tok->size;
			tok = tok + 1;
			for (int i = 0; i < obj_size; i++) {
				tok = _skip_tokens(tok + 1);
			}
			return tok;
		case JSMN_ARRAY:
			array_size = tok->size;
			tok = tok + 1;
			for (int i = 0; i < array_size; i++) {
				tok = _skip_tokens(tok);
			}
			return tok;
		default:
			return tok + 1;
	}
}

static jsmntok_t *_get_string(jsmntok_t *tok, char *json, char **string) {
	json[tok->end] = 0;
	int string_len = tok->end - tok->start + 1;
	*string = (char *)malloc(string_len);
	memcpy(*string, json + tok->start, string_len);
	tok = tok + 1;
	return tok;
}

static jsmntok_t *_get_int(jsmntok_t *tok, char *json, int *value) {
	json[tok->end] = 0;
	*value = atoi(json + tok->start);
	tok = tok + 1;
	return tok;
}

static jsmntok_t *_get_float(jsmntok_t *tok, char *json, float *value) {
	json[tok->end] = 0;
	*value = (float)atof(json + tok->start);
	tok = tok + 1;
	return tok;
}

jsmntok_t *_load_spriter_file(SpriterCharacter *sc, SpriterFolder *folder, jsmntok_t *tok, char *json) {
	SpriterTexture file;
	file.pivot_x = 0;
	file.pivot_y = 0;
	file.t_x = 0;
	file.t_y = 0;
	file.t_w = 1;
	file.t_h = 1;
	int id = 0;
	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "id")) {
			tok = _get_int(tok, json, &id);
		} else if (!strcmp(name, "width")) {
			tok = _get_float(tok, json, &file.width);
		} else if (!strcmp(name, "height")) {
			tok = _get_float(tok, json, &file.height);
		} else if (!strcmp(name, "pivot_x")) {
			tok = _get_float(tok, json, &file.pivot_x);
		} else if (!strcmp(name, "pivot_y")) {
			tok = _get_float(tok, json, &file.pivot_y);
		} else if (!strcmp(name, "name")) {
			char *relative_path;
			tok = _get_string(tok, json, &relative_path);
			int file_name_len = strlen(relative_path);
			int path_len = 0;
			int j = 0;
			for(;;) {
				char c = sc->name[j++];
				if (c == '/') path_len = j;
				else if (c == '\0') break;
			}
			char *absolute_path = (char *)malloc(path_len + file_name_len + 1);
			memcpy(absolute_path, sc->name, path_len);
			memcpy(absolute_path + path_len, relative_path, file_name_len + 1);
			file.name = absolute_path;
			file.texture = load_texture(absolute_path);
			free(relative_path);
		} else {
			tok = _skip_tokens(tok);
		}
	}
	list_set(&folder->files, id, &file);
	return tok;
}

jsmntok_t *_load_spriter_folder(SpriterCharacter *sc, jsmntok_t *tok, char *json) {
	SpriterFolder folder;
	int id = 0;
	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "id")) {
			tok = _get_int(tok, json, &id);
		} else if (!strcmp(name, "file")) {
			int file_count = tok->size;
			list_init(&folder.files, sizeof SpriterTexture, file_count);
			tok = tok + 1;
			for (int j = 0; j < file_count; j++) {
				tok = _load_spriter_file(sc, &folder, tok, json);
			}
		} else {
			tok = _skip_tokens(tok);
		}
	}
	list_set(&sc->folders, id, &folder);
	return tok;
}

jsmntok_t *_load_spriter_animation_key_object(ArrayList *list, jsmntok_t *tok, char *json) {
	SpriterObjectRef object;
	object.parent = -1;
	object.timeline = 0;
	object.key = 0;
	int id = 0;
	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "id")) {
			tok = _get_int(tok, json, &id);
		} else if (!strcmp(name, "parent")) {
			tok = _get_int(tok, json, &object.parent);
		} else if (!strcmp(name, "timeline")) {
			tok = _get_int(tok, json, &object.timeline);
		} else if (!strcmp(name, "key")) {
			tok = _get_int(tok, json, &object.key);
		} else {
			tok = _skip_tokens(tok);
		}
	}
	list_set(list, id, &object);
	return tok;
}

jsmntok_t *_load_spriter_animation_key(SpriterAnimation *animation, jsmntok_t *tok, char *json) {
	SpriterAnimationKey key;
	key.time = 0;
	int id = 0;
	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "id")) {
			tok = _get_int(tok, json, &id);
		} else if (!strcmp(name, "time")) {
			tok = _get_int(tok, json, &key.time);
		} else if (!strcmp(name, "bone_ref")) {
			int bone_count = tok->size;
			list_init(&key.bones, sizeof SpriterObjectRef, bone_count);
			tok = tok + 1;
			for (int j = 0; j < bone_count; j++) {
				tok = _load_spriter_animation_key_object(&key.bones, tok, json);
			}
		} else if (!strcmp(name, "object_ref")) {
			int image_count = tok->size;
			list_init(&key.images, sizeof SpriterObjectRef, image_count);
			tok = tok + 1;
			for (int j = 0; j < image_count; j++) {
				tok = _load_spriter_animation_key_object(&key.images, tok, json);
			}
		} else {
			tok = _skip_tokens(tok);
		}
	}
	list_set(&animation->animation_keys, id, &key);
	return tok;
}

jsmntok_t *_load_spriter_timeline_key_object(SpriterTimelineKey *key, jsmntok_t *tok, char *json) {
	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "x")) {
			tok = _get_float(tok, json, &key->x);
		} else if (!strcmp(name, "y")) {
			tok = _get_float(tok, json, &key->y);
		} else if (!strcmp(name, "scale_x")) {
			tok = _get_float(tok, json, &key->scale_x);
		} else if (!strcmp(name, "scale_y")) {
			tok = _get_float(tok, json, &key->scale_y);
		} else if (!strcmp(name, "angle")) {
			tok = _get_float(tok, json, &key->angle);
		} else if (!strcmp(name, "a")) {
			tok = _get_float(tok, json, &key->a);
		} else if (!strcmp(name, "folder")) {
			tok = _get_int(tok, json, &key->folder);
		} else if (!strcmp(name, "file")) {
			tok = _get_int(tok, json, &key->file);
		} else {
			tok = _skip_tokens(tok);
		}
	}
	return tok;
}

jsmntok_t *_load_spriter_timeline_key(SpriterTimeline *timeline, jsmntok_t *tok, char *json) {
	SpriterTimelineKey key;
	key.time = 0;
	key.x = 0;
	key.y = 0;
	key.scale_x = 1;
	key.scale_y = 1;
	key.angle = 0;
	key.a = 1;
	key.folder = 0;
	key.file = 0;
	int id = 0;

	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "id")) {
			tok = _get_int(tok, json, &id);
		} else if (!strcmp(name, "time")) {
			tok = _get_int(tok, json, &key.time);
		} else if (!strcmp(name, "spin")) {
			tok = _get_int(tok, json, &key.spin);
		} else if (!strcmp(name, "object") || !strcmp(name, "bone")) {
			tok = _load_spriter_timeline_key_object(&key, tok, json);
		} else {
			tok = _skip_tokens(tok);
		}
	}
	list_set(&timeline->keys, id, &key);
	return tok;
}


jsmntok_t *_load_spriter_timeline(SpriterAnimation *animation, jsmntok_t *tok, char *json) {
	SpriterTimeline timeline;
	timeline.type = SPRITER_IMAGE;
	int id = 0;
	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "id")) {
			tok = _get_int(tok, json, &id);
		} else if (!strcmp(name, "name")) {
			tok = _get_string(tok, json, &timeline.name);
		} else if (!strcmp(name, "object_type")) {
			json[tok->end] = 0;
			if (!strcmp(json + tok->start, "bone"))
				timeline.type = SPRITER_BONE;
			tok = tok + 1;
		} else if (!strcmp(name, "key")) {
			int key_count = tok->size;
			list_init(&timeline.keys, sizeof SpriterTimelineKey, key_count);
			tok = tok + 1;
			for (int j = 0; j < key_count; j++) {
				tok = _load_spriter_timeline_key(&timeline, tok, json);
			}
		} else {
			tok = _skip_tokens(tok);
		}
	}
	list_set(&animation->timelines, id, &timeline);
	return tok;
}

jsmntok_t *_load_spriter_animation(SpriterCharacter *sc, jsmntok_t *tok, char *json) {
	SpriterAnimation animation;
	int id = 0;
	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "id")) {
			tok = _get_int(tok, json, &id);
		} else if (!strcmp(name, "interval")) {
			tok = _get_int(tok, json, &animation.interval);
		} else if (!strcmp(name, "length")) {
			tok = _get_int(tok, json, &animation.length);
		} else if (!strcmp(name, "name")) {
			tok = _get_string(tok, json, &animation.name);
		} else if (!strcmp(name, "mainline")) {
			tok = tok + 2;
			int key_count = tok->size;
			list_init(&animation.animation_keys, sizeof SpriterAnimationKey, key_count);
			tok = tok + 1;
			for (int j = 0; j < key_count; j++) {
				tok = _load_spriter_animation_key(&animation, tok, json);
			}
		} else if (!strcmp(name, "timeline")) {
			int timeline_count = tok->size;
			list_init(&animation.timelines, sizeof SpriterTimeline, timeline_count);
			tok = tok + 1;
			for (int j = 0; j < timeline_count; j++) {
				tok = _load_spriter_timeline(&animation, tok, json);
			}
		} else {
			tok = _skip_tokens(tok);
		}
	}
	list_set(&sc->animations, id, &animation);
	return tok;
}

jsmntok_t *_load_spriter_entity(SpriterCharacter *sc, jsmntok_t *tok, char *json) {
	int size = tok->size;
	tok = tok + 1;
	for (int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if (!strcmp(name, "animation")) {
			int animation_count = tok->size;
			list_init(&sc->animations, sizeof SpriterAnimation, animation_count);
			tok = tok + 1;
			for (int j = 0; j < animation_count; j++) {
				tok = _load_spriter_animation(sc, tok, json);
			}
		} else {
			tok = _skip_tokens(tok);
		}
	}
	return tok;
}

jsmntok_t *_load_spriter_scon(SpriterCharacter *sc, jsmntok_t *tok, char *json) {
	int size = tok->size;
	tok = tok + 1;
	for(int i = 0; i < size; i++) {
		json[tok->end] = 0;
		char *name = json + tok->start;
		tok = tok + 1;
		if(!strcmp(name, "entity")) {
			tok = _load_spriter_entity(sc, tok + 1, json);
		} else if(!strcmp(name, "folder")) {
			int folder_count = tok->size;
			list_init(&sc->folders, sizeof SpriterFolder, folder_count);
			tok = tok + 1;
			for (int j = 0; j < folder_count; j++) {
				tok = _load_spriter_folder(sc, tok, json);
			}
		} else {
			tok = _skip_tokens(tok);
		}
	}
	return tok;
}

SpriterCharacter *load_spriter_character(char *path) {
	SpriterCharacter *sc = (SpriterCharacter *)_get_asset(SPRITER_CHARACTER, path);

	if (sc->id == -1) {
		jsmn_parser p;
		jsmntok_t *tok;
		int tok_count = 2;
		char *string= NULL;
		int string_len = 0;
		{
			FILE *fptr;
			fptr = fopen(path, "rb");
			if (!fptr) {
				fprintf(LOGGER_STREAM, "%s ", path);
				log_error("File not found.");
			}
			fseek(fptr, 0, SEEK_END);
			string_len = ftell(fptr);
			string = (char*)malloc(string_len + 1);
			fseek(fptr, 0, SEEK_SET);
			fread(string, string_len, 1, fptr);
			fclose(fptr);
			string[string_len] = '\0';
		}
		jsmn_init(&p);
		tok = (jsmntok_t *)malloc(sizeof(*tok) * tok_count);
		for (;;) {
			int error = jsmn_parse(&p, string, string_len, tok, tok_count);
			switch(error) {
				case JSMN_ERROR_NOMEM:
				tok_count = tok_count * 2;
				tok = (jsmntok_t *)realloc(tok, sizeof(*tok) * tok_count);
				break;
				case JSMN_ERROR_INVAL:
				case JSMN_ERROR_PART:
				fprintf(LOGGER_STREAM, "%s: ", path);
				log_error("File corrupted.");
				break;
				default:
				tok_count = error;
				jsmntok_t *t = tok;
				_load_spriter_scon(sc, tok, string);
				free(tok);
				free(string);
				return sc;
			}
		}
	}
	return sc;
}

void dispose_texture(Texture *texture) {
	glDeleteTextures(1, &texture->id);
	_remove_asset(texture->name);
}

void dispose_shader(Shader *shader) {
	glDeleteProgram(shader->id);
	_remove_asset(shader->name);
}

void dispose_font(Font *font) {
	dispose_texture(font->texture);
	dispose_shader(font->shader);
	free(font->chars);
	free(font->kerns);
	_remove_asset(font->name);
}

void dispose_spriter_character(SpriterCharacter *sc) {

	_remove_asset(sc->name);
}

void dispose_all() {
	for(int i = 0; i < TABLE_SIZE; i++) {
		Asset *asset = game.asset_manager.assets[i];
		while(asset != 	NULL)	{
			Asset *next = asset->next;
			switch(asset->type) {
				case TEXTURE:
					dispose_texture((Texture *)asset->data);
					break;
				case SHADER:
					dispose_shader((Shader *)asset->data);
					break;
				case FONT:
					dispose_font((Font *)asset->data);
					break;
				case SPRITER_CHARACTER:
					dispose_spriter_character((SpriterCharacter *)asset->data);
					break;
			}
			asset = next;
		}
	}
}

void list_init(ArrayList *list, int element_size, int count) {
	list->array = malloc(element_size * count);
	list->count = count;
	list->element_size = element_size;
}

void list_set(ArrayList *list, int index, void *value) {
	if (index >= list->count) {
		int new_count = list->count * 2;
		if (new_count <= index) new_count = index + 1;
		list->array = realloc(list->array, list->element_size * new_count);
		list->count = new_count;
	}
	memcpy((char *)list->array + list->element_size * index, value, list->element_size);
}

void *list_get(ArrayList *list, int index) {
	return (void *)((char *)list->array + list->element_size * index);
}