#pragma once
#include "shader.hpp"

class Stereo {
  private:
    Stereo(Program program);

    Program program;

  public:
    static Stereo init();

    void draw();
};
