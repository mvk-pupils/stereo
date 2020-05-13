#pragma once

#include <memory>

#include "gl.h"

/// A shader program (the combination of a vertex and fragment shader).
class Program {
  private:
    /// Create a new program from a raw OpenGL program handle.
    Program(GLuint program);

    /// A handle to an OpenGL program handle.
    /// Automatically deallocates the resource upon destruction.
    std::shared_ptr<GLuint> program;

  public:
    /// Compile a vertex and fragment shader.
    /// @param vertex_source The source code for a vertex.
    /// @param fragment_source The source code for a fragment shader.
    /// @returns An OpenGL shader program.
    static Program compile(const char* vertex_source, const char* fragment_source);

    /// Bind this program to the OpenGL context. Unbinds any previous shaders.
    void use();

    /// Get the location of a uniform in the shader.
    GLuint get_location(char* name);
};
