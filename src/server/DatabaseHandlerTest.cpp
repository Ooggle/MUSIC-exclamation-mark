/* compile without sqlite3 : g++ -std=c++17 DatabaseHandlerTest.cpp WebHandler.cpp TcpServer.cpp -o DatabaseHandlerTest -lsqlite3 -lpthread -ldl `taglib-config --libs` `pkg-config --cflags --libs taglib` -lz -Bstatic -static-libstdc++ */

// c++ includes
#include <string>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

// c includes
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// taglib
#include <taglib/tag.h>
#include <taglib/fileref.h>

// web handler class
#include "WebHandler.h"

// database handler class
#include "DatabaseHandler.h"

// return a vector of strings of all mp3, flac and ogg files in directory and subdirectories.
std::vector<std::string> getFilesInDirectory(std::string path) {
    std::vector<std::string> filenames;

    for(const auto & entry : std::filesystem::directory_iterator(path)) {
        //std::cout << entry.path() << ", directory: " << entry.is_directory() << std::endl;
        if(entry.is_directory()) {
            std::vector<std::string> filenamesTemp = getFilesInDirectory(entry.path());
            filenames.insert(filenames.end(), filenamesTemp.begin(), filenamesTemp.end());
        } else {
            if((entry.path().extension() == ".mp3") ||
            (entry.path().extension() == ".ogg") ||
            (entry.path().extension() == ".flac")) {
                filenames.push_back(entry.path());
            }
        }
    }
    return filenames;
}

// Create the server databases.
int createDatabases(sqlite3 *db) {

    printf("Generating the Music! database... ");
    
    std::string sql;
    char *zErrMsg = NULL;
    int rc;

    /* CREATE TABLE musics */
    sql = "CREATE TABLE musics (\
            id              INTEGER PRIMARY KEY,\
            path            VARCHAR(5000),\
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

// Init the server databases.
int initDatabases(std::string path, sqlite3 *db) {
    
    std::vector<std::string> filenames = getFilesInDirectory(path);

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
    rc = sqlite3_prepare_v2(db, "INSERT INTO musics (path, album_id, genre, track_number, comment, title, artist, year) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
    } else {

        for(int i = 0; i < filenames.size(); i++){
            

            //get music infos
            TagLib::FileRef f(filenames[i].c_str());
            fTitle = f.tag()->title();
            fArtist = f.tag()->artist();
            fAlbum = f.tag()->album();
            fGenre = f.tag()->genre();
            fYear = f.tag()->year();
            fComment = f.tag()->comment();
            fTrack = f.tag()->track();

            sqlite3_bind_text(stmt, 1, filenames[i].c_str(), filenames[i].length(), NULL);
            sqlite3_bind_int(stmt, 2, 1); //TODO
            sqlite3_bind_text(stmt, 3, fGenre.toCString(), fGenre.length(), NULL);
            sqlite3_bind_int(stmt, 4, fTrack);
            sqlite3_bind_text(stmt, 5, fComment.toCString(), fComment.length(), NULL);
            sqlite3_bind_text(stmt, 6, fTitle.toCString(true), fTitle.size(), NULL);
            sqlite3_bind_text(stmt, 7, fArtist.toCString(true), fArtist.size(), NULL);
            sqlite3_bind_int(stmt, 8, fYear);
            

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                printf("execution failed: %s\n", sqlite3_errmsg(db));
            }
            
            
            sqlite3_reset(stmt);
        }
    }


    return 0;
}

int checkDatabaseIntegrity(sqlite3 *db) {
    return 0;
}



int main() {

    /* WebHandler handler("localhost", 81);
    handler.handlerLoop(); */

    DatabaseHandler *dbHandler = new DatabaseHandler("test.db");

    if(dbHandler->getIsDatabaseInitialised() == false) {
        return 1;
    }

    dbHandler->createDatabases();
    dbHandler->initDatabases("tests/musics/");



    /* char select[] = "SELECT * FROM COMPANY";
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db, select, -1, &stmt, NULL) != SQLITE_OK) {
        printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        sqlite3_finalize(stmt);
    }

    bool found = false;

    // execute sql statement, and while there are rows returned, print ID
    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        printf("TEST: ID = %d\n", sqlite3_column_int(stmt, 0));
        printf("TEST: name = %s\n", name);
        printf("TEST: AGE = %d\n", sqlite3_column_int(stmt, 2));
        found = true;
    }
    if(ret_code != SQLITE_DONE) {
        //this error handling could be done better, but it works
        printf("ERROR: while performing sql: %s\n", sqlite3_errmsg(db));
        printf("ret_code = %d\n", ret_code);
    }

    printf("entry %s\n", found ? "found" : "not found");
    sqlite3_finalize(stmt); */

    // file loading
    /*std::ifstream myFile;
    myFile.open("album_cover.jpg", std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    if(!myFile.is_open())
        return EXIT_FAILURE;

    std::filesystem::path p{"album_cover.jpg"};

    char file[std::filesystem::file_size(p)];

    if(!myFile.read(file, std::filesystem::file_size(p)))
        return EXIT_FAILURE;



    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "INSERT INTO users_musics (user_id, music_id) VALUES (?, ?)", -1, &stmt, NULL);
    
    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
    } else {

        for (int i = 1; i < 5; i++)
        {
            rc = sqlite3_bind_int(stmt, 1, 55 + i);

            if(rc != SQLITE_OK) {
                printf("bind failed: %s\n", sqlite3_errmsg(db));
            } else {
                
                //rc = sqlite3_bind_blob(stmt, 1, file, std::filesystem::file_size(p), SQLITE_STATIC);
                rc = sqlite3_bind_int(stmt, 2, 42 + i);

                if(rc != SQLITE_OK) {
                    printf("bind failed: %s\n", sqlite3_errmsg(db));
                } else {
                    rc = sqlite3_step(stmt);
                    if (rc != SQLITE_DONE) {
                        printf("execution failed: %s\n", sqlite3_errmsg(db));
                    }
                }

            }
            sqlite3_reset(stmt);
        }
        

        
    }
    sqlite3_finalize(stmt);

    sqlite3_close(db);*/

    return EXIT_SUCCESS;
}
