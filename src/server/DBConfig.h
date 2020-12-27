#include <string>
#include "json.hpp"
#include <filesystem>
#include "JsonDatabase.h"

#ifndef DBCONFIG_H
#define DBCONFIG_H

using json = nlohmann::json;
namespace fs = std::filesystem;

class DBConfig : public JsonDatabase {
    private:

    public:
        DBConfig();
        ~DBConfig();


};

#endif
