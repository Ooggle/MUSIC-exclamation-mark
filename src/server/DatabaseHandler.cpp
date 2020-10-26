#include "DatabaseHandler.h"

DatabaseHandler::DatabaseHandler(std::string DatabasePath)
{
    char *zErrMsg = NULL;

    if(sqlite3_open(DatabasePath.c_str(), &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        isDatabaseInitialised = false;
    } else {
        fprintf(stderr, "Opened database successfully\n");
        isDatabaseInitialised = true;
    }
}

DatabaseHandler::~DatabaseHandler()
{
    sqlite3_close(db);
}

std::vector<std::vector<std::string>> DatabaseHandler::getFilesInDirectory(std::string path)
{
    std::vector<std::vector<std::string>> filenames;
    int i = 0;

    for(const auto & entry : std::filesystem::directory_iterator(path))
    {
        //std::cout << entry.path() << ", directory: " << entry.is_directory() << std::endl;
        if(entry.is_directory()) {
            std::vector<std::vector<std::string>> filenamesTemp = getFilesInDirectory(entry.path());
            filenames.insert(filenames.end(), filenamesTemp.begin(), filenamesTemp.end());
        } else {
            if((entry.path().extension() == ".mp3") ||
            (entry.path().extension() == ".ogg") ||
            (entry.path().extension() == ".flac")) {

                std::vector<std::string> vectTemp;
                vectTemp.push_back(entry.path());
                std::string filenameTemp = entry.path().filename();
                size_t lastindexTemp = filenameTemp.find_last_of(".");
                vectTemp.push_back(filenameTemp.substr(0, lastindexTemp));
                vectTemp.push_back(entry.path().extension());

                filenames.push_back(vectTemp);
            }
        }
        i += 1;
    }
    return filenames;
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
            comment         TEXT(5000),\
            title           VARCHAR(50),\
            artist          VARCHAR(500),\
            music_year            INTEGER\
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
            library_revision    INTEGER,\
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

int DatabaseHandler::addMusicsforUser(std::string path, std::string username)
{
    std::vector<std::vector<std::string>> filenames = getFilesInDirectory(path);

    // print vector of strings
    /* printf("\n      -- ALL FILES --\n");
    for(int i = 0; i < filenames.size(); i++)
    { 
        printf("%s\n", filenames[i].c_str());
    } */

    // Verify username
    int username_id = getUserID(username);
    if(username_id == -1) {
        printf("Cannot add music(s), user not found\n");
        return -1;
    }

    for(int i = 0; i < filenames.size(); i++)
        addMusicforUser(filenames[i], username);

    return 0;
}

int DatabaseHandler::addMusicforUser(std::vector<std::string> file, std::string username)
{

    // Verify username
    int username_id = getUserID(username);
    if(username_id == -1) {
        printf("Cannot add music, user not found\n");
        return -1;
    }
    

    int rc;
    sqlite3_stmt *stmt = NULL;

    TagLib::String fTitle;
    TagLib::String fArtist;
    TagLib::String fAlbum;
    TagLib::String fGenre;
    TagLib::String fComment;
    unsigned int fTrack;
    unsigned int fYear;

    //get music infos
    TagLib::FileRef f(file.at(0).c_str());
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

    char album[200];
    strcpy(album, fAlbum.toCString(true));
    int albumLength = sprintf(album, "%s", album);

    // verify if music, album or artist not already exist

    int artistID, albumID;

    artistID = isArtistExistForUser(username, std::string(artist));
    if(!artistID) {
        stmt = NULL;
        rc = sqlite3_prepare_v2(db, "INSERT INTO artists(name, user_id) VALUES(?, ?)", -1, &stmt, NULL);

        if(rc != SQLITE_OK) {
            printf("prepare failed: %s\n", sqlite3_errmsg(db));
        } else {

            sqlite3_bind_text(stmt, 1, artist, artistLength, NULL); // artist name
            sqlite3_bind_int(stmt, 2, username_id); // user_id

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                printf("execution failed: %s\n", sqlite3_errmsg(db));
            }
            
        }

        artistID = isArtistExistForUser(username, std::string(artist));
    }

    albumID = isAlbumExistForUser(username, std::string(album), fYear);
    if(!albumID) {
        stmt = NULL;
        rc = sqlite3_prepare_v2(db, "INSERT INTO albums(name, user_id, artist_id, album_year) VALUES(?, ?, ?, ?)", -1, &stmt, NULL);

        if(rc != SQLITE_OK) {
            printf("prepare failed: %s\n", sqlite3_errmsg(db));
        } else {

            sqlite3_bind_text(stmt, 1, album, albumLength, NULL); // album name
            sqlite3_bind_int(stmt, 2, username_id); // user_id
            sqlite3_bind_int(stmt, 3, artistID); // artist_id
            sqlite3_bind_int(stmt, 4, fYear); // year

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                printf("execution failed: %s\n", sqlite3_errmsg(db));
            }
            
        }

        albumID = isAlbumExistForUser(username, std::string(album), fYear);
    }

    if(!isMusicExistForUser(username, std::string(genre), fTrack, std::string(title), std::string(artist), fYear)) {
        stmt = NULL;
        rc = sqlite3_prepare_v2(db, "INSERT INTO musics(filename, path, extension, user_id, album_id, genre, track_number, comment, title, artist, music_year) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, NULL);

        if(rc != SQLITE_OK) {
            printf("prepare failed: %s\n", sqlite3_errmsg(db));
        } else {

            sqlite3_bind_text(stmt, 1, file.at(1).c_str(), file.at(1).length(), NULL); // filename
            sqlite3_bind_text(stmt, 2, file.at(0).c_str(), file.at(0).length(), NULL); // path
            sqlite3_bind_text(stmt, 3, file.at(2).c_str(), file.at(2).length(), NULL); // extension
            sqlite3_bind_int(stmt, 4, username_id); // user_id
            sqlite3_bind_int(stmt, 5, albumID); // album ID
            sqlite3_bind_text(stmt, 6, genre, genreLength, NULL); // genre
            sqlite3_bind_int(stmt, 7, fTrack);
            sqlite3_bind_text(stmt, 8, comment, commentLength, NULL);
            sqlite3_bind_text(stmt, 9, title, titleLength, NULL);
            sqlite3_bind_text(stmt, 10, artist, artistLength, NULL);
            sqlite3_bind_int(stmt, 11, fYear);
            

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


bool DatabaseHandler::getIsDatabaseInitialised()
{
    return isDatabaseInitialised;
}

int DatabaseHandler::isMusicExistForUser(std::string username, std::string genre, int track_number, std::string title, std::string artist, int year)
{
    int rc, returnInt = 0;

    int user_id = getUserID(username);
    if(user_id == -1) {
        printf("User not found\n");
        return -1;
    }

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
        sqlite3_bind_text(stmt, 5, artist.c_str(), artist.length(), NULL); // genre
        sqlite3_bind_int(stmt, 6, year); // title

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

int DatabaseHandler::isArtistExistForUser(std::string username, std::string artist)
{
    int rc, returnInt = 0;

    int user_id = getUserID(username);
    if(user_id == -1) {
        printf("User not found\n");
        return -1;
    }

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

int DatabaseHandler::isAlbumExistForUser(std::string username, std::string album_name, int album_year)
{
    int rc, returnInt = 0;

    int user_id = getUserID(username);
    if(user_id == -1) {
        printf("User not found\n");
        return -1;
    }

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

sqlite3 *DatabaseHandler::getDB()
{
    return db;
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
