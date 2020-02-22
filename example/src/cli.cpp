#include "cli.hpp"

#include <stdio.h>
#include <algorithm>
#include <string>

const char* USAGE = R"(
USAGE: 
    stereo-world <OPTIONS>

OPTIONS:
    -h                            print help information.
    -d, --display [monitor|vr]    set the display target (default = monitor)
)";


/// Print the usage of the program
void print_usage() {

  puts(USAGE);
}


/// Parse the CLI arguments into a struct. Will cause the program to exit
/// (intentionally) on some inputs.
CliArguments parse_cli_arguments(int argc, const char* argv[]) {
  CliArguments options;

  for (int i = 1; i < argc; i++) {
    auto arg = std::string(argv[i]);

    if (arg == "-d" || arg == "--display") {
      const char* invalid_arg = "expected one of 'monitor' or 'vr', found '%s'\n";
      i++;
      if (i >= argc) {
        printf(invalid_arg, "");
        print_usage();
        exit(1);
      }

      auto param = std::string(argv[i]);

      if (param == "monitor") options.display = MONITOR;
      else if (param == "vr") options.display = VR;
      else { 
        printf(invalid_arg, param.c_str());
        print_usage(); 
        exit(1);
      }
    } else if (arg == "-h" || arg == "--help") {
      print_usage();
      exit(0);
    } else {
      printf("unrecognized option: %s\n", arg.c_str());
      print_usage();
      exit(1);
    }
  }

  return options;
}


