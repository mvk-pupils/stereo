#include <stdio.h>
#include <algorithm>
#include <exception>

#include <stereo.hpp>


#include "util.hpp"
#include "cli.hpp"
#include "window.hpp"

#include <openvr.h>

int main(int argc, const char* argv[]) {
  auto arguments = parse_cli_arguments(argc, argv);

  try {
    printf("Stereo Example Executable (SEE)\n\n");

    auto window = Window::open(800, 600);
    window.make_context_current();

    auto stereo = Stereo::init();

    /*
    vr::HmdError error;
    auto openvr = vr::VR_Init(&error, vr::VRApplication_Scene);

    if (error) {
      auto message = VR_GetVRInitErrorAsEnglishDescription(error);
      printf("ERROR: %s\n", message);
    }
    */

    while (window.is_open()) {
      window.poll_events();

      if (window.is_key_down(GLFW_KEY_ESCAPE)) {
        window.close();
        break;
      }

      stereo.draw();

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

