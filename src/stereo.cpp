#include "stereo.hpp"
#include "shader.hpp"

#include <stdexcept>
#include <stdio.h>

#include "gl.h"

const char* VERTEX_SOURCE = R"(
#version 330 core

void main() {
  
}
)";

const char* FRAGMENT_SOURCE = R"(
#version 330 core

void main() {

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

Program::~Program() {
  glDeleteProgram(this->program);
}
