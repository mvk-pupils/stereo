#include "stereo.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "log.hpp"

#include <stdexcept>
#include <stdio.h>

#include "gl.h"

const char* VERTEX_SOURCE = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texture_coord;

out vec4 frag_color;
out vec2 frag_texture_coord;

void main() {
  gl_Position = vec4(position, 1.0);
  frag_color = color;
  frag_texture_coord = texture_coord;
}
)";

const char* FRAGMENT_SOURCE = R"(
#version 330 core

in vec4 frag_color;
in vec2 frag_texture_coord;

out vec4 out_color;

uniform sampler2D frag_texture;

void main() {
  out_color = texture(frag_texture, frag_texture_coord);
}
)";

Stereo Stereo::init() {
  if (!gladLoadGL()) {
    ERROR("Failed to load OpenGL");
    throw std::runtime_error("Failed to load OpenGL");
  }

  INFO("Successfully loaded OpenGL context");
  INFO("Using OpenGL: %s", glGetString(GL_VERSION));
  INFO("Using GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

  auto program = Program::compile(VERTEX_SOURCE, FRAGMENT_SOURCE);
  auto mesh = Mesh::create();

  Vertex vertices[] = {
    {{0.0, 0.5, 0.0}, {1.0, 0.0, 0.0, 1.0}, {0.47, 0.07}},
    {{0.5, -0.5, 0.0}, {0.0, 1.0, 0.0, 1.0}, {0.75, 0.81}},
    {{-0.5, -0.5, 0.0}, {0.0, 0.0, 1.0, 1.0}, {0.19, 0.81}},
  };

  GLuint indices[] = {
    0, 1, 2
  };

  mesh.set_vertices(sizeof(vertices)/sizeof(vertices[0]), vertices);
  mesh.set_indices(sizeof(indices)/sizeof(indices[0]), indices);

  mesh.load_texture("img/hqimg.png");

  Stereo stereo = Stereo(program, mesh);
  return stereo;
}

Stereo::Stereo(Program program, Mesh mesh): program(program), mesh(mesh) {}

void Stereo::draw() {
  glClearColor(0.2, 0.2, 0.2, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  this->program.use();
  this->mesh.draw();
}
