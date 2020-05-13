#include "gl.h"

/// A video frame containing certain auxiliary information.
struct Frame {
  /// OpenGL texture.
  GLuint texture = 0;
  /// Frame index.
  int number;

  int width;
  int height;
};

/// The state of the video player: playing, paused, or in fast forward mode.
enum class Playback {
  PLAY, PAUSE, FFW
};

/// Represents the application of a video decoder to a specific video file.
/// Provided by the user and passed to the stereo library.
/// @see Stereo.display_video().
class VideoDecoder {
public:

  /// Returns the frame after the current one.
  /// If there is no next frame, a Frame object with `texture` set to 0 should be returned.
  virtual Frame next_frame() = 0;

  virtual void set_playback(Playback) = 0;

  /// Returns the number of frames in the whole video.
  virtual int total_frames() = 0;

  /// Returns the number of frames per second.
  virtual double frame_rate() = 0;

};
