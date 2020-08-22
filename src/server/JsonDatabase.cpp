#include "Database.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

Database::Database(std::string databaseName) {

}
Database::~Database() {

}

void Database::initDatabase() {

}

void Database::makeDatabaseBakup() {
    
}

int Database::get_databaseContent() {
    return this->databaseContent;
}

int Database::get_dbState() {
    return this->dbState;
}