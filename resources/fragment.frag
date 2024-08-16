#version 330 core

in vec3 v_color;

uniform float u_green;

void main() {
	gl_FragColor = vec4(v_color.x, u_green, v_color.z, 1);
}
