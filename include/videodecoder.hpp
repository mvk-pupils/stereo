#include "gl.h"

struct Frame{

  GLuint texture = 0; //OpenGL texture
  int number; //frame index
  int width;
  int height;
};

enum Playback {
    PLAY, PAUSE, FFW
};

class VideoDecoder{
public:

  virtual Frame next_frame() = 0;

  virtual void set_playback(Playback) = 0;

  virtual int total_frames() = 0;

  virtual double frame_rate() = 0;

};
