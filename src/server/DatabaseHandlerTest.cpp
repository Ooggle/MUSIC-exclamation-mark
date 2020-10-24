/* compile : g++ -std=c++17 DatabaseHandlerTest.cpp DatabaseHandler.cpp WebHandler.cpp TcpServer.cpp -o DatabaseHandlerTest -lsqlite3 -lpthread -ldl `taglib-config --libs` `pkg-config --cflags --libs taglib` -lz -lstdc++fs -Bstatic -static-libstdc++ */

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


int main() {

    DatabaseHandler *dbHandler = new DatabaseHandler("music.db");

    if(dbHandler->getIsDatabaseInitialised() == false) {
        return 1;
    }

    if(!dbHandler->isDatabaseValid()) {
        dbHandler->createTables();

        std::string *errMsg;
        if(dbHandler->createUser("Ooggle", "password", errMsg) == false) {
            printf("cannot create user: %s\n", errMsg->c_str());
        }
        dbHandler->addMusicsforUser("tests/musics/", "Ooggle");
    }

    /* sqlite3 *db;
    db = dbHandler->getDB();

    WebHandler handler("localhost", 81, db);
    handler.handlerLoop(); */










    // OLD THINGS

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
