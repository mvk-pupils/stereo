#include "cli.hpp"

#include <stdio.h>
#include <algorithm>
#include <string>

const char* USAGE = R"(
USAGE: 
    stereo-world [OPTIONS] <video>

ARGUMENTS:
    <video>         path to the video to display.

OPTIONS:
    -h              print this help information.
)";

#define ERROR(...) printf("ERROR: "), printf(__VA_ARGS__), puts(USAGE), exit(1);

/// Parse the CLI arguments into a struct. Will cause the program to exit
/// (intentionally) on some inputs.
CliArguments parse_cli_arguments(int argc, const char* argv[]) {
  CliArguments options;

  for (int i = 1; i < argc; i++) {
    auto arg = std::string(argv[i]);

    if (!arg.empty() && arg[0] == '-') {
        if (arg == "-h" || arg == "--help") {
            puts(USAGE);
            exit(0);
        } else {
            ERROR("unknown option: %s\n", argv[i]);
        }
    } else if (options.video_path == nullptr) {
      options.video_path = argv[i];
    } else {
      ERROR("unexpected argument: %s\n", argv[i]);
    }
  }

  if (options.video_path == nullptr) {
      ERROR("missing argument <video>");
  }

  return options;
}


