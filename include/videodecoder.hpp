#include "gl.h"

struct Frame{

  GLuint texture; //OpenGL texture
  int number; //frame index
  int width;
  int height;
};

enum Playback {
  PLAY, PAUSE, FFW
}

class VideoDecoder{
public:

  virtual Frame next_frame();

  virtual void set_playback(Playback);

  virtual int total_frames();

  virtual int frame_rate();

};
