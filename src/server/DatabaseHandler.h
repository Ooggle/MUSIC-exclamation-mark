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
        int addMusicsforUser(std::string path, std::string username);
        int addMusicforUser(std::vector<std::string> file, std::string username);
        bool isDatabaseValid();

        bool getIsDatabaseInitialised();
        int isMusicExistForUser(std::string username, std::string genre, int track_number, std::string title, std::string artist, int year);
        int isArtistExistForUser(std::string username, std::string artist);
        int isAlbumExistForUser(std::string username, std::string album_name, int album_year);
        int getUserID(std::string username);
        sqlite3 *getDB();

        bool createUser(std::string username, std::string password, std::string *errMsg);
        bool getUserInfos(std::string username, int *id, std::vector<std::string> *directories, std::string *errMsg);
};

#endif
