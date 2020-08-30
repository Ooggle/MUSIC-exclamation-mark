/* compile without sqlite3 : g++ -std=c++17 WebHandlerTest.cpp WebHandler.cpp TcpServer.cpp -o WebHandlerTest -static -static-libstdc++
 * with sqlite3 : g++ -std=c++17 WebHandlerTest.cpp WebHandler.cpp TcpServer.cpp -o WebHandlerTest -lsqlite3 -lpthread -ldl -Bstatic -static-libstdc++
*/

#include <string>
#include <vector>
#include <sqlite3.h>
#include "WebHandler.h"

int main() {

    /* WebHandler handler("localhost", 81);
    handler.handlerLoop(); */

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_close(db);

    return EXIT_SUCCESS;
}
