#pragma once

#include "gl.h"

#include <memory>

/// The representation of a vertex.
struct Vertex {
  /// The position of the vertex in 3D space.
  float position[3];
  /// The color of the vertex in RGBA format (normalized in the range 0-1).
  float color[4];
  /// Coordinate to sample the bound texture from (normalized in the range 0-1).
  float texture_coord[2];
};

/// A handle to a mesh (a list triangles and its vertices).
class Mesh {
  private:
    /// Construct an empty mesh.
    Mesh();

    /// Handle to the OpenGL Vertex Array Object (VAO).
    /// Automatically cleans up any resources upon destruction.
    std::shared_ptr<GLuint> vertex_array;

    /// Handle to the OpenGL Vertex Buffer Object (VBO).
    /// Automatically cleans up any resources upon destruction.
    std::shared_ptr<GLuint> vertex_buffer;

    /// Handle to the OpenGL Element Buffer Object.
    /// Automatically cleans up any resources upon destruction.
    std::shared_ptr<GLuint> element_buffer;

    /// The number of vertices in the mesh.
    std::shared_ptr<GLuint> vertex_count;

    /// The number of indices in the mesh.
    std::shared_ptr<GLuint> index_count;

  public:
    /// Create a new, empty, mesh.
    static Mesh create();

    /// Upload a list of vertices to the mesh.
    void set_vertices(GLuint count, Vertex* vertices);

    /// Upload a list of indices to the mesh.
    void set_indices(GLuint count, GLuint* indices);

    /// Draw the mesh.
    void draw();

  private:
    /// Setup the vertex and element buffers.
    void configure_buffers();
};
