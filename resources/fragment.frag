#version 330 core

in vec3 v_color;
in vec2 v_textureCoordinates;

uniform float u_green;
uniform vec3 u_offset;

uniform sampler2D u_textureBricks;
uniform sampler2D u_texturePaint;

void main() {
	gl_FragColor = mix(
		texture(u_textureBricks, v_textureCoordinates),
		texture(u_texturePaint, v_textureCoordinates) * vec4(v_color, 1.0f),
		u_offset.x + 0.5
	);
}
