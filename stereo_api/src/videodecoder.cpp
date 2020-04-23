#include "videodecoder.hpp"
#include <IconicVideo/GpuVideoDecoder.h>


Frame VideoDecoder::next_frame(){
  //get the next frame in the video, or `NULL` if there is none
}

set_playback(VideoDecoder*){
  //play | pause | ffw ...
}

int VideoDecoder::total_frames(){
  //return the total number of frames in the video
}

int VideoDecoder::frame_rate(GpuVideoDecoder decoder){
  //get the frame rate of the video (frames per second)

  return decoder.GetFrameRate();
}
