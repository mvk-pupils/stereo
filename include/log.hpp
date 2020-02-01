//! Defines some macros that can be used for logging information.

#define DEBUG(text, ...) fprintf(stderr, "[Stereo::Debug] " text "\n", ##__VA_ARGS__)
#define INFO(text, ...)  fprintf(stderr, "[Stereo::Info] " text "\n", ##__VA_ARGS__)
#define WARN(text, ...)  fprintf(stderr, "[Stereo::Warning] " text "\n", ##__VA_ARGS__)
#define ERROR(text, ...) fprintf(stderr, "[Stereo::Error] " text "\n", ##__VA_ARGS__)

