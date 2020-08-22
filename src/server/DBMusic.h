#include <string>
#include "json.hpp"
#include <filesystem>
#include "JsonDatabase.h"

#ifndef DBMUSIC_H
#define DBMUSIC_H

using json = nlohmann::json;
namespace fs = std::filesystem;

class DBMusic : public JsonDatabase {
    private:

    public:
        std::vector<std::string> getMusicsInDirectory(std::string path);
        void createJsonDatabase(std::string path);

        int get_musicCount();
};

#endif
