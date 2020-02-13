#pragma once

#include "gl.h"

#include <memory>

struct Vertex {
  float position[3];
  float color[4];
  float texture_coord[2];
};

class Mesh {
  private:
    Mesh();

    std::shared_ptr<GLuint> vertex_array;
    std::shared_ptr<GLuint> vertex_buffer;
    std::shared_ptr<GLuint> element_buffer;

    std::shared_ptr<GLuint> vertex_count;
    std::shared_ptr<GLuint> index_count;

    GLuint texture;

  public:
    static Mesh create();

    void set_vertices(GLuint count, Vertex* vertices);
    void set_indices(GLuint count, GLuint* indices);

    void load_texture(const char* path);

    void draw();

  private:
    void configure_buffers();
};
