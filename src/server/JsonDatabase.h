#include <string>
#include "json.hpp"
#include <filesystem>
#include <taglib/tag.h>
#include <taglib/fileref.h>

#ifndef JSONDATABASE_H
#define JSONDATABASE_H

using json = nlohmann::json;
namespace fs = std::filesystem;

class JsonDatabase {
    private:
        FILE *pDatabase;
        std::string databaseName;
        json databaseContent;

        enum DB_STATE {
            NON_INITIALIZED,
            CORRUPTED,
            EMPTY,
            INITIALIZED
        };

    public:
        int dbState;

        JsonDatabase(std::string databaseName);
        ~JsonDatabase();

        
        void initDatabase();
        void makeDatabaseBakup();

        int get_databaseContent();
        int get_dbState();
};

#endif
