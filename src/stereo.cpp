#include "stereo.hpp"
#include "shader.hpp"
#include "log.hpp"

#include <stdexcept>
#include <stdio.h>

#include "gl.h"

const char* VERTEX_SOURCE = R"(
#version 330 core

layout (location = 0) in vec3 position;

void main() {
  gl_Position = vec4(position, 1.0);
}
)";

const char* FRAGMENT_SOURCE = R"(
#version 330 core

out vec4 out_color;

void main() {
  out_color = vec4(0.0, 1.0, 1.0, 1.0);
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

  Stereo stereo = Stereo(program);

  return stereo;
}

Stereo::Stereo(Program program): program(program) {}

void Stereo::draw() {
  glClearColor(0.3, 0.6, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}
