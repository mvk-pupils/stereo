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

out vec4 frag_color;

void main() {
  gl_Position = vec4(position, 1.0);
  frag_color = color;
}
)";

const char* FRAGMENT_SOURCE = R"(
#version 330 core

in vec4 frag_color;

out vec4 out_color;

void main() {
  out_color = frag_color;
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
    {{0.0, 0.5, 0.0}, {1.0, 0.0, 0.0, 1.0}},
    {{0.5, -0.5, 0.0}, {0.0, 1.0, 0.0, 1.0}},
    {{-0.5, -0.5, 0.0}, {0.0, 0.0, 1.0, 1.0}},
  };

  GLuint indices[] = {
    0, 1, 2
  };

  mesh.set_vertices(sizeof(vertices)/sizeof(vertices[0]), vertices);
  mesh.set_indices(sizeof(indices)/sizeof(indices[0]), indices);

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
