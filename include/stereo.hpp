#pragma once

#include "shader.hpp"
#include "mesh.hpp"

/// Main entry point for the stereo display.
class Stereo {
  private:
    /// Create a new display.
    /// @param program The shader program.
    /// @param mesh The display mesh.
    Stereo(Program program, Mesh mesh);

    /// Shader program to display the screen.
    Program program;

    /// The vertices of the display.
    Mesh mesh;

  public:
    /// Initialize the library and construct a new handle.
    /// @returns A handle to the library.
    static Stereo init();

    /// Draw the stereo display.
    void draw();
};
