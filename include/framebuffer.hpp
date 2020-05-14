#include <memory>

#include "gl.h"

/// A framebuffer for a specific eye.
class Framebuffer {
  private:
    struct Buffers {
      /// An identifier.
      GLuint framebuffer;

      /// The texture shown in the framebuffer.
      GLuint color;

      GLuint width, height;
    };

    Framebuffer(Buffers buffers);

    std::shared_ptr<Buffers> buffers;

  public:
    /// Create a new frame buffer.
    static Framebuffer create(int width, int height);

    /// Bind a texture to the framebuffer.
    void bind(GLuint target = GL_FRAMEBUFFER);
    /// Unbind a texture.
    void unbind();

    GLuint width();
    GLuint height();

    GLuint get_color_texture();
};
