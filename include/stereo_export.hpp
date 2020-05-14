#pragma once

/// Depending on if we are building or using the DLL we export/import symbols, respectively.
#ifdef MAKEDLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
