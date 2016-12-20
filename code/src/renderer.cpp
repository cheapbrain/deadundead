#include <GL/glew.h>
#include <stdlib.h>
#include <math.h>

#include "utils.h"
#include "renderer.h"

void init(SpriteRenderer *renderer, Texture *default_texture, Shader *default_shader) {
	GLuint vao, vbo;
	int size = INSTANCE_COUNT * 6 * 8;
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), NULL, GL_STREAM_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(4 * sizeof(float)));

	glActiveTexture(GL_TEXTURE0);

	renderer->active_color = Color{1.f, 1.f, 1.f, 1.f};
	renderer->active_texture = default_texture;
	renderer->active_shader = default_shader;
	renderer->default_texture = default_texture;
	renderer->default_shader = default_shader;
	renderer->vao = vao;
	renderer->vbo = vbo;
	renderer->size = 0;
	renderer->capacity = size;
	renderer->buffer = (float *)malloc(sizeof(float) * size);
	identity(&renderer->active_matrix);
	glUniformMatrix3fv(renderer->active_shader->u_transform, 1, GL_TRUE, renderer->active_matrix.m);
}

void dispose(SpriteRenderer *renderer) {
	free(renderer->buffer);
}

void bind(SpriteRenderer *renderer) {

}

void flush(SpriteRenderer *renderer) {
	if (renderer->size > 0) {
		//glBufferData(GL_ARRAY_BUFFER, renderer->capacity * sizeof(float), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->size * sizeof(float), renderer->buffer);
		glDrawArrays(GL_TRIANGLES, 0, renderer->size >> 3);
		renderer->size = 0;
	}
}

#define BB(x) buffer[size++] = x; 
#define VERTEX(x, y, tx, ty) BB(x)BB(y)BB(tx)BB(ty)BB(color->r)BB(color->g)BB(color->b)BB(color->a)
void draw(SpriteRenderer *renderer, Texture *texture, float x, float y, float w, float h, float tx, float ty, float tw, float th) {
	set_texture(renderer, texture);
	if (renderer->size + 8 * 6 > renderer->capacity) {
		flush(renderer);
	}
	w += x;
	h += y;
	tw += tx;
	th += ty;
	Color *color = &renderer->active_color;
	float *buffer = renderer->buffer;
	int size = renderer->size;
	VERTEX(x, h, tx, ty)
	VERTEX(x, y, tx, th)
	VERTEX(w, y, tw, th)
	VERTEX(x, h, tx, ty)
	VERTEX(w, y, tw, th)
	VERTEX(w, h, tw, ty)
	renderer->size = size;
}

void draw(SpriteRenderer *renderer, Texture *texture, float x, float y, float w, float h, float ox, float oy, float angle, float tx, float ty, float tw, float th) {
	set_texture(renderer, texture);
	if (renderer->size + 8 * 6 > renderer->capacity) {
		flush(renderer);
	}

  float s = sinf(angle);
  float c = cosf(angle);

  x -= ox;
  y -= oy;
	w += x;
	h += y;
	tw += tx;
	th += ty;

	Vec2 lb = {x * c - y * s + ox, x * s + y * c + oy};
	Vec2 lt = {x * c - h * s + ox, x * s + h * c + oy};
	Vec2 rb = {w * c - y * s + ox, w * s + y * c + oy};
	Vec2 rt = {w * c - h * s + ox, w * s + h * c + oy};

	Color *color = &renderer->active_color;
	float *buffer = renderer->buffer;
	int size = renderer->size;
	VERTEX(lt.x, lt.y, tx, ty)
	VERTEX(lb.x, lb.y, tx, th)
	VERTEX(rb.x, rb.y, tw, th)
	VERTEX(lt.x, lt.y, tx, ty)
	VERTEX(rb.x, rb.y, tw, th)
	VERTEX(rt.x, rt.y, tw, ty)
	renderer->size = size;
}

#undef BB
#undef VERTEX

void draw(SpriteRenderer *renderer, Texture *texture, float x, float y, float w, float h) {
	draw(renderer, texture, x, y, w, h, 0, 0, 1, 1);
}

void draw(SpriteRenderer *renderer, Font *font, char *text, float x, float y) {
	set_shader(renderer, font->shader);
	glUniform1f(font->u_smoothing, font->smoothing / font->drawh);
	glUniform1f(font->u_thickness, 1.f - font->thickness);

	float scale = font->drawh / font->height;
	float startx = x;

	unsigned char old = 0;
	char c;
	while ((c = *text) != '\0') {
		Character *cc = &font->chars[c];

		if (c == '\n') {
			x = startx;
			y -= (font->height + font->yspacing) * scale;
			old = c;
			text++;
			continue;
		}

		x += font->kerns[old + c * 256].amount * scale;

		draw(renderer,
			font->texture, 
			x + cc->xoff * scale, y - (cc->yoff + cc->h) * scale,
			cc->w * scale, cc->h * scale, 
			cc->x / (float)font->scaleW, cc->y / (float)font->scaleH,
			cc->w / (float)font->scaleW, cc->h / (float)font->scaleH
		);

		x += (cc->xadv + font->xspacing) * scale;
		old = c;
		text++;
	}
}

void set_color(SpriteRenderer *renderer, Color *color) {
	renderer->active_color = *color;

}

void set_shader(SpriteRenderer *renderer, Shader *shader) {
	flush(renderer);
	renderer->active_shader = shader;
	glUseProgram(shader->id);
	glUniformMatrix3fv(renderer->active_shader->u_transform, 1, GL_TRUE, renderer->active_matrix.m);
}

void set_texture(SpriteRenderer *renderer, Texture *texture) {
	if (texture->id != renderer->active_texture->id) {
		flush(renderer);
		glBindTexture(GL_TEXTURE_2D, texture->id);
		renderer->active_texture = texture;
	}
}

void set_matrix(SpriteRenderer *renderer, Mat3 *mat) {
	flush(renderer);
	renderer->active_matrix = *mat;
	glUniformMatrix3fv(renderer->active_shader->u_transform, 1, GL_TRUE, mat->m);
}