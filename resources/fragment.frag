#version 330 core

in vec3 v_color;

uniform float u_green;

void main() {
	gl_FragColor = vec4(v_color, 1);
	gl_FragColor.g *= u_green;
}
