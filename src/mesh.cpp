#include "mesh.hpp"
#include "log.hpp"

#include <stddef.h>

Mesh::Mesh() {
  this->vertex_count = std::make_shared<GLuint>(0);
  this->index_count = std::make_shared<GLuint>(0);
}

Mesh Mesh::create() {
  GLuint* vertex_array = new GLuint(0);
  GLuint* vertex_buffer = new GLuint(0);
  GLuint* element_buffer = new GLuint(0);

  glGenVertexArrays(1, vertex_array);
  glGenBuffers(1, vertex_buffer);
  glGenBuffers(1, element_buffer);

  Mesh mesh;
  mesh.vertex_array = std::shared_ptr<GLuint>(
      vertex_array,
      [](GLuint* v){
        DEBUG("Deleting vertex array (%d)", *v);
        glDeleteVertexArrays(1, v);
      }
    );

  mesh.vertex_buffer = std::shared_ptr<GLuint>(
      vertex_buffer,
      [](GLuint* v){
        DEBUG("Deleting vertex buffer (%d)", *v);
        glDeleteBuffers(1, v);
      }
    );

  mesh.element_buffer = std::shared_ptr<GLuint>(
      element_buffer,
      [](GLuint* v){
        DEBUG("Deleting element buffer (%d)", *v);
        glDeleteBuffers(1, v);
      }
    );

  mesh.configure_buffers();

  return mesh;
}

void Mesh::configure_buffers() {
  glBindVertexArray(*this->vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, *this->vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->element_buffer);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
      0,
      sizeof(Vertex::position) / sizeof(float),
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex), (GLvoid *)offsetof(Vertex, position)
    );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1,
      sizeof(Vertex::color) / sizeof(float),
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex), (GLvoid *)offsetof(Vertex, color)
    );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(
      2,
      sizeof(Vertex::texture_coord) / sizeof(float),
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex), (GLvoid *)offsetof(Vertex, texture_coord)
    );
}

void Mesh::set_vertices(GLuint count, Vertex* vertices) {
  glBindVertexArray(*this->vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, *this->vertex_buffer);

  glBufferData(GL_ARRAY_BUFFER, count * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);
  *this->vertex_count = count;
}

void Mesh::set_indices(GLuint count, GLuint* indices) {
  glBindVertexArray(*this->vertex_array);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->element_buffer);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(indices[0]), indices, GL_STATIC_DRAW);
  *this->index_count = count;
}

void Mesh::draw() {
  glBindVertexArray(*this->vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, *this->vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->element_buffer);

  glDrawElements(GL_TRIANGLES, *this->index_count, GL_UNSIGNED_INT, 0);
}
