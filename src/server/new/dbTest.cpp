/* to compile:
 * g++ -std=c++17 .\TagsHandler.cpp .\DatabaseHandler.cpp .\localFilesHandler.cpp .\dbTest.cpp -lavformat -lavcodec -lavutil -lsqlite3 -lz -lm -lpthread -lstdc++fs -o dbTest
 */
#include "DatabaseHandler.h"

int main()
{
    DatabaseHandler *dbHandler = new DatabaseHandler("test.db");
    
    if(!dbHandler->isDatabaseValid()) {
        std::string *errMsg;
        dbHandler->createTables();
        dbHandler->createUser("Ooggle", "password", errMsg);
    }
    
    dbHandler->addMusicsforUser("D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/", 1);

    return 0;
}
