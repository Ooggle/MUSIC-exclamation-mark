#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>

#if defined(_WIN32)
#define WINDOWS
// Used for simpler compatibility of unicode strings through OSs.
#define Ustring std::wstring
#else
#define POSIX
#define Ustring std::string
#endif

#endif
