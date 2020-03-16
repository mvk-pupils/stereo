#include <stdio.h>
#include <algorithm>
#include <exception>

#include <stereo.hpp>

#include "util.hpp"
#include "log.hpp"
#include "cli.hpp"
#include "window.hpp"

#include <openvr.h>

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

StereoViewport get_stereo_viewport(int width, int height) {
  StereoViewport viewport;

  auto left = &viewport.left;
  left->width = width / 2;
  left->height = height;

  auto right = &viewport.right;
  right->width = width / 2;
  right->height = height;

  return viewport;
}

int main(int argc, const char* argv[]) {
  auto arguments = parse_cli_arguments(argc, argv);

  try {
    printf("Stereo Example Executable (SEE)\n\n");

    vr::IVRSystem* openvr = nullptr;

    switch (arguments.display) {
      case MONITOR:
        // TODO: display the on the monitor
        break;
      case VR:
        // TODO: display the in the VR headset
        openvr = init_openvr();
		if (!vr::VRCompositor())
		{
			ERROR("Compositor initialization failed. See log file for details\n");
			return 1;
		}
        break;
    }

    auto window = Window::open(1200, 600);
    window.make_context_current();

    auto stereo = Stereo::init();

    while (window.is_open()) {
      window.poll_events();

      if (window.is_key_down(GLFW_KEY_ESCAPE)) {
        window.close();
        break;
      }

      int width, height;
      window.get_inner_size(&width, &height);
      auto viewport = get_stereo_viewport(width, height);

      auto views = stereo.draw(viewport);
      if (openvr) {
        // Taken from the OpenVR OpenGL example:
        vr::Texture_t left = { (void*)(uintptr_t)views.left.texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        vr::Texture_t right = { (void*)(uintptr_t)views.right.texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

		vr::VRCompositor()->WaitGetPoses(nullptr, 0, nullptr, 0);

        vr::VRCompositor()->Submit(vr::Eye_Left, &left);
        vr::VRCompositor()->Submit(vr::Eye_Right, &right);
      }

	  glFlush();
	  glFinish();

      window.swap_buffers();
    }

    printf("Bye\n");
  } catch (std::exception& e) {
    printf("Error: %s\n", e.what());
  } catch (const char* s) {
    printf("Program error: %s\n", s);
  } catch (...) {
    printf("There was an error.\n");
  }
}

