#include "stereo.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "log.hpp"

#include <stdexcept>
#include <stdio.h>

#include "util.hpp"
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

  return Stereo();
}

Stereo::Stereo() : 
  program(Program::compile(VERTEX_SOURCE, FRAGMENT_SOURCE)),
  mesh(Mesh::create()),
  texture(Texture::load("img/hqimg.png")),
  left(Framebuffer::create(600, 600)),
  right(Framebuffer::create(600, 600))
{
  Vertex vertices[] = {
    {{0.0, 0.5, 0.0}, {1.0, 0.0, 0.0, 1.0}, {0.47, 0.07}},
    {{0.5, -0.5, 0.0}, {0.0, 1.0, 0.0, 1.0}, {0.75, 0.81}},
    {{-0.5, -0.5, 0.0}, {0.0, 0.0, 1.0, 1.0}, {0.19, 0.81}},
  };

  GLuint indices[] = {
    0, 1, 2
  };

  this->mesh.set_vertices(sizeof(vertices)/sizeof(vertices[0]), vertices);
  this->mesh.set_indices(sizeof(indices)/sizeof(indices[0]), indices);
}

StereoView Stereo::draw(StereoViewport viewport) {
  UNUSED(viewport);

  this->program.use();
  this->texture.bind();

  // Left eye
  this->left.bind();

  auto left = viewport.left;
  glViewport(0, 0, this->left.width(), this->left.height());
  glClearColor(0.77, 0.62, 0.78, 1.0);
  this->render_scene(left);

  this->left.unbind();


  // Right eye
  this->right.bind();

  auto right = viewport.right;
  glViewport(0, 0, this->right.width(), this->right.height());
  glClearColor(0.2, 0.2, 0.2, 1.0);
  this->render_scene(right);

  this->right.unbind();


  // Blit to screen
  this->left.bind(GL_READ_FRAMEBUFFER);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(
      0, 0, this->left.width(), this->left.height(), 
      0, 0, left.width, left.height, 
      GL_COLOR_BUFFER_BIT, GL_LINEAR
    );

  this->right.bind(GL_READ_FRAMEBUFFER);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(
      0, 0, this->right.width(), this->right.height(), 
      left.width, 0, left.width + right.width, right.height, 
      GL_COLOR_BUFFER_BIT, GL_LINEAR
    );


  StereoView view;
  view.left.texture = this->left.get_color_texture();
  view.right.texture = this->right.get_color_texture();

  return view;
}

void Stereo::render_scene(Viewport viewport) {
  UNUSED(viewport);
  glClear(GL_COLOR_BUFFER_BIT);

  this->mesh.draw();
}
