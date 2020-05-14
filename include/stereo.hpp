#pragma once

#include "shader.hpp"
#include "mesh.hpp"
#include "framebuffer.hpp"
#include "videodecoder.hpp"

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

/// A rectangle for selecting a subsection of the input video.
struct ScissorRectangle {
  float left, right, top, bottom;
};

/// The position and size of a viewport (an eye).
struct Viewport {
    /// The texture (a frame of video) to display.
    GLuint texture;
    /// Aspect ratio of the texture. (width / height)
    float aspect;
    /// The subsection of the frame to display.
    ScissorRectangle rectangle;
};

/// The position and size of the eyes/cameras.
struct StereoViewport {
  Viewport left;
  Viewport right;
};

struct UniformLocations {
    GLuint eye;
    GLuint proj;
    GLuint view;
};

/// Main entry point for the stereo display.
class Stereo {
  private:
    /// Create a new display.
    Stereo(int width, int height, VideoDecoder* decoder, vr::IVRSystem* openvr);

    /// Pointer to a video decoder provided by the user.
    VideoDecoder* decoder;

    /// OpenVR handle.
    vr::IVRSystem* openvr;

    /// Shader program to display the screen.
    Program program;

    /// Framebuffer for the left eye.
    Framebuffer left;
    /// Framebuffer for the right eye.
    Framebuffer right;

    /// Transformation matrix for the VR headset in the world.
    float hmd_transformation[16];

    /// GLSL shader binding points for all uniforms
    UniformLocations locations;

  public:
    /// Initialize the library and display a video to the user.
    static void display_video(VideoDecoder*);

  private:
    /// Displays a video frame for a specific eye.
    void render_scene(Viewport viewport, vr::Hmd_Eye eye);

    /// Update the position of the headeset within the world.
    void update_hmd_pose();

    /// Draw the stereo display.
    /// @param viewport How to render each eye.
    /// @returns Two framebuffers, one for each eye.
    StereoView draw(StereoViewport viewport);
};
