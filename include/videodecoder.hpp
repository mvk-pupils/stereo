#include "gl.h"

/// Different stereo video modes.
enum class VideoMode {
    /// Full side by side video.
    SIDE_BY_SIDE,
    /// Half side by side video (horizontal compression).
    HALF_SIDE_BY_SIDE,
};

/// A video frame containing certain auxiliary information.
struct Frame {
  /// OpenGL texture.
  GLuint texture = 0;
  /// Frame index.
  int number = -1;

  /// Width of the texture, in pixels.
  int width = -1;
  /// Height of the texture, in pixels.
  int height = -1;

  /// Stereo video mode of the frame.
  VideoMode mode;
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

  /// Returns the number of frames per second.
  virtual double frame_rate() = 0;
};
