
/// A kind of display
enum DisplayKind {
  /// Display to the monitor.
  MONITOR,
  /// Display to the VR headset.
  VR,
};

/// Options extracted from command line arguments
struct CliArguments {
  /// Which display to use for output.
  DisplayKind display = VR;
};

/// Parse a sequence of command line arguments.
CliArguments parse_cli_arguments(int argc, const char* argv[]);
