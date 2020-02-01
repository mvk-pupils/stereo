#pragma once

#include "shader.hpp"
#include "mesh.hpp"

class Stereo {
  private:
    Stereo(Program program, Mesh mesh);

    Program program;
    Mesh mesh;

  public:
    static Stereo init();

    void draw();
};
