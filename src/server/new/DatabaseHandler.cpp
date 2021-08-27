#include "DatabaseHandler.h"

DatabaseHandler::DatabaseHandler(std::string DatabasePath)
{
    char *zErrMsg = NULL;

    if(sqlite3_open(DatabasePath.c_str(), &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        databaseInitialised = false;
    } else {
        fprintf(stderr, "Opened database successfully\n");
        databaseInitialised = true;
    }
}

DatabaseHandler::~DatabaseHandler()
{
    sqlite3_close(db);
}

int DatabaseHandler::createTables()
{
    printf("Generating the Music! database...\n");
    
    std::string sql;
    char *zErrMsg = NULL;
    int rc;

    /* CREATE TABLE database_informations */
    sql = "CREATE TABLE database_informations (\
            description         VARCHAR(5000),\
            creation_date        DATETIME\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //printf("Table database_description created successfully\n");
    }

    /* CREATE TABLE musics */
    sql = "CREATE TABLE musics (\
            id              INTEGER PRIMARY KEY,\
            filename        VARCHAR(5000),\
            path            VARCHAR(5000),\
            extension       VARCHAR(10),\
            user_id         INTEGER,\
            album_id        INTEGER,\
            genre           VARCHAR(100),\
            track_number    INTEGER,\
            track_total     INTEGER,\
            disc_number     INTEGER,\
            disc_total      INTEGER,\
            comment         TEXT(5000),\
            title           VARCHAR(50),\
            artist          VARCHAR(500),\
            music_year      INTEGER\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //printf("Table musics created successfully\n");
    }



    /* CREATE TABLE users */
    sql = "CREATE TABLE users (\
            id              INTEGER PRIMARY KEY,\
            username        VARCHAR(250) NOT NULL UNIQUE,\
            password_crypt  VARCHAR(500) NOT NULL,\
            library_revision INTEGER,\
            creation_date   DATETIME\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //printf("Table users created successfully\n");
    }


    /* CREATE TABLE artists */
    sql = "CREATE TABLE artists (\
            id              INTEGER PRIMARY KEY,\
            name            VARCHAR(500),\
            user_id         INTEGER,\
            artist_image    BLOB\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //printf("Table artists created successfully\n");
    }


    /* CREATE TABLE albums */
    sql = "CREATE TABLE albums (\
            id              INTEGER PRIMARY KEY,\
            name            VARCHAR(500),\
            user_id         INTEGER,\
            artist_id       INTEGER,\
            album_year      INTEGER,\
            cover_image     BLOB\
        )";

    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //printf("Table albums created successfully\n");
    }

    /* add row to database_informations table */
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_exec(db, "INSERT INTO database_informations(description, creation_date) VALUES('', DATETIME('now','localtime'))", NULL, 0, &zErrMsg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //printf("Row inserted successfully\n");
    }

    printf("Done.\n");

    return 0;
}

int DatabaseHandler::addMusicsforUser(std::string path, int user_id)
{
    LocalFilesHandler *localFiles = new LocalFilesHandler();
    std::vector<std::vector<std::string>> filenames = localFiles->getFilesInDirectory(path);

    // Verify user_id
    if(!isUserIdExists(user_id)) {
        printf("Cannot add music(s), user not found\n");
        return -1;
    }

    for(int i = 0; i < filenames.size(); i++)
        addMusicforUser(filenames[i], user_id);

    return 0;
}

/*
 * Consider using ffprobe:
 * ffprobe -v quiet -print_format json -loglevel fatal -show_error -show_format "path/to/file"
*/
int DatabaseHandler::addMusicforUser(std::vector<std::string> file, int user_id)
{
    // Verify user_id
    if(!isUserIdExists(user_id)) {
        printf("Cannot add music(s), user not found\n");
        return -1;
    }
    
    int rc;
    sqlite3_stmt *stmt = NULL;

    // get music infos
    std::string fTitle = "";
    std::string fArtist = "";
    std::string fAlbum = "";
    std::string fAlbumArtist = "";
    std::string fGenre = "";
    std::string fComment = "";
    int fTrack = 0;
    int fTrackTotal = 0;
    int fDisc = 0;
    int fDiscTotal = 0;
    int fYear = 0;

    std::vector<std::pair<std::string, std::string>> res;
    TagsHandler *tagsHandler = new TagsHandler();

    tagsHandler->getTags(file[0], res);
    for(int i = 0; i < res.size(); i++)
    {
        //printf("%s = %s\n", res[i].first.c_str(), res[i].second.c_str());
        if(strcmp(res[i].first.c_str(), "title") == 0) {
            fTitle = res[i].second.c_str();
        } else if(strcmp(res[i].first.c_str(), "artist") == 0) {
            fArtist = res[i].second.c_str();
        } else if(strcmp(res[i].first.c_str(), "album") == 0) {
            fAlbum = res[i].second.c_str();
        } else if(strcmp(res[i].first.c_str(), "album_artist") == 0) {
            fAlbumArtist = res[i].second.c_str();
        } else if(strcmp(res[i].first.c_str(), "genre") == 0) {
            fGenre = res[i].second.c_str();
        } else if(strcmp(res[i].first.c_str(), "comment") == 0) {
            fComment = res[i].second.c_str();
        } else if(strcmp(res[i].first.c_str(), "track") == 0) {
            fTrack = (int)strtol(res[i].second.c_str(), NULL, 10);
        } else if(strcmp(res[i].first.c_str(), "tracktotal") == 0) {
            fTrackTotal = (int)strtol(res[i].second.c_str(), NULL, 10);
        } else if(strcmp(res[i].first.c_str(), "disc") == 0) {
            fDisc = (int)strtol(res[i].second.c_str(), NULL, 10);
        } else if(strcmp(res[i].first.c_str(), "disctotal") == 0) {
            fDiscTotal = (int)strtol(res[i].second.c_str(), NULL, 10);
        } else if(strcmp(res[i].first.c_str(), "date") == 0) {
            fYear = (int)strtol(res[i].second.c_str(), NULL, 10);
        }
    }

    // verify if music, album or artist not already exist

    int artistID, albumArtistID, albumID;

    artistID = isArtistExistForUser(user_id, fArtist);
    if(!artistID) {
        stmt = NULL;
        rc = sqlite3_prepare_v2(db, "INSERT INTO artists(name, user_id) VALUES(?, ?)", -1, &stmt, NULL);

        if(rc != SQLITE_OK) {
            printf("prepare failed: %s\n", sqlite3_errmsg(db));
        } else {

            sqlite3_bind_text(stmt, 1, fArtist.c_str(), fArtist.length(), NULL); // artist name
            sqlite3_bind_int(stmt, 2, user_id); // user_id

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                printf("execution failed: %s\n", sqlite3_errmsg(db));
            } else {
                // get newly created artist
                artistID = isArtistExistForUser(user_id, fArtist);
            }
        }
    }

    if(fAlbumArtist != "")
    {
        albumArtistID = isArtistExistForUser(user_id, fAlbumArtist);
        if(!albumArtistID) {
            stmt = NULL;
            rc = sqlite3_prepare_v2(db, "INSERT INTO artists(name, user_id) VALUES(?, ?)", -1, &stmt, NULL);

            if(rc != SQLITE_OK) {
                printf("prepare failed: %s\n", sqlite3_errmsg(db));
            } else {

                sqlite3_bind_text(stmt, 1, fAlbumArtist.c_str(), fAlbumArtist.length(), NULL); // artist name
                sqlite3_bind_int(stmt, 2, user_id); // user_id

                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE) {
                    printf("execution failed: %s\n", sqlite3_errmsg(db));
                } else {
                    // get newly created artist
                    albumArtistID = isArtistExistForUser(user_id, fAlbumArtist);
                }
            }
        }
    }

    albumID = isAlbumExistForUser(user_id, fAlbum, fAlbumArtist, fYear);
    if(!albumID) {
        stmt = NULL;
        rc = sqlite3_prepare_v2(db, "INSERT INTO albums(name, user_id, artist_id, album_year, cover_image) VALUES(?, ?, ?, ?, ?)", -1, &stmt, NULL);

        if(rc != SQLITE_OK) {
            printf("prepare failed: %s\n", sqlite3_errmsg(db));
        } else {

            sqlite3_bind_text(stmt, 1, fAlbum.c_str(), fAlbum.length(), NULL); // album name
            sqlite3_bind_int(stmt, 2, user_id); // user_id
            if(fAlbumArtist != "")
            {
                sqlite3_bind_int(stmt, 3, albumArtistID); // artist_id
            } else {
                sqlite3_bind_int(stmt, 3, artistID); // artist_id
            }
            sqlite3_bind_int(stmt, 4, fYear); // year

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                printf("execution failed: %s\n", sqlite3_errmsg(db));
            } else {
                albumID = isAlbumExistForUser(user_id, fAlbum, fAlbumArtist, fYear);
            }
        }
    }

    if(!isMusicExistForUser(user_id, fGenre, fTrack, fTitle, fArtist, fYear)) {
        stmt = NULL;
        rc = sqlite3_prepare_v2(db, "INSERT INTO musics(filename, path, extension, user_id, album_id, genre, track_number, track_total, disc_number, disc_total, comment, title, artist, music_year) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, NULL);

        if(rc != SQLITE_OK) {
            printf("prepare failed: %s\n", sqlite3_errmsg(db));
        } else {

            sqlite3_bind_text(stmt, 1, file.at(1).c_str(), file.at(1).length(), NULL); // filename
            sqlite3_bind_text(stmt, 2, file.at(0).c_str(), file.at(0).length(), NULL); // path
            sqlite3_bind_text(stmt, 3, file.at(2).c_str(), file.at(2).length(), NULL); // extension
            sqlite3_bind_int(stmt, 4, user_id); // user_id
            sqlite3_bind_int(stmt, 5, albumID); // album ID
            sqlite3_bind_text(stmt, 6, fGenre.c_str(), fGenre.length(), NULL); // genre
            sqlite3_bind_int(stmt, 7, fTrack);
            sqlite3_bind_int(stmt, 8, fTrackTotal);
            sqlite3_bind_int(stmt, 9, fDisc);
            sqlite3_bind_int(stmt, 10, fDiscTotal);
            sqlite3_bind_text(stmt, 11, fComment.c_str(), fComment.length(), NULL);
            sqlite3_bind_text(stmt, 12, fTitle.c_str(), fTitle.length(), NULL);
            sqlite3_bind_text(stmt, 13, fArtist.c_str(), fArtist.length(), NULL);
            sqlite3_bind_int(stmt, 14, fYear);
            
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                printf("execution failed: %s\n", sqlite3_errmsg(db));
            }
        }
    }

    return 0;
}

bool DatabaseHandler::isDatabaseValid()
{
    char *zErrMsg = NULL;
    int returnInt = true;

    if(sqlite3_exec(db, "SELECT * FROM musics", NULL, 0, &zErrMsg) != SQLITE_OK) {
        returnInt = false;
    } else if(sqlite3_exec(db, "SELECT * FROM users", NULL, 0, &zErrMsg) != SQLITE_OK) {
        returnInt = false;
    } else if(sqlite3_exec(db, "SELECT * FROM albums", NULL, 0, &zErrMsg) != SQLITE_OK) {
        returnInt = false;
    } else if(sqlite3_exec(db, "SELECT * FROM artists", NULL, 0, &zErrMsg) != SQLITE_OK) {
        returnInt = false;
    } else if(sqlite3_exec(db, "SELECT * FROM database_informations", NULL, 0, &zErrMsg) != SQLITE_OK) {
        returnInt = false;
    }

    if(returnInt == false) {
        printf("Database integrity check failed: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return returnInt;
}


bool DatabaseHandler::isDatabaseInitialised()
{
    return databaseInitialised;
}

int DatabaseHandler::isMusicExistForUser(int user_id, std::string genre, int track_number, std::string title, std::string artist, int year)
{
    int rc, returnInt = 0;

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "SELECT * \
                            FROM musics \
                            WHERE user_id = ? AND genre = ? AND track_number = ? AND title = ? AND artist = ? AND music_year = ?", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
    } else {
        sqlite3_bind_int(stmt, 1, user_id); // user_id
        sqlite3_bind_text(stmt, 2, genre.c_str(), genre.length(), NULL); // genre
        sqlite3_bind_int(stmt, 3, track_number); // track_number
        sqlite3_bind_text(stmt, 4, title.c_str(), title.length(), NULL); // genre
        sqlite3_bind_text(stmt, 5, artist.c_str(), artist.length(), NULL); // artist
        sqlite3_bind_int(stmt, 6, year); // year

        rc = sqlite3_step(stmt);

        if(rc == SQLITE_ROW) {
            returnInt = sqlite3_column_int(stmt, 0);;
            //printf("Music found\n");
        } else {
            //printf("Music not found\n");
        }
    }

    return returnInt;
}

int DatabaseHandler::isArtistExistForUser(int user_id, std::string artist)
{
    int rc, returnInt = 0;

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "SELECT * \
                            FROM artists \
                            WHERE user_id = ? AND name = ?", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
    } else {
        sqlite3_bind_int(stmt, 1, user_id); // user_id
        sqlite3_bind_text(stmt, 2, artist.c_str(), artist.length(), NULL); // genre

        rc = sqlite3_step(stmt);

        if(rc == SQLITE_ROW) {
            returnInt = sqlite3_column_int(stmt, 0);;
            //printf("Artist found\n");
        } else {
            //printf("Artist not found\n");
        }
    }

    return returnInt;
}

int DatabaseHandler::isAlbumExistForUser(int user_id, std::string album_name, std::string album_artist, int album_year)
{
    int rc, returnInt = 0;

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "SELECT * \
                                FROM albums \
                                LEFT JOIN artists ON artists.id = albums.artist_id \
                                WHERE albums.user_id = ? AND albums.name = ? AND album_year = ?", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
    } else {
        sqlite3_bind_int(stmt, 1, user_id); // user_id
        sqlite3_bind_text(stmt, 2, album_name.c_str(), album_name.length(), NULL); // album name
        sqlite3_bind_int(stmt, 3, album_year); // album year

        rc = sqlite3_step(stmt);

        if(rc == SQLITE_ROW) {
            returnInt = sqlite3_column_int(stmt, 0);
            //printf("Album found\n");
        } else {
            //printf("Album not found\n");
        }
    }

    return returnInt;
}

int DatabaseHandler::isUserIdExists(int user_id)
{
    int rc;

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "SELECT * FROM users WHERE id = ?", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
        return false;
    } else {

        sqlite3_bind_int(stmt, 1, user_id);
        
        rc = sqlite3_step(stmt);

        if(rc != SQLITE_ROW) {
            printf("Error: %s\n", sqlite3_errmsg(db));
            return -1;   
        }
    }

    return true;
}

int DatabaseHandler::getUserID(std::string username)
{
    int rc, username_id;

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "SELECT * FROM users WHERE username = ?", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
        return false;
    } else {

        sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), NULL);
        
        rc = sqlite3_step(stmt);

        if(rc == SQLITE_ROW) {
            username_id = sqlite3_column_int(stmt, 0);
        } else {
            printf("Error: %s\n", sqlite3_errmsg(db));
            return -1;
        }
    }

    return username_id;
}

/* sqlite3 *DatabaseHandler::getDB()
{
    return db;
} */

bool DatabaseHandler::createUser(std::string username, std::string password, std::string *errMsg)
{

    int rc;
    int returnInt = true;

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "INSERT INTO users(username, password_crypt, library_revision, creation_date) VALUES(?, ?, ?, DATETIME('now','localtime'))", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
        *errMsg = sqlite3_errmsg(db);
        returnInt = false;
    } else {

        sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), NULL);
        sqlite3_bind_text(stmt, 2, password.c_str(), password.length(), NULL);

        /* unsigned int total_count = 0;
        char *paths = serialize(musicPath, &total_count);
        sqlite3_bind_text(stmt, 3, paths, total_count, NULL);

        sqlite3_bind_int(stmt, 4, total_count); */ // paths size
        sqlite3_bind_int(stmt, 3, 0); // library revision

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            printf("user creation failed: %s\n", sqlite3_errmsg(db));
            *errMsg = sqlite3_errmsg(db);
            returnInt = false;
        }
    }

    return returnInt;
}

bool DatabaseHandler::getUserInfos(std::string username, int *id, std::vector<std::string> *directories, std::string *errMsg)
{

    int rc;
    int returnInt = true;

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "SELECT * FROM users WHERE username = ?", -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
        *errMsg = sqlite3_errmsg(db);
        returnInt = false;
    } else {

        sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), NULL);

        /* unsigned int total_count = 0;
        char *paths = serialize(musicPath, &total_count);
        sqlite3_bind_text(stmt, 3, paths, total_count, NULL);

        sqlite3_bind_int(stmt, 4, total_count); */ // paths size
        sqlite3_bind_int(stmt, 3, 0); // library revision

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            printf("user fetch error: %s\n", sqlite3_errmsg(db));
            *errMsg = sqlite3_errmsg(db);
            returnInt = false;
        }
    }

    return returnInt;
}
