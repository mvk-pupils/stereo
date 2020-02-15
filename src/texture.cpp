#include "texture.hpp"

#include "stb_image.h"

#include "log.hpp"

Texture::Texture(GLuint texture) {
  this->texture = std::shared_ptr<GLuint>(
      new GLuint(texture),
      [](GLuint* v){
        DEBUG("Deleting texture (%d)", *v);
        glDeleteTextures(1, v);
      }
    );
}

Texture Texture::load(const char *path) {
  GLuint texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    throw "Failed to load texture";
  }

  stbi_image_free(data);

  return Texture(texture);
}

void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, *this->texture);
}

