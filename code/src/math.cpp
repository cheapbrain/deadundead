#include "utils.h"

void identity(Mat3 *mat) {
	mat->m[0] = 1;
	mat->m[1] = 0;
	mat->m[2] = 0;
	mat->m[3] = 0;
	mat->m[4] = 1;
	mat->m[5] = 0;
	mat->m[6] = 0;
	mat->m[7] = 0;
	mat->m[8] = 1;
}

void orthographic(Mat3 *mat, float height, float left, float bottom) {
	height = 2.f / height;
	float width = height * (10.f / 17.f);
	mat->m[0] = width;
	mat->m[1] = 0;
	mat->m[2] = width * left - 1.f;
	mat->m[3] = 0;
	mat->m[4] = height;
	mat->m[5] = height * bottom - 1.f;
	mat->m[6] = 0;
	mat->m[7] = 0;
	mat->m[8] = 1.f;
}