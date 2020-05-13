#pragma once

#include "gl.h"

#include <memory>

/// Wrapper for an OpenGL texture.
class Texture {
  private:
    Texture(GLuint texture);

    /// Handle to the OpenGL texture.
    std::shared_ptr<GLuint> texture;

  public:

    /// Load a texture from an image.
    /// @param path The path to the image.
    static Texture load(const char* path);

    /// Bind the texture to the OpenGL context.
    void bind();
};
