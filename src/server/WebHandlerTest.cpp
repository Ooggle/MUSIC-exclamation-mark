/* with sqlite3 : g++ -std=c++17 WebHandlerTest.cpp WebHandler.cpp TcpServer.cpp -o WebHandlerTest -lsqlite3 -lpthread -ldl -Bstatic -static-libstdc++
*/

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3/sqlite3.h"

#include "WebHandler.h"
#include "DatabaseHandler.h"

/* static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
} */

int main()
{

    sqlite3 *db;

    WebHandler handler("localhost", 81, db);
    handler.handlerLoop();

    /* sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }


    char *sql;
    /* Create SQL statement */
    /* sql = "CREATE TABLE COMPANY("  \
        "ID INT PRIMARY KEY     NOT NULL," \
        "NAME           TEXT    NOT NULL," \
        "AGE            INT     NOT NULL," \
        "ADDRESS        CHAR(50)," \
        "SALARY         REAL );"; */

    /* Execute SQL statement */
    /* rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    sqlite3_close(db); */

    return EXIT_SUCCESS;
}
