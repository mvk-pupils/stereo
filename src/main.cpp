#include <stdio.h>
#include <lib.hpp>

int main() {
  printf("Stereo\n");

  auto window = Window::open(800, 600);

  while (window.is_open()) {
    window.poll_events();

    if (window.is_key_down(GLFW_KEY_ESCAPE)) {
      window.close();
    }
  }

  printf("Bye\n");
}
