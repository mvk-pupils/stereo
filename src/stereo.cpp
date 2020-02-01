#include "stereo.hpp"

#include <stdexcept>
#include <stdio.h>

#include <glad/glad.h>

Stereo::Stereo() {}

Stereo Stereo::init() {
  Stereo stereo;

  if (!gladLoadGL()) {
    ERROR("Failed to load OpenGL");
    throw std::runtime_error("Failed to load OpenGL");
  }

  INFO("Successfully loaded OpenGL context");
  INFO("Using OpenGL: %s", glGetString(GL_VERSION));
  INFO("Using GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

  return stereo;
}

void Stereo::draw() {
  glClearColor(0.3, 0.6, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

