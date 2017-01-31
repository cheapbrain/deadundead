#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

void draw(SpriteRenderer *renderer, Texture *texture, 
		float x, float y, float w, float h, 
		float ox, float oy, float a_sin, float a_cos, 
		float tx, float ty, float tw, float th, 
		int flip, int rotate_texture) 
{
	set_texture(renderer, texture);
	if (renderer->size + 8 * 6 > renderer->capacity) {
		flush(renderer);
	}

	float s = a_sin;
	float c = a_cos;

	x -= ox;
	y -= oy;
	w += x;
	h += y;
	tw += tx;
	th += ty;

	Vec2 lb = {x * c - y * s, x * s + y * c + oy};
	Vec2 lt = {x * c - h * s, x * s + h * c + oy};
	Vec2 rb = {w * c - y * s, w * s + y * c + oy};
	Vec2 rt = {w * c - h * s, w * s + h * c + oy};

	if (flip) {
		lb.x = -lb.x;
		lt.x = -lt.x;
		rb.x = -rb.x;
		rt.x = -rt.x;
	}

	lb.x += ox;
	lt.x += ox;
	rb.x += ox;
	rt.x += ox;

	Color *color = &renderer->active_color;
	float *buffer = renderer->buffer;
	int size = renderer->size;

	if (rotate_texture) {
		VERTEX(lt.x, lt.y, tw, ty)
		VERTEX(lb.x, lb.y, tx, ty)
		VERTEX(rb.x, rb.y, tx, th)
		VERTEX(lt.x, lt.y, tw, ty)
		VERTEX(rb.x, rb.y, tx, th)
		VERTEX(rt.x, rt.y, tw, th)
	} else {
		VERTEX(lt.x, lt.y, tx, ty)
		VERTEX(lb.x, lb.y, tx, th)
		VERTEX(rb.x, rb.y, tw, th)
		VERTEX(lt.x, lt.y, tx, ty)
		VERTEX(rb.x, rb.y, tw, th)
		VERTEX(rt.x, rt.y, tw, ty)
	}
	
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

Vec2 rotate(Vec2 *v, float a_sin, float a_cos) {
	Vec2 out;
	out.x = v->x * a_cos - v->y * a_sin;
	out.y = v->x * a_sin + v->y * a_cos;
	return out;
}

Vec2 rotate(Vec2 *v, Vec2 *o, float a_sin, float a_cos) {
	Vec2 out;

	v->x = v->x - o->x;
	v->y = v->y - o->y;

	out.x = v->x * a_cos - v->y * a_sin + o->x;
	out.y = v->x * a_sin + v->y * a_cos + o->y;

	return out;
}

#define PId180 0.01745329251f
#define SIN(ANGLE) (sinf(ANGLE * PId180))
#define COS(ANGLE) (cosf(ANGLE * PId180))

float lerp(float a, float b, float k) {
	return a * (1 - k) + b * k;
}

float angle_lerp(float a, float b, float k, int spin) {
	if (spin == 0) return a;
	if (spin > 0 && b - a < 0) b += 360;
	if (spin < 0 && b - a > 0) b -= 360;
	return lerp(a, b, k);
}

void absolute_coordinates(
		SpriterAnimation *anim, SpriterAnimationKey *anim_key, SpriterObjectRef *obj, 
		float *scaleX, float *scaleY, Vec2 *origin, float *a_sin, float *a_cos, float *alpha, float time) {

	if (obj->parent >= 0) {
		SpriterObjectRef *parent = (SpriterObjectRef *)list_get(&anim_key->bones, obj->parent);
		absolute_coordinates(anim, anim_key, parent, scaleX, scaleY, origin, a_sin, a_cos, alpha, time);
	}

	SpriterTimeline *timeline = (SpriterTimeline *)list_get(&anim->timelines, obj->timeline);
	SpriterTimelineKey *key = (SpriterTimelineKey *)list_get(&timeline->keys, obj->key);
	SpriterTimelineKey *next_key = (SpriterTimelineKey *)list_get(&timeline->keys, (obj->key + 1) % timeline->keys.count);
	float time1 = (float)key->time;
	float time2 = (float)next_key->time;
	if (time <= time1) time += anim->length;
	if (time2 <= time1) time2 += anim->length;
	float k = (time - time1) / (time2 - time1);
	
	float angle = angle_lerp(key->angle, next_key->angle, k, key->spin);

	float x = lerp(key->x, next_key->x, k);
	float y = lerp(key->y, next_key->y, k);
	float a = lerp(key->a, next_key->a, k);
	float scale_x = lerp(key->scale_x, next_key->scale_x, k);
	float scale_y = lerp(key->scale_y, next_key->scale_y, k);

	float o_sin = SIN(angle);
	float o_cos = COS(angle);
	float s_sin = *a_sin * o_cos + *a_cos * o_sin;
	float s_cos = *a_cos * o_cos - *a_sin * o_sin;

	Vec2 pos = {x * *scaleX, y * *scaleY};
	pos = rotate(&pos, *a_sin, *a_cos);
	*a_sin = s_sin;
	*a_cos = s_cos;
	origin->x += pos.x;
	origin->y += pos.y;
	*alpha *= a;
	*scaleX *= scale_x;
	*scaleY *= scale_y;
}

void draw(SpriteRenderer *renderer, SpriterInstance *spriter_instance) {
	float c_x = spriter_instance->x;
	float c_y = spriter_instance->y;
	float c_scaleX = spriter_instance->scale_x;
	float c_scaleY = spriter_instance->scale_y;

	int animation_id = spriter_instance->active_animation;

	SpriterAnimation *animation = (SpriterAnimation *)list_get(&spriter_instance->character->animations, animation_id);
	SpriterAnimationKey *anim_keys = (SpriterAnimationKey *)animation->animation_keys.array;

	spriter_instance->animation_time = fmodf(spriter_instance->animation_time, (float)animation->length);
	float animation_time = spriter_instance->animation_time;

	int first = 0;
	int last = animation->animation_keys.count - 1;
	while (1) {
		int mid = (first + last) >> 1;
		SpriterAnimationKey *mid_key = anim_keys + mid;
		if (last - first == 1) break;
		if (mid_key->time <= animation_time) first = mid;
		else last = mid;
	}
	if ((anim_keys + last)->time <= animation_time) first = last;


	SpriterAnimationKey *anim_key = anim_keys + first;

	for (int i = 0; i < anim_key->images.count; i++) {
		SpriterObjectRef *obj = (SpriterObjectRef *)list_get(&anim_key->images, i);
		float scaleX = c_scaleX;
		float scaleY = c_scaleY;
		Vec2 origin = {c_x, c_y};
		float a_sin = 0;
		float a_cos = 1;
		float alpha = 1;
		absolute_coordinates(animation, anim_key, obj, &scaleX, &scaleY, &origin, &a_sin, &a_cos, &alpha, animation_time);

		SpriterTimeline *timeline = (SpriterTimeline *)list_get(&animation->timelines, obj->timeline);
		SpriterTimelineKey *key = (SpriterTimelineKey *)list_get(&timeline->keys, obj->key);
		SpriterFolder *folder = (SpriterFolder *)list_get(&spriter_instance->character->folders, key->folder);
		SpriterTexture *file = (SpriterTexture *)list_get(&folder->files, key->file);

		if (spriter_instance->flip) origin.x = c_x + c_x - origin.x;

		float w = file->width * scaleX;
		float h = file->height * scaleY;
		float x = origin.x - file->pivot_x * w;
		float y = origin.y - file->pivot_y * h;
		set_color(renderer, 1, 1, 1, alpha);
		draw(renderer, file->texture, x, y, w, h, origin.x, origin.y, a_sin, a_cos, file->t_x, file->t_y, file->t_w, file->t_h, spriter_instance->flip, file->rotate_texture);
	}
}

void set_color(SpriteRenderer *renderer, Color *color) {
	renderer->active_color = *color;
}

void set_color(SpriteRenderer *renderer, float r, float g, float b, float a) {
	renderer->active_color.r = r;
	renderer->active_color.g = g;
	renderer->active_color.b = b;
	renderer->active_color.a = a;
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