#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texture_coord;

out vec4 frag_color;
out vec2 frag_texture_coord;

uniform mat4 eye;
uniform mat4 proj;
uniform mat4 view;

void main() {
  gl_Position = transpose(proj) * inverse(eye) * inverse(view) * vec4(position, 1.0);
  frag_color = color;
  frag_texture_coord = texture_coord;
}