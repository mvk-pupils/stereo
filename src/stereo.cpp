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

#define M_PI 3.141592653589793238462643383279502884197169399375105820974

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texture_coord;

out vec4 frag_color;
out vec2 frag_texture_coord;

uniform float time;

vec3 axis_rotate(vec3 axis, vec3 coord, float angle) {
  float rot_cos = cos(angle), rot_sin = sin(angle);
  float inv_cos = (1 - rot_cos);
  mat3 rotate = mat3(
    vec3(
      rot_cos + axis.x*axis.x*inv_cos,
      axis.y*axis.x*inv_cos + axis.z*rot_sin,
      axis.z*axis.x*inv_cos - axis.y*rot_sin),
    vec3(
      axis.x*axis.y*inv_cos - axis.z*rot_sin,
      rot_cos + axis.y*axis.y*inv_cos,
      axis.z*axis.y*inv_cos + axis.x*rot_sin),
    vec3(
      axis.x*axis.z*inv_cos + axis.y*rot_sin,
      axis.y*axis.z*inv_cos - axis.x*rot_sin,
      rot_cos + axis.z*axis.z*inv_cos));
  return rotate * coord;
}

void main() {
  vec3 axis = vec3(-1,1,-1) / sqrt(3);
  gl_Position = vec4(axis_rotate(axis, position, M_PI/3), 1.0);
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
  vec4 tex_color = texture(frag_texture, frag_texture_coord);
  out_color = frag_color;
}
)";

Stereo Stereo::init(int width, int height) {
  if (glewInit()) {
    ERROR("Failed to load OpenGL");
    throw std::runtime_error("Failed to load OpenGL");
  }

  INFO("Successfully loaded OpenGL context");
  INFO("Using OpenGL: %s", glGetString(GL_VERSION));
  INFO("Using GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

  return Stereo(width, height);
}

Stereo::Stereo(int width, int height) : 
  program(Program::compile(VERTEX_SOURCE, FRAGMENT_SOURCE)),
  left(Framebuffer::create(width / 2, height)),
  right(Framebuffer::create(width / 2, height))
{
}

StereoView Stereo::draw(StereoViewport viewport) {
  this->program.use();

  // Left eye
  this->left.bind();
  
  glViewport(0, 0, this->left.width(), this->left.height());
  glClearColor(0.77f, 0.62f, 0.78f, 1.0f);
  this->render_scene(viewport.left);

  this->left.unbind();


  // Right eye
  this->right.bind();

  glViewport(0, 0, this->right.width(), this->right.height());
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  this->render_scene(viewport.right);

  this->right.unbind();


  // Blit to screen
  this->left.bind(GL_READ_FRAMEBUFFER);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(
      0, 0, this->left.width(), this->left.height(), 
      0, 0, viewport.left.width, viewport.left.height, 
      GL_COLOR_BUFFER_BIT, GL_LINEAR
    );

  this->right.bind(GL_READ_FRAMEBUFFER);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(
      0, 0, this->right.width(), this->right.height(), 
      viewport.left.width, 0, viewport.left.width + viewport.right.width, viewport.right.height, 
      GL_COLOR_BUFFER_BIT, GL_LINEAR
    );

  StereoView view;
  view.left.texture = this->left.get_color_texture();
  view.right.texture = this->right.get_color_texture();

  return view;
}

void Stereo::render_scene(Viewport viewport) {
  glClear(GL_COLOR_BUFFER_BIT);
  ScissorRectangle r = viewport.rectangle;

  Vertex vertices[] = {
      {{-1, 1, 0}, {1,1,1,1}, {r.left, r.top}},
      {{-1, -1, 0}, {1,1,1,1}, {r.left, r.bottom}},
      {{1, -1, 0}, {1,1,1,1}, {r.right, r.bottom}},
      {{1, 1, 0}, {1,1,1,1}, {r.right, r.top}},
  };

  GLuint indices[] = { 
      // Triangle, lower left
      0, 1, 2, 
      // Triangle, upper right
      2, 3, 0
  };

  Mesh mesh = Mesh::create();
  mesh.set_vertices(4, vertices);
  mesh.set_indices(6, indices);

  glBindTexture(GL_TEXTURE_2D, viewport.texture);
  mesh.draw();
}
