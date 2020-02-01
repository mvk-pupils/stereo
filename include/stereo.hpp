#pragma once
#include "log.hpp"

class Stereo {
  private:
    Stereo();

  public:
    static Stereo init();

    void draw();
};
