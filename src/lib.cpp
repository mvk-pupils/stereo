#include <iostream>
#include "lib.hpp"
#include <algorithm> 
#include "util.hpp"
#include <openvr.h>

void setup_window_callbacks(Window* window);

// Temporary test of OpenVR
void test_vr_init() {
  vr::HmdError error;
  auto vr_context = vr::VR_Init(&error, vr::EVRApplicationType::VRApplication_Scene);

  if (error) {
    std::cerr << "Failed to init OpenVR: " << vr::VR_GetVRInitErrorAsEnglishDescription(error) << std::endl;
  }
}

Window Window::open(int width, int height) {
  if (!glfwInit()) {
    throw 1;
  }

  test_vr_init();

  Window window;
  window.handle = glfwCreateWindow(width, height, "Stereo", NULL, NULL);

  window.setup_window_callbacks();

  return window;
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
