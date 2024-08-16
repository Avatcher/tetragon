#version 330 core

in vec3 color;
in vec3 pos;

out vec3 v_color;

void main() {
	gl_Position = vec4(pos, 1.0);
	v_color = color;
}
