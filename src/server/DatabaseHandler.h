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

        std::vector<std::vector<std::string>> getFilesInDirectory(std::string path);
        // Create the server databases.
        int createTables();
        // Init the server databases.
        int initDatabases(std::string path);
        bool isDatabaseValid();

        bool getIsDatabaseInitialised();
        sqlite3 *getDB();

        char* serialize(std::vector<std::string> &v, unsigned int *count);
        void deserialize(std::vector<std::string> &restore, const char* buffer, int total_count);

        bool createUser(std::string username, std::string password, std::string *errMsg);
        bool getUserInfos(std::string username, int *id, std::vector<std::string> *directories, std::string *errMsg);
        bool updateUserPaths(std::vector<std::string> musicPath, std::string errMsg);
};

#endif
