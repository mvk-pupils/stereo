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

uniform mat4 eye;
uniform mat4 proj;

out vec2 frag_texture_coord;

void main() {
  gl_Position = transpose(proj) * inverse(eye) * vec4(position, 1.0);
  frag_texture_coord = texture_coord;
}
)";

const char* FRAGMENT_SOURCE = R"(
#version 330 core

in vec2 frag_texture_coord;

out vec4 out_color;

uniform sampler2D frag_texture;

void main() {
  out_color = texture(frag_texture, frag_texture_coord);
}
)";

/// Initialize OpenVR (kinda broken on MacOS and Linux right now)
vr::IVRSystem* init_openvr() {
    vr::HmdError error;
    auto openvr = vr::VR_Init(&error, vr::VRApplication_Scene);

    if (error) {
        auto message = VR_GetVRInitErrorAsEnglishDescription(error);
        printf("ERROR: %s\n", message);
    }

    return openvr;
}

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
  right(Framebuffer::create(width / 2, height)),
  openvr(init_openvr())
{
}

StereoView Stereo::draw(StereoViewport viewport) {
  this->program.use();

  // Left eye
  this->left.bind();
  
  glViewport(0, 0, this->left.width(), this->left.height());
  glClearColor(0.77f, 0.62f, 0.78f, 1.0f);
  this->render_scene(viewport.left, vr::Eye_Left);

  this->left.unbind();


  // Right eye
  this->right.bind();

  glViewport(0, 0, this->right.width(), this->right.height());
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  this->render_scene(viewport.right, vr::Eye_Right);

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


  // Display to the vr headset
  if (this->openvr) {
      vr::Texture_t left = { (void*)(uintptr_t)view.left.texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
      vr::Texture_t right = { (void*)(uintptr_t)view.right.texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

      vr::VRCompositor()->WaitGetPoses(nullptr, 0, nullptr, 0);

      vr::VRCompositor()->Submit(vr::Eye_Left, &left);
      vr::VRCompositor()->Submit(vr::Eye_Right, &right);
  }

  return view;
}

void Stereo::render_scene(Viewport viewport, vr::Hmd_Eye eye) {
    glClear(GL_COLOR_BUFFER_BIT);
    ScissorRectangle r = viewport.rectangle;

    float width = 1.0;
    float height = 1.0;

    float left = -0.5;
    float top = 0.33;
    float right = left + width;
    float bottom = top - height;

    float depth = 0.8;

    Vertex vertices[] = {
        {{left, top, -depth}, {1,1,1,1}, {r.left, r.top}},
        {{left, bottom, -depth}, {1,1,1,1}, {r.left, r.bottom}},
        {{right, bottom, -depth}, {1,1,1,1}, {r.right, r.bottom}},
        {{right, top, -depth}, {1,1,1,1}, {r.right, r.top}},
    };

    GLuint indices[] = {
        // Triangle, lower left
        0, 1, 2,
        // Triangle, upper right
        2, 3, 0
    };

    auto eth = this->openvr->GetEyeToHeadTransform(eye);
    float view_matrix[16] = {
        eth.m[0][0], eth.m[1][0], eth.m[2][0], 0.0f,
        eth.m[0][1], eth.m[1][1], eth.m[2][1], 0.0f,
        eth.m[0][2], eth.m[1][2], eth.m[2][2], 0.0f,
        eth.m[0][3], eth.m[1][3], eth.m[2][3], 1.0f,
    };

    auto projection_matrix = this->openvr->GetProjectionMatrix(eye, 0.1, 5.0);

    auto eye_location = this->program.get_location("eye");
    glUniformMatrix4fv(eye_location, 1, false, view_matrix);

    auto proj_location = this->program.get_location("proj");
    glUniformMatrix4fv(proj_location, 1, false, &projection_matrix.m[0][0]);

    Mesh mesh = Mesh::create();
    mesh.set_vertices(4, vertices);
    mesh.set_indices(6, indices);

    glBindTexture(GL_TEXTURE_2D, viewport.texture);
    mesh.draw();
}
