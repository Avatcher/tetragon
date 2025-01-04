#version 330 core

in vec3 color;
in vec3 pos;
in vec2 texCoord;

out vec3 v_color;
out vec2 v_textureCoordinates;

uniform vec3 u_offset;

void main() {
	gl_Position = vec4(pos + u_offset, 1.0);
	v_color = color;
	v_textureCoordinates = texCoord;
}
