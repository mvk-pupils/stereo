#pragma once

#include <memory>

#include "gl.h"

class Program {
  private:
    Program(GLuint program);

    std::shared_ptr<GLuint> program;

  public:
    static Program compile(const char* vertex_source, const char* fragment_source);
};

