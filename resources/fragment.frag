#version 330 core

in vec3 v_color;

uniform float globalGreen;

void main() {
	gl_FragColor = vec4(v_color.x, globalGreen, v_color.z, 1);
}
