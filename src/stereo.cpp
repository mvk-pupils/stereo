#include "stereo.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "log.hpp"

#include <stdexcept>
#include <stdio.h>

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
    // Cube
    {{0.5, 0.5, -0.5}, {1.0, 1.0, 1.0, 1.0}, {0.75, 0.25}},
    {{0.5, 0.5, 0.5}, {1.0, 1.0, 0.0, 1.0}, {0.5, 0.5}},
    {{0.5, -0.5, -0.5}, {1.0, 0.0, 1.0, 1.0}, {0.75, 0.75}},
    {{-0.5, 0.5, -0.5}, {0.0, 1.0, 1.0, 1.0}, {0.25, 0.25}},
    {{-0.5, -0.5, -0.5}, {0.0, 0.0, 1.0, 1.0}, {0.25, 0.75}},
    {{-0.5, 0.5, 0.5}, {0.0, 1.0, 0.0, 1.0}, {0.5, 0.5}},
    {{0.5, -0.5, 0.5}, {1.0, 0.0, 0.0, 1.0}, {0.5, 0.5}},
    {{-0.5, -0.5, 0.5}, {0.0, 0.0, 0.0, 1.0}, {0.5, 0.5}},

    // Triangle
    {{0.0, 0.5, 0.75}, {1.0, 0.0, 0.0, 1.0}, {0.47, 0.07}},
    {{0.5, -0.5, 0.75}, {0.0, 1.0, 0.0, 1.0}, {0.75, 0.81}},
    {{-0.5, -0.5, 0.75}, {0.0, 0.0, 1.0, 1.0}, {0.19, 0.81}},
  };

  GLuint triangle_indicies[] = {
      8, 9, 10,
  };

  GLuint cube_indices[] = {
    // Front
    0, 2, 3,
    2, 3, 4,
    // Right
    0, 1, 2,
    1, 2, 6,
    // Top
    0, 1, 3,
    1, 3, 5,
    // Left
    3, 4, 5,
    4, 5, 7,
    // Bottom
    2, 4, 6,
    4, 6, 7,
    // Back
    1, 5, 6,
    5, 6, 7,
  };

  mesh.set_vertices(sizeof(vertices)/sizeof(vertices[0]), vertices);
  mesh.set_indices(sizeof(cube_indices)/sizeof(cube_indices[0]), cube_indices);

  mesh.load_texture("img/hqimg.png");

  Stereo stereo = Stereo(program, mesh);
  return stereo;
}

Stereo::Stereo(Program program, Mesh mesh): program(program), mesh(mesh) {}

void Stereo::draw() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glClearColor(0.2, 0.2, 0.2, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->program.use();
  this->mesh.draw();
}
