#version 140

in vec4 i_pos_tex;
in vec4 i_color;

out vec2 a_tex_coord;
out vec4 a_color;

uniform mat3 u_transform;

void main(void) {
	a_tex_coord = i_pos_tex.zw;
	a_color = i_color;
	gl_Position = vec4(u_transform * vec3(i_pos_tex.xy, 1.0), 1.0);
}