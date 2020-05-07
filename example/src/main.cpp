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

class Canvas : public ImageGLBase {
public:
    Window window;
    int stream_number;

    int width, height;

    Canvas(int width, int height, int stream_number = 0): 
        ImageGLBase(width, height, width, height, true, stream_number, ImageGLBase::PixelFormat::BGRA_PIXEL_FORMAT),
        stream_number(stream_number),
        window(Window::open(width, height, WindowMode::HIDDEN)),
        width(width),
        height(height)
    {
    }

    // Inherited via ImageGLBase
    virtual void render(int field_num, bool refresh) override
    {
        this->refresh();
    }

    virtual void SetCurrent() override
    {
        // Init gl
        this->window.make_context_current();
    }

    virtual void refresh() override
    {
        this->window.poll_events();
        this->window.swap_buffers();
    }

    virtual bool SwapBuffers() override
    {
        this->window.swap_buffers();
        return true;
    }
};


class VideoStream : public VideoDecoder {
    GpuVideoDecoder* decoder;
    Canvas* canvas;
    Playback playback;
    std::chrono::time_point<std::chrono::high_resolution_clock> previous_frame_time;

    int frame_number;

public:

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

    virtual void set_playback(Playback playback) override
    {
        this->playback = playback;
    }

    virtual int total_frames() override
    {
        // TODO
        return -1;
    }

    virtual double frame_rate() override
    {
        // FIXME: despite what the documentation says `GetFrameRate` actually returns seconds per frame.
        auto seconds_per_frame = this->decoder->GetFrameRate();
        return seconds_per_frame == 0 ? 0 : 1.0 / seconds_per_frame;
    }
};


int main(int argc, const char* argv[]) {
  auto arguments = parse_cli_arguments(argc, argv);

  try {
    printf("Stereo Example Executable (SEE)\n\n");

    auto video_stream = new VideoStream(arguments.video_path);
    Stereo::display_video(video_stream);

    printf("Bye\n");
  } catch (std::exception& e) {
    printf("Error: %s\n", e.what());
  } catch (const char* s) {
    printf("Program error: %s\n", s);
  } catch (...) {
    printf("There was an error.\n");
  }
}

