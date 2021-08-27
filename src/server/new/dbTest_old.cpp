#include "DatabaseHandler.h"

int main()
{
    DatabaseHandler *dbHandler = new DatabaseHandler("test.db");
    if(!dbHandler->isDatabaseValid())
        dbHandler->createTables();
    
    return 0;
}
