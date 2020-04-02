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

#include <openvr.h>

/// Initialize OpenVR (kinda broken on MacOS and Linux right now)
vr::IVRSystem* init_openvr() {
    vr::HmdError error;
    auto openvr = vr::VR_Init(&error, vr::VRApplication_Scene);

    if (error) {
      auto message = VR_GetVRInitErrorAsEnglishDescription(error);
      printf("ERROR: %s\n", message);
    }

    return openvr;
}

StereoViewport get_stereo_viewport(int width, int height) {
  StereoViewport viewport;

  auto left = &viewport.left;
  left->width = width / 2;
  left->height = height;

  auto right = &viewport.right;
  right->width = width / 2;
  right->height = height;

  return viewport;
}

class Canvas : public ImageGLBase {
public:
    Window window;
    int stream_number;
    vr::IVRSystem* openvr;
    Stereo* stereo;

    int width, height;

    Canvas(int width, int height, int stream_number = 0): 
        ImageGLBase(width, height, width, height, true, stream_number, ImageGLBase::PixelFormat::BGRA_PIXEL_FORMAT),
        stream_number(stream_number),
        window(Window::open(width, height)),
        openvr(init_openvr()),
        stereo(nullptr),
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
        if (!this->stereo) {
            this->stereo = new Stereo(Stereo::init(this->width, this->height));
        }
    }

    virtual void refresh() override
    {
        // Draw stuff
        spacetime::GpuStreamPtr pStream = spacetime::GpuProcessor::Get(GpuContext::Get())->GetStream(0);
        if (!pStream)
            return;
        GLuint gl = pStream->GetGLTexture(ORIGINAL);
        if (gl) {
            Viewport base;
            base.texture = gl;
            base.width = this->width / 2;
            base.height = this->height;
            base.rectangle.left = 0.0f;
            base.rectangle.right = 1.0f;
            base.rectangle.top = 0.0f;
            base.rectangle.bottom = 1.0f;

            StereoViewport viewport;
            viewport.left = base;
            viewport.left.rectangle.right = 0.5f;
            viewport.right = base;
            viewport.right.rectangle.left = 0.5f;

            StereoView view = this->stereo->draw(viewport);

            if (this->openvr) {
                // Taken from the OpenVR OpenGL example:
                vr::Texture_t left = { (void*)(uintptr_t)view.left.texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
                vr::Texture_t right = { (void*)(uintptr_t)view.right.texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

                vr::VRCompositor()->WaitGetPoses(nullptr, 0, nullptr, 0);

                vr::VRCompositor()->Submit(vr::Eye_Left, &left);
                vr::VRCompositor()->Submit(vr::Eye_Right, &right);
            }

            this->window.swap_buffers();
        }
    }

    virtual bool SwapBuffers() override
    {
        this->window.swap_buffers();
        return false;
    }
};

int main(int argc, const char* argv[]) {
  auto arguments = parse_cli_arguments(argc, argv);

  try {
    printf("Stereo Example Executable (SEE)\n\n");
    
    auto decoder = GpuVideoDecoder::Create();
    int stream_number = 0;
    auto canvas = new Canvas(1280, 720, stream_number);
    canvas->ProcessBeforePaint(EOperation::texture | EOperation::pyramid | EOperation::gpuimage);

    decoder->LoadVideo("img/jilin-side-by-side-stereo1.mp4");
    decoder->Start(canvas);

    while (canvas->window.is_open()) {
        bool bFramesDecoded;
        decoder->renderVideoFrame(canvas, true, true, bFramesDecoded);

        canvas->window.poll_events();

        if (canvas->window.is_key_down(GLFW_KEY_ESCAPE)) {
            canvas->window.close();
            break;
        }
    }

    printf("Bye\n");
  } catch (std::exception& e) {
    printf("Error: %s\n", e.what());
  } catch (const char* s) {
    printf("Program error: %s\n", s);
  } catch (...) {
    printf("There was an error.\n");
  }
}

