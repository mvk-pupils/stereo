#include <stereo.hpp>

/// Options extracted from command line arguments
struct CliArguments {
		const char* video_path = nullptr;

		VideoMode mode = VideoMode::SIDE_BY_SIDE;
};

/// Parse a sequence of command line arguments.
CliArguments parse_cli_arguments(int argc, const char* argv[]);
