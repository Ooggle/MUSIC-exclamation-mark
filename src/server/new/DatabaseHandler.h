#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "platform.h"

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "localFilesHandler.h"
#include "TagsHandler.h"

class DatabaseHandler {
    private:
        sqlite3 *db;
        bool databaseInitialised;
        int dbVersion;
        
    public:
        DatabaseHandler(std::string DatabasePath);
        ~DatabaseHandler();

        // Create the server databases.
        int createTables();
        // Init the server databases.
        int addMusicsforUser(std::string path, int user_id);
        int addMusicforUser(std::vector<std::string> file, int user_id);
        bool isDatabaseValid();

        bool isDatabaseInitialised();
        int isMusicExistForUser(int user_id, std::string genre, int track_number, std::string title, std::string artist, int year);
        int isArtistExistForUser(int user_id, std::string artist);
        int isAlbumExistForUser(int user_id, std::string album_name, std::string album_artist, int album_year);
        int isUserIdExists(int user_id);
        int getUserID(std::string username);
        // sqlite3 *getDB();

        bool createUser(std::string username, std::string password, std::string *errMsg);
        bool getUserInfos(std::string username, int *id, std::vector<std::string> *directories, std::string *errMsg);
};

#endif
