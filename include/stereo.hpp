#pragma once

#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"

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

  public:
    /// Initialize the library and construct a new handle.
    /// @returns A handle to the library.
    static Stereo init();

    /// Draw the stereo display.
    void draw();
};
