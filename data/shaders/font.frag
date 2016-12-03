#version 140

in vec2 a_tex_coord;
in vec4 a_color;

out vec4 gl_FragColor;

uniform sampler2D u_texture;

uniform float u_smoothing;
uniform float u_thickness;

void main(void) {
	float distance = texture2D(u_texture, a_tex_coord).a;
	float alpha = smoothstep(u_thickness - u_smoothing, u_thickness + u_smoothing, distance) * a_color.a;
	gl_FragColor = vec4(a_color.rgb, alpha);
}