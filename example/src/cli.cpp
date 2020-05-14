#include "cli.hpp"

#include <stdio.h>
#include <algorithm>
#include <string>

const char* USAGE = R"(
USAGE: 
    stereo-world [OPTIONS] <video>

ARGUMENTS:
    <video>                     path to the video to display.

OPTIONS:
    -h                          print this help information.
    -m, --mode <video_mode>     sets the mode in which do display the video,
                                one of `side-by-side` or `half-side-by-side`
)";

#define ERROR(...) printf("ERROR: "), printf(__VA_ARGS__), puts(USAGE), exit(1);

/// Parse the CLI arguments into a struct. Will cause the program to exit
/// (intentionally) on some inputs.
CliArguments parse_cli_arguments(int argc, const char* argv[]) {
  int i = 1;
  auto next_argument = [argc, argv, &i](std::string& out) -> bool {
      if (i < argc) {
          out = argv[i];
          i++;
          return true;
      }
      else {
          return false;
      }
  };

  CliArguments options;

  std::string arg;
  while (next_argument(arg)) {
    if (!arg.empty() && arg[0] == '-') {
        if (arg == "-h" || arg == "--help") {
            puts(USAGE);
            exit(0);
        }
        else if (arg == "-m" || arg == "--mode") {
            if (!next_argument(arg)) ERROR("expected <video_mode>");

            if (arg == "side-by-side" || arg == "sbs")
                options.mode = VideoMode::SIDE_BY_SIDE;
            else if (arg == "half-side-by-side" || arg == "hsbs")
                options.mode = VideoMode::HALF_SIDE_BY_SIDE;
            else
                ERROR("unknonw video mode: %s", arg.c_str());
        } 
        else {
            ERROR("unknown option: %s\n", arg.c_str());
        }
    } else if (options.video_path == nullptr) {
      options.video_path = argv[i-1];
    } else {
      ERROR("unexpected argument: %s\n", arg.c_str());
    }
  }

  if (options.video_path == nullptr) {
      ERROR("missing argument <video>");
  }

  return options;
}


