#include "window.hpp"
#include <util.hpp>
#include <algorithm>

Window::Window() {
	this->handle = nullptr;
	this->state = std::make_shared<WindowState>(WindowState());
}

Window Window::open(int width, int height) {
  if (!glfwInit()) {
    throw 1;
  }

  Window window;

  // Set the minimum OpenGL context version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

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

void Window::get_inner_size(int* width, int* height) {
  glfwGetFramebufferSize(this->handle, width, height);
}

void Window::poll_events() {
  glfwPollEvents();
}

void Window::swap_buffers() {
  glfwSwapBuffers(this->handle);
}

bool Window::is_key_down(int key) {
  auto start = this->state->pressed_keys.begin();
  auto end = this->state->pressed_keys.end();
  return std::find(start, end, key) != end;
}

void Window::setup_window_callbacks() {
  // Store a pointer to the current `Window` class in the handle so that we can
  // reference it from the callbacks.
  glfwSetWindowUserPointer(this->handle, this->state.get());
  glfwSetKeyCallback(this->handle, Window::key_callback);
}

void Window::key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mode) {
  UNUSED(scancode);
  UNUSED(mode);

  auto state = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(glfw_window));
  auto& keys = state->pressed_keys;

  if (action == GLFW_PRESS) {
    keys.push_back(key);
  } else if (action == GLFW_RELEASE) {
    for (size_t i = keys.size() - 1; i >= 0; i--) {
      if (keys[i] == key) {
        util::swap_remove(keys, i);
      }
    }
  }
}
