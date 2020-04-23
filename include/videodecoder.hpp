#include "texture.hpp"

struct Frame{

  Texture texture; //OpenGL texture
  int number; //frame index

};



class VideoDecoder{
public:

  virtual void next_frame();

  virtual void set_playback(Playback);

  virtual void total_frames();

  virtual void frame_rate(GpuVideoDecoder decoder);

};
