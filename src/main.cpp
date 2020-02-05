#include <stdio.h>
#include <algorithm>
#include <exception>

#include <stereo.hpp>

#include <GLFW/glfw3.h>

#include "util.hpp"

class Window {
  private:
    GLFWwindow* handle;
    std::vector<int> pressed_keys;

  public:
    static Window open(int width, int height);

    void make_context_current();

    bool is_open();
    void close();

    void poll_events();
    void swap_buffers();

    bool is_key_down(int key);

  private:
    void setup_window_callbacks();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

void setup_window_callbacks(Window* window);

Window Window::open(int width, int height) {
  if (!glfwInit()) {
    throw 1;
  }

  Window window;

  // Set the minimum OpenGL context version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  glfwWindowHint(GLFW_SAMPLES, 8);

  window.handle = glfwCreateWindow(width, height, "Stereo Example Executable", NULL, NULL);

  window.setup_window_callbacks();

  return window;
}

void Window::make_context_current() {
  glfwMakeContextCurrent(this->handle);
}

bool Window::is_open() {
  return this->handle && !glfwWindowShouldClose(this->handle);
}

void Window::close() {
  glfwDestroyWindow(this->handle);
  this->handle = nullptr;
}

void Window::poll_events() {
  glfwPollEvents();
}

void Window::swap_buffers() {
  glfwSwapBuffers(this->handle);
}

bool Window::is_key_down(int key) {
  auto start = this->pressed_keys.begin();
  auto end = this->pressed_keys.end();
  return std::find(start, end, key) != end;
}

void Window::setup_window_callbacks() {
  // Store a pointer to the current `Window` class in the handle so that we can
  // reference it from the callbacks.
  glfwSetWindowUserPointer(this->handle, this);
  glfwSetKeyCallback(this->handle, Window::key_callback);
}

void Window::key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mode) {
  auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  auto& keys = window->pressed_keys;

  if (action == GLFW_PRESS) {
    keys.push_back(key);
  } else if (action == GLFW_RELEASE) {
    for (int i = keys.size() - 1; i >= 0; i--) {
      if (keys[i] == key) {
        util::swap_remove(keys, i);
      }
    }
  }
}

int main() {
  try {
    printf("Stereo Example Executable (SEE)\n\n");

    auto window = Window::open(800, 600);
    window.make_context_current();

    auto stereo = Stereo::init();

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
