#include <GLFW/glfw3.h> 
#include <vector>
#include <memory>

enum class WindowMode {
    VISIBLE,
    HIDDEN,
};

struct WindowState {
	/// A list of the currently pressed keys.
	std::vector<int> pressed_keys;
};

/// A wnapper around a GLFW window.
class Window {
  private:
	Window();

    /// The handle to the window.
    GLFWwindow* handle;

	/// The current window state
	std::shared_ptr<WindowState> state;

  public:
    /// Open a new window with the specified size.
    ///
    /// @param width The width of the window.
    /// @param height The height of the window.
    static Window open(int width, int height, WindowMode mode = WindowMode::VISIBLE);

    /// Set this thread's OpenGL context to point to this window.
    void make_context_current();

    /// @returns A bool denoting wheter or not the window is still open.
    bool is_open();

    /// Close the window.
    void close();

    /// Stores the dimensions of the window's content (the framebuffer) in the
    /// input parameters.
    /// @param width The width of the window.
    /// @param height The height of the window.
    void get_inner_size(int* width, int* height);

    /// Collect all events that occured since the last time this method was
    /// called.
    void poll_events();

    /// Swap the front and back buffer of the window, displaying what has been
    /// rendered.
    void swap_buffers();

    /// @param key The key to poll.
    /// @returns Whether or not the key is pressed. 
    bool is_key_down(int key);

  private:
    /// Setup event handlers.
    void setup_window_callbacks();

    /// Called when a key event is genereted by a window.
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

