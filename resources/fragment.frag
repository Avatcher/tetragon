#version 330 core

in vec3 v_color;

uniform float u_green;

void main() {
	gl_FragColor = vec4(v_color.r, v_color.g * u_green, v_color.b, 1);
}
