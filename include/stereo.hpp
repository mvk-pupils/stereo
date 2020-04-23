#pragma once

#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"

#include <openvr.h>

/// A rasterized view of the scene.
struct View {
  /// The ID of a GL texture that contains the color data of the rendered view.
  GLuint texture;
};

/// A rasterized view of the scene, from the perspective of two eyes.
struct StereoView {
  View left;
  View right;
};

struct ScissorRectangle {
    float left, right, top, bottom;
};

/// The position and size of a viewport (an eye).
struct Viewport {
  /// The width and height of the resulting rendered view.
  GLuint width, height;
  /// The texture (a frame of video) to display.
  GLuint texture;
  /// The subsection of the frame to display.
  ScissorRectangle rectangle;
};

/// The position and size of the eyes/cameras.
struct StereoViewport {
  Viewport left;
  Viewport right;
};

/// Main entry point for the stereo display.
class Stereo {
  private:
    /// Create a new display.
    Stereo(int width, int height);

    VideoDecoder decoder;

    /// OpenVR handle
    vr::IVRSystem* openvr;

    /// Shader program to display the screen.
    Program program;

    /// Framebuffer for the left eye.
    Framebuffer left;
    /// Framebuffer for the right eye.
    Framebuffer right;

  public:
    /// Initialize the library and construct a new handle.
    /// @returns A handle to the library.
    static void display_video(VideoDecoder*);

  private:
    void render_scene(Viewport viewport, vr::Hmd_Eye eye);

    /// Draw the stereo display.
    /// @param viewport How to render each eye.
    /// @returns Two framebuffers, one for each eye.
    StereoView draw(StereoViewport viewport);
};
