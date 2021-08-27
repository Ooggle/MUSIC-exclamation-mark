#ifndef LOCALFILESHANDLER_H
#define LOCALFILESHANDLER_H

#include "platform.h"

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

#if defined(WINDOWS)
#include <locale>
#include <codecvt>
#include <fcntl.h>
#endif


class LocalFilesHandler
{
    public:
        std::vector<std::vector<std::string>> getFilesInDirectory(std::string path);
};

#endif
