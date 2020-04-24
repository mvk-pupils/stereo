#include <stdio.h>
#include <algorithm>
#include <exception>

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
        window(Window::open(width, height)),
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

public:

    VideoStream(char* video_path) {
        this->decoder = GpuVideoDecoder::Create();
        int stream_number = 0;
        this->decoder->LoadVideo(video_path);

        auto width = decoder->GetVideoWidth();
        auto height = decoder->GetVideoHeight();
        this->canvas = new Canvas(width, height);

        this->canvas->ProcessBeforePaint(EOperation::texture | EOperation::pyramid | EOperation::gpuimage);
        decoder->Start(this->canvas);
    }
    
    virtual Frame next_frame() override
    {
        bool bFramesDecoded;
        decoder->renderVideoFrame(this->canvas, true, true, bFramesDecoded);

        Frame frame;

        if (bFramesDecoded) {
            spacetime::GpuStreamPtr pStream = spacetime::GpuProcessor::Get(GpuContext::Get())->GetStream(0);
            if (pStream) {
                frame.texture = pStream->GetGLTexture(ORIGINAL);
            }
        }

        return frame;
    }

    virtual void set_playback(Playback) override
    {
        // TODO
    }

    virtual int total_frames() override
    {
        // TODO
        return 0;
    }

    virtual double frame_rate() override
    {
        // TODO 
        return this->decoder->GetFrameRate();
    }
};


int main(int argc, const char* argv[]) {
  auto arguments = parse_cli_arguments(argc, argv);

  try {
    printf("Stereo Example Executable (SEE)\n\n");

    auto video_stream = new VideoStream("img/roller_coaster.mp4");
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

