#include "GLFW/glfw3.h"
#include <vector> 

class Window {
  GLFWwindow* handle;
  std::vector<int> pressed_keys;

  public:
  static Window open(int width, int height);

  bool is_open();
  void close();

  void poll_events();

  bool is_key_down(int key);

  private:

  void setup_window_callbacks();

  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

