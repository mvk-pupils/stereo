#include <stdio.h>
#include <algorithm> 
#include <exception>

#include <stereo.hpp>

#include <GLFW/glfw3.h>

#include "util.hpp"

/// A wnapper around a GLFW window.
class Window {
  private:
    /// The handle to the window.
    GLFWwindow* handle;

    /// A list of the currently pressed keys.
    std::vector<int> pressed_keys;

  public:
    /// Open a new window with the specified size.
    ///
    /// @param width The width of the window.
    /// @param height The height of the window.
    static Window open(int width, int height);

    /// Set this thread's OpenGL context to point to this window.
    void make_context_current();

    /// @returns A bool denoting wheter or not the window is still open.
    bool is_open();

    /// Close the window.
    void close();

    /// Collect all events that occured since the last time this method was
    /// called.
    void poll_events();

    /// Swap the front and back buffer of the window, displaying what has been
    /// rendered.
    void swap_buffers();

    /// @params key The key to poll.
    /// @returns Whether or not the key is pressed. 
    bool is_key_down(int key);

  private:
    /// Setup event handlers.
    void setup_window_callbacks();

    /// Called when a key event is genereted by a window.
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

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
  } catch (...) {
    printf("There was an error\n");
  }
}
