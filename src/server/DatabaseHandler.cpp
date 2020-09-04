#include "DatabaseHandler.h"

DatabaseHandler::DatabaseHandler(std::string DatabasePath) {
    char *zErrMsg = NULL;

    if(sqlite3_open(DatabasePath.c_str(), &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        isDatabaseInitialised = false;
    } else {
        fprintf(stderr, "Opened database successfully\n");
        isDatabaseInitialised = true;
    }
}

DatabaseHandler::~DatabaseHandler() {
    sqlite3_close(db);
}

std::vector<std::pair<std::string, std::string>> DatabaseHandler::getFilesInDirectory(std::string path) {
    std::vector<std::pair<std::string, std::string>> filenames;

    for(const auto & entry : std::filesystem::directory_iterator(path)) {
        //std::cout << entry.path() << ", directory: " << entry.is_directory() << std::endl;
        if(entry.is_directory()) {
            std::vector<std::pair<std::string, std::string>> filenamesTemp = getFilesInDirectory(entry.path());
            filenames.insert(filenames.end(), filenamesTemp.begin(), filenamesTemp.end());
        } else {
            if((entry.path().extension() == ".mp3") ||
            (entry.path().extension() == ".ogg") ||
            (entry.path().extension() == ".flac")) {
                filenames.push_back(std::pair(entry.path(), entry.path().extension()));
            }
        }
    }
    return filenames;
}

int DatabaseHandler::createDatabases() {
    printf("Generating the Music! database...\n");
    
    std::string sql;
    char *zErrMsg = NULL;
    int rc;

    /* CREATE TABLE musics */
    sql = "CREATE TABLE musics (\
            id              INTEGER PRIMARY KEY,\
            path            VARCHAR(5000),\
            extension       VARCHAR(10),\
            album_id        INTEGER,\
            genre           VARCHAR(100),\
            track_number    INTEGER,\
            comment         TEXT(5000),\
            title           VARCHAR(50),\
            artist          VARCHAR(500),\
            year            INTEGER\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        printf("Table musics created successfully\n");
    }



    /* CREATE TABLE users */
    sql = "CREATE TABLE users (\
            id              INTEGER PRIMARY KEY,\
            username        VARCHAR(250),\
            password_crypt  VARCHAR(500),\
            musics_paths    TEXT(10000),\
            creation_date   VARCHAR(100),\
            date            DATE\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        printf("Table users created successfully\n");
    }


    /* CREATE TABLE users_musics */
    sql = "CREATE TABLE users_musics (\
            id              INTEGER PRIMARY KEY,\
            user_id         INTEGER,\
            music_id        INTEGER\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        printf("Table users_musics created successfully\n");
    }


    /* CREATE TABLE albums */
    sql = "CREATE TABLE albums (\
            id              INTEGER PRIMARY KEY,\
            user_id         INTEGER,\
            music_id        INTEGER,\
            cover_image     BLOB\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        printf("Table albums created successfully\n");
    }

    printf("Done.\n");

    return 0;
}

int DatabaseHandler::initDatabases(std::string path) {
    std::vector<std::pair<std::string, std::string>> filenames = getFilesInDirectory(path);

    // print vector of strings
    /* printf("\n      -- ALL FILES --\n");
    for(int i = 0; i < filenames.size(); i++){ 
        printf("%s\n", filenames[i].c_str());
    } */

    TagLib::String fTitle;
    TagLib::String fArtist;
    TagLib::String fAlbum;
    TagLib::String fGenre;
    TagLib::String fComment;
    unsigned int fTrack;
    unsigned int fYear;

    int rc;

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "INSERT INTO musics (path, extension, album_id, genre, track_number, comment, title, artist, year) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
    } else {

        for(int i = 0; i < filenames.size(); i++){
            

            //get music infos
            TagLib::FileRef f(filenames[i].first.c_str());
            fTitle = f.tag()->title();
            fArtist = f.tag()->artist();
            fAlbum = f.tag()->album();
            fGenre = f.tag()->genre();
            fYear = f.tag()->year();
            fComment = f.tag()->comment();
            fTrack = f.tag()->track();


            // get true length of char arrays
            char genre[200];
            strcpy(genre, fGenre.toCString(true));
            int genreLength = sprintf(genre, "%s", genre);

            char comment[200];
            strcpy(comment, fComment.toCString(true));
            int commentLength = sprintf(comment, "%s", comment);

            char title[200];
            strcpy(title, fTitle.toCString(true));
            int titleLength = sprintf(title, "%s", title);

            char artist[200];
            strcpy(artist, fArtist.toCString(true));
            int artistLength = sprintf(artist, "%s", artist);

            sqlite3_bind_text(stmt, 1, filenames[i].first.c_str(), filenames[i].first.length(), NULL);
            sqlite3_bind_text(stmt, 2, filenames[i].second.c_str(), filenames[i].second.length(), NULL);
            sqlite3_bind_int(stmt, 3, 1); //TODO
            sqlite3_bind_text(stmt, 4, genre, genreLength, NULL);
            sqlite3_bind_int(stmt, 5, fTrack);
            sqlite3_bind_text(stmt, 6, comment, commentLength, NULL);
            sqlite3_bind_text(stmt, 7, title, titleLength, NULL);
            sqlite3_bind_text(stmt, 8, artist, artistLength, NULL);
            sqlite3_bind_int(stmt, 9, fYear);
            

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                printf("execution failed: %s\n", sqlite3_errmsg(db));
            }
            
            
            sqlite3_reset(stmt);
        }
    }


    return 0;
}

int DatabaseHandler::checkDatabaseIntegrity() {
    // TODO
    return 0;
}


bool DatabaseHandler::getIsDatabaseInitialised() {
    return isDatabaseInitialised;
}

sqlite3 *DatabaseHandler::getDB() {
    return db;
}
