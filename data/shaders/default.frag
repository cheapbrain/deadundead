#version 140

in vec2 a_tex_coord;
in vec4 a_color;

out vec4 gl_FragColor;

uniform sampler2D u_texture;
void main(void) {
	gl_FragColor = a_color * texture2D(u_texture, a_tex_coord);
}