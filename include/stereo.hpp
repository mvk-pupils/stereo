#pragma once

#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"

/// A rasterized view of the scene.
struct View {
  /// The ID of a GL texture that contains the color data.
  GLuint texture;
};

/// A rasterized view of the scene, from the perspective of two eyes.
struct StereoView {
  View left;
  View right;
};

/// The position and size of a viewport (an eye).
struct Viewport {
  GLuint width, height;
};

/// The position and size of the eyes/cameras.
struct StereoViewport {
  Viewport left;
  Viewport right;
};

/// Main entry point for the stereo display.
class Stereo {
  private:
    /// Create a new display.
    Stereo();

    /// Shader program to display the screen.
    Program program;

    /// The vertices of the display.
    Mesh mesh;

    /// The texture to render on the display.
    Texture texture;

    /// Framebuffer for the left eye. 
    Framebuffer left;
    /// Framebuffer for the right eye. 
    Framebuffer right;

  public:
    /// Initialize the library and construct a new handle.
    /// @returns A handle to the library.
    static Stereo init();

    /// Draw the stereo display.
    /// @param viewport How to render each eye.
    /// @returns Two framebuffers, one for each eye.
    StereoView draw(StereoViewport viewport);

  private:
    void render_scene(Viewport viewport);
};
