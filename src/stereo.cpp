#include "stereo.hpp"
#include "shader.hpp"
#include "shadersource.hpp"
#include "mesh.hpp"
#include "log.hpp"

#include <stdexcept>
#include <stdio.h>

#include "util.hpp"
#include "gl.h"

#include <windows.h>

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

std::string get_executable_dir() {
    const auto MAX_SIZE = 1024;
    char raw_executable_path[MAX_SIZE] = { 0 };
    GetModuleFileNameA(NULL, raw_executable_path, MAX_SIZE);
    auto executable_path = std::string(raw_executable_path);
    auto last_slash = executable_path.find_last_of('\\');
    if (last_slash != std::string::npos) {
        return executable_path.substr(0, last_slash);
    }
    return executable_path;
}

void Stereo::display_video(VideoDecoder* decoder) {
  if (glewInit()) {
    ERROR("Failed to load OpenGL");
    throw std::runtime_error("Failed to load OpenGL");
  }

  INFO("Successfully loaded OpenGL context");
  INFO("Using OpenGL: %s", glGetString(GL_VERSION));
  INFO("Using GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

  auto openvr = init_openvr();
  unsigned int width = 0, height = 0;
  openvr->GetRecommendedRenderTargetSize(&width, &height);
  auto stereo = Stereo(width, height, decoder, openvr);

  auto executable_dir = get_executable_dir();
  auto action_path = executable_dir + "\\actions.json";
  DEBUG("ACTION: %s", action_path.c_str());
  vr::VRInput()->SetActionManifestPath(action_path.c_str());

  vr::VRActionHandle_t play_action, ffw_action, stop_action;
  vr::VRInput()->GetActionHandle("/actions/playback/in/play_pause", &play_action);
  vr::VRInput()->GetActionHandle("/actions/playback/in/fastforward", &ffw_action);
  vr::VRInput()->GetActionHandle("/actions/playback/in/stop", &stop_action);

  vr::VRActionSetHandle_t playback_set;
  vr::VRInput()->GetActionSetHandle("/actions/playback", &playback_set);


  Frame frame;
  while(true){
      vr::VRActiveActionSet_t active_set;
      active_set.ulActionSet = playback_set;
      active_set.ulRestrictedToDevice = vr::k_ulInvalidActionHandle;
      active_set.nPriority = 0;

      vr::VRInput()->UpdateActionState(&active_set, sizeof(active_set), 1);

      vr::InputDigitalActionData_t action_data;
      vr::VRInput()->GetDigitalActionData(play_action, &action_data, sizeof(action_data), vr::k_ulInvalidActionHandle);

      static Playback playback = Playback::PLAY;
      auto update_playback = [&](Playback next) {
          if (next != playback) decoder->set_playback(next), playback = next;
      };

      if (action_data.bChanged && action_data.bState) {
          if (playback != Playback::PAUSE) {
              update_playback(Playback::PAUSE);
          } else {
              update_playback(Playback::PLAY);
          }
      }

      vr::VRInput()->GetDigitalActionData(ffw_action, &action_data, sizeof(action_data), vr::k_ulInvalidActionHandle);

    if (action_data.bChanged && action_data.bState) {
        if (playback != Playback::FFW) {
            update_playback(Playback::FFW);
        } else {
            update_playback(Playback::PLAY);
        }
    }

    vr::VRInput()->GetDigitalActionData(stop_action, &action_data, sizeof(action_data), vr::k_ulInvalidActionHandle);

    if (action_data.bChanged && action_data.bState) {
        INFO("STOP");
        break;
    }

    auto next_frame = decoder->next_frame();
    if (next_frame.texture != 0) {
      frame = next_frame;
    }

    Viewport base;
    base.texture = frame.texture;
    base.aspect = 0.5 * float(frame.width) / float(frame.height);
    base.rectangle.left = 0.0f;
    base.rectangle.right = 1.0f;
    base.rectangle.top = 0.0f;
    base.rectangle.bottom = 1.0f;
      
    StereoViewport viewport;
    viewport.left = base;
    viewport.left.rectangle.right = 0.5f;
    viewport.right = base;
    viewport.right.rectangle.left = 0.5f;
    stereo.draw(viewport);
  }
}

Stereo::Stereo(int width, int height, VideoDecoder* decoder, vr::IVRSystem* openvr) :
  program(Program::compile(shader_vert, shader_frag)),
  left(Framebuffer::create(width / 2, height)),
  right(Framebuffer::create(width / 2, height)),
  openvr(openvr),
  decoder(decoder)
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

    float width;
    float height;

    if (viewport.aspect > 1.0) {
        width = 1.0;
        height = 1.0 / viewport.aspect;
    } else {
        width = viewport.aspect;
        height = 1.0;
    }

    float x = 0.0;
    float y = 0.0;

    float left = x - width / 2.0;
    float right = x + width / 2.0;
    float bottom = y - height / 2.0;
    float top = y + height / 2.0;

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
