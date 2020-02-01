#pragma once

#include "gl.h"

class Program {
  private:
    Program(GLuint program);

    GLuint program;

  public:
    ~Program();

    static Program compile(const char* vertex_source, const char* fragment_source);
};

