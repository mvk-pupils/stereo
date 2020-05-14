#include <stdio.h>
#include <algorithm>
#include <exception>
#include <chrono>

#include <IconicVideo/GpuVideoDecoder.h>
#include <IconicMatch/Defines.h>
#include <IconicMatch/GpuStream.h>
#include <IconicMatch/GpuProcessor.h>
#include <IconicGpu/GpuContext.h>

#include <stereo.hpp>

#include "util.hpp"
#include "log.hpp"
#include "cli.hpp"
#include "window.hpp"

/// Canvas represents a canvas which the textures from the video decoding is drawn to.
///
/// Canvas inherits from ImageGLBase, which provides functions render, refresh, SwapBuffers and SetCurrent().
class Canvas : public ImageGLBase
{
public:
  Window window;
  int stream_number;

  int width, height;

  /// @param width The width of the canvas
  /// @param width The height of the canvas
  /// @param stream_number A number identifying a certainstream. Defaults to 0.
  Canvas(int width, int height, int stream_number = 0) : ImageGLBase(width, height, width, height, true, stream_number, ImageGLBase::PixelFormat::BGRA_PIXEL_FORMAT),
                                                         stream_number(stream_number),
                                                         window(Window::open(width, height, WindowMode::HIDDEN)),
                                                         width(width),
                                                         height(height)
  {
  }

  /// Render - Inherited via ImageGLBase. Simply calls
  /// @see refresh()
  virtual void render(int field_num, bool refresh) override
  {
    this->refresh();
  }

  /// Makes the context of the specified window current for the calling thread.
  /// @see make_context_current()
  virtual void SetCurrent() override
  {
    /// Init gl
    this->window.make_context_current();
  }

  /// Inherited via ImageGLBase
  /// @see poll_events - Processes received events. The window and input callbacks associated with those events will be called.
  /// @see swap_buffers - Swaps the front and back buffers of the specified window. The window buffers will be swapped.
  virtual void refresh() override
  {
    this->window.poll_events();
    this->window.swap_buffers();
  }

  /// Inherited via ImageGLBase
  virtual bool SwapBuffers() override
  {
    this->window.swap_buffers();
    return true;
  }
};

/// An example of a videodecoder to be used by the Stereo library to display the video.
///
/// Inherits from VideoDecoder. Creates a GpuVideoDecoder, loads the video from given PATH, creates a Canvas and starts the video.
class VideoStream : public VideoDecoder
{
  GpuVideoDecoder *decoder;
  Canvas *canvas;
  Playback playback;
  std::chrono::time_point<std::chrono::high_resolution_clock> previous_frame_time;

    int frame_number;

public:
    /// @param video_path pointer to path of video file to be decoded.
    VideoStream(const char* video_path) {
        this->previous_frame_time = std::chrono::high_resolution_clock::now();

        this->playback = Playback::PLAY;
        this->decoder = GpuVideoDecoder::Create();
        int stream_number = 0;

        // Temporarily allocate a new string, because for <reasons...> `LoadVideo` only accepts `char*` and not `const char*`
        auto tmp_path = std::string(video_path);
        this->decoder->LoadVideo(&tmp_path[0]);

        int width = decoder->GetVideoWidth();
        int height = decoder->GetVideoHeight();
        this->canvas = new Canvas(width, height);

        this->canvas->ProcessBeforePaint(EOperation::texture | EOperation::pyramid | EOperation::gpuimage);
        decoder->Start(this->canvas);

        this->frame_number = 0;
    }
    
    /// Gets a new frame from the video and renders it if enough time has passed since the last frame was rendered.
    ///
    /// Inherited from VideoDecoder. Rendering depends on seconds_per_frame, which is specified by @see frame_rate()
    /// @returns the next frame
    virtual Frame next_frame() override
    {
        double frames_per_second = this->frame_rate();
        double seconds_per_frame = frames_per_second == 0 ? 0.0 : 1.0 / this->frame_rate();
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed_nanos = (now - this->previous_frame_time).count();
        auto elapsed_seconds = 1e-9 * (double)elapsed_nanos;

        bool bFramesDecoded = false;

        bool should_render = (this->playback == Playback::PLAY && elapsed_seconds > seconds_per_frame)
            || this->playback == Playback::FFW;
        if (should_render) {
            this->previous_frame_time = now;
            decoder->renderVideoFrame(this->canvas, true, true, bFramesDecoded);
        }

        Frame frame;
        if (bFramesDecoded) {
            spacetime::GpuStreamPtr pStream = spacetime::GpuProcessor::Get(GpuContext::Get())->GetStream(0);
            if (pStream) {
                frame.texture = pStream->GetGLTexture(ORIGINAL);
                frame.width = this->decoder->GetVideoWidth();
                frame.height = this->decoder->GetVideoHeight();
                frame.number = this->frame_number;
                this->frame_number++;
            }
        }

        return frame;
    }

    /// Sets the playback to one of the given playback options.
    /// @param playback The playback to switch to. E.g. PLAY, PAUSE, FFW
    virtual void set_playback(Playback playback) override
    {
        this->playback = playback;
    }

    virtual double frame_rate() override
    {
        // FIXME: despite what the documentation says `GetFrameRate` actually returns seconds per frame.
        auto seconds_per_frame = this->decoder->GetFrameRate();
        return seconds_per_frame == 0 ? 0 : 1.0 / seconds_per_frame;
    }
};

/// Provides an example of how to use the DLL.
int main(int argc, const char *argv[])
{
  auto arguments = parse_cli_arguments(argc, argv);

  try
  {
    printf("Stereo Example Executable (SEE)\n\n");

    auto video_stream = new VideoStream(arguments.video_path);
    Stereo::display_video(video_stream);

    printf("Bye\n");
  }
  catch (std::exception &e)
  {
    printf("Error: %s\n", e.what());
  }
  catch (const char *s)
  {
    printf("Program error: %s\n", s);
  }
  catch (...)
  {
    printf("There was an error.\n");
  }
}
