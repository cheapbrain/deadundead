
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
//#define STBI_ONLY_PNG
#include "stb_image.h"

#include "utils.h"

Color white = {1.f, 1.f, 1.f, 1.f};
Color black = {0.f, 0.f, 0.f, 1.f};

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

	Texture *texture = (Texture *)malloc(sizeof(Texture));
	texture->width = width;
	texture->height = height;
	texture->id = id;
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
	GLuint vertshader = compile_shader(vpath, GL_VERTEX_SHADER);
	GLuint fragshader = compile_shader(fpath, GL_FRAGMENT_SHADER);
	GLuint program = link_shader(vertshader, fragshader);

	GLint u_texture = glGetUniformLocation(program, "u_texture");
	glUniform1i(u_texture, 0);

	Shader *shader = (Shader *)malloc(sizeof(Shader));
	shader->id = program;
	shader->u_transform = glGetUniformLocation(program, "u_transform");
	return shader;
}

Font *load_font(char *path) {
	//std::string folder(path);
	//folder = folder.substr(0, folder.find_last_of("\\/") + 1);

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

	Shader *shader = load_shader("../data/shaders/font.vert", "../data/shaders/font.frag", SHADER_SPRITE);
	Texture *texture = load_texture("../data/fonts/font.png");

	GLint u_smoothing = glGetUniformLocation(shader->id, "u_smoothing");
	GLint u_thickness = glGetUniformLocation(shader->id, "u_thickness");

	Font * font = (Font *)malloc(sizeof(Font));
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
	return font;
}