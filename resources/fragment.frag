#version 330 core

in vec3 v_color;
in vec2 v_textureCoordinates;

uniform float u_green;
uniform sampler2D u_texture;

void main() {
	gl_FragColor = texture(u_texture, v_textureCoordinates) * vec4(v_color, 1.0f);
}
