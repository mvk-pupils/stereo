//! Defines some macros that can be used for logging information.

// See: https://stackoverflow.com/questions/5641427/how-to-make-preprocessor-generate-a-string-for-line-keyword
#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)


/// Print some debug information: anything that could help with locating bugs.
#define DEBUG(...) fprintf(stderr, "[ " __FILE__ ":" S__LINE__ "\t DEBUG ]  " __VA_ARGS__), fputs("\n", stderr)

/// Print some general information: anything that could be of interest to the user.
#define INFO(...)  fprintf(stderr, "[ " __FILE__ ":" S__LINE__ "\t INFO  ]  " __VA_ARGS__), fputs("\n", stderr)

/// Print a warning: something has gone wrong, this is potentially a bug.
#define WARN(...)  fprintf(stderr, "[ " __FILE__ ":" S__LINE__ "\t WARN  ]  "__VA_ARGS__), fputs("\n", stderr)

/// Print an error: something has gone very wrong, this is potentially fatal to
///the program.
#define ERROR(...) fprintf(stderr, "[ " __FILE__ ":" S__LINE__ "\t ERROR ]  " __VA_ARGS__), fputs("\n", stderr)

