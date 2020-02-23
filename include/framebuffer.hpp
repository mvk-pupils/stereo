
#include <memory>

#include "gl.h"

class Framebuffer {
  private:
    struct Buffers {
      GLuint framebuffer;
      GLuint color;

      GLuint width, height;
    };

    Framebuffer(Buffers buffers);

    std::shared_ptr<Buffers> buffers;

  public:
    static Framebuffer create(int width, int height);

    void bind(GLuint target = GL_FRAMEBUFFER);
    void unbind();

    GLuint width();
    GLuint height();

    GLuint get_color_texture();
};

