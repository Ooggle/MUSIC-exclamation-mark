#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <string>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

// taglib
#include <taglib/tag.h>
#include <taglib/fileref.h>

class DatabaseHandler {
    private:
        sqlite3 *db;
        bool isDatabaseInitialised;
        
    public:
        DatabaseHandler(std::string DatabasePath);
        ~DatabaseHandler();

        std::vector<std::string> getFilesInDirectory(std::string path);
        int createDatabases();
        int initDatabases(std::string path);
        int checkDatabaseIntegrity();

        bool getIsDatabaseInitialised();
};

#endif
