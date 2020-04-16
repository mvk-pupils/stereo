#include "framebuffer.hpp"
#include <iostream>
#include "log.hpp"

Framebuffer::Framebuffer(Buffers buffers) {
  this->buffers = std::shared_ptr<Buffers>(
      new Buffers(buffers),
      [](Buffers* b){
        DEBUG("Deleting texture (%d)", b->color);
        glDeleteTextures(1, &b->color);

        DEBUG("Deleting framebuffer (%d)", b->framebuffer);
        glDeleteFramebuffers(1, &b->framebuffer);
      }
    );
}

Framebuffer Framebuffer::create(int width, int height) {
  Buffers buffers;
  buffers.width = width;
  buffers.height = height;

  glGenFramebuffers(1, &buffers.framebuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, buffers.framebuffer);

  glGenTextures(1, &buffers.color);
  glBindTexture(GL_TEXTURE_2D, buffers.color);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffers.color, 0);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    ERROR("Framebuffer is not complete!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return Framebuffer(buffers);
}

void Framebuffer::bind(GLuint target) {
  glBindFramebuffer(target, this->buffers->framebuffer);
}

void Framebuffer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::width() {
  return this->buffers->width;
}

GLuint Framebuffer::height() {
  return this->buffers->height;
}

GLuint Framebuffer::get_color_texture() {
  return this->buffers->color;
}
