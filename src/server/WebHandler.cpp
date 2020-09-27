#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <string.h>
#include <regex.h>
#include "WebHandler.h"

WebHandler::WebHandler(std::string address, int port, sqlite3 *db) {
    tcpServer = new TcpServer();
    tcpServer->openServer(address, port);
    printf("Server opened\n");

    // init all attributes
    this->db = db;
    dbHandler = new DatabaseHandler("music.db");
    lastRequestStatus = LAST_REQUEST_STATUS::NON_INITIALIZED;
    lastRequestEndpoint = "";
    lastRequestType = "";
    lastRequestParams.clear();
    lastRequestHasContentRange = false;
    lastRequestContentRange.first = -1;
    lastRequestContentRange.second = -1;
    lastRequestHasTimedOut = false;
}

WebHandler::~WebHandler() {
    tcpServer->closeServer();
    delete tcpServer;
}

std::string WebHandler::getRequestLine() {
    uint8_t data = 0;
    std::string line = "";
    int timeout = 0, rc = 0;

    while(data != 0x0A && lastRequestHasTimedOut == false)
    {
        rc = tcpServer->getData(&data, 1);

        if(data != 0x0A && rc != -1)
            line += data;

        timeout += 1;
        if(timeout > 100000) {
            lastRequestHasTimedOut = true;
        }
    }

    return line;
}

std::vector<std::string> WebHandler::getRequestHeader() {
    std::vector<std::string> request;
    lastRequestHasTimedOut = false;

    do
    {
        request.push_back(getRequestLine());

        if(lastRequestHasTimedOut)
            break;
        
        /* printf("length: %d\n", request.back().length()); */
        // printf("%s\n", request.back().c_str());
    } while(request.back().length() > 2);
    
    return request;
}

int WebHandler::parseRequest(std::vector<std::string> vectorSource) {
    lastRequestParams.clear();
    lastRequestEndpoint = "";
    lastRequestType = "";
    lastRequestHasContentRange = false;
    lastRequestContentRange.first = -1;
    lastRequestContentRange.second = -1;
    
    regex_t reg;

    // Function call to create regex
    if (regcomp(&reg, "^(GET|OPTIONS)\\s\\/(music|music-db|create-user|get-user-infos)(\\?.*)\\sHTTP", REG_EXTENDED) != 0) {
        printf("Compilation error.\n");
    }
    
    size_t maxMatches = 10;
    size_t maxGroups = 15;
    regmatch_t groupArray[maxGroups];
    const char *source = vectorSource.at(0).c_str();
    
    if(regexec(&reg, source, maxGroups, groupArray, 0) == 0) {
        unsigned int g = 0;
        for(g = 0; g < maxGroups; g++)
        {
            if(groupArray[g].rm_so == (size_t)-1)
                break; // No more groups

            char sourceCopy[strlen(source) + 1];
            strcpy(sourceCopy, source);
            sourceCopy[groupArray[g].rm_eo] = 0;
            /*printf("Group %u: [%2u-%2u]: %s\n",
                    g, groupArray[g].rm_so, groupArray[g].rm_eo,
                    sourceCopy + groupArray[g].rm_so);*/

        }
        // ALL PARAMS
        char sourceCopy[strlen(source) + 1];
        strcpy(sourceCopy, source);
        sourceCopy[groupArray[3].rm_eo] = 0;
        //printf("param source: %s\n", sourceCopy + groupArray[3].rm_so);

        // ENDPOINT
        char sourceCopy2[strlen(source) + 1];
        strcpy(sourceCopy2, source);
        sourceCopy2[groupArray[2].rm_eo] = 0;
        //printf("endpoint: %s\n", sourceCopy2 + groupArray[2].rm_so);
        const char *endTemp = sourceCopy2 + groupArray[2].rm_so;
        lastRequestEndpoint = endTemp;

        // REQUEST TYPE
        char sourceCopy3[strlen(source) + 1];
        strcpy(sourceCopy3, source);
        sourceCopy3[groupArray[1].rm_eo] = 0;
        //printf("request type: %s\n", sourceCopy3 + groupArray[1].rm_so);
        const char *reqTypeTemp = sourceCopy3 + groupArray[1].rm_so;
        lastRequestType = reqTypeTemp;

        /* IF CONTENT-RANGE */
        for(int i = 0; i < vectorSource.size(); i++)
        {
            std::size_t found = vectorSource[i].find("Range");

            // printf("%s\n", vectorSource[i].c_str());
            if (found!=std::string::npos) {
                printf("%s\n", vectorSource[i].c_str());
                printf("Range in request header.\n");

                // create regex
                if (regcomp(&reg, "=([0-9]+)", REG_EXTENDED) != 0) {
                    printf("Compilation error.\n");
                }

                regmatch_t groupArrayRange[maxGroups];
                unsigned int m;
                const char *sourceRange = vectorSource[i].c_str();
                printf("sourceRange: %s\n", sourceRange);

                if(regexec(&reg, sourceRange, maxGroups, groupArrayRange, 0) == 0) {
                    
                    char sourceCopy[strlen(sourceRange) + 1];
                    strcpy(sourceCopy, sourceRange);
                    sourceCopy[groupArrayRange[1].rm_eo] = 0;
                    // printf("start byte: %lu\n", std::stoi(sourceCopy + groupArrayRange[1].rm_so));
                    lastRequestHasContentRange = true;
                    lastRequestContentRange.first = std::stoi(sourceCopy + groupArrayRange[1].rm_so);
                }



                regfree(&reg);
                
            } else {
                // printf("No Range in request header.\n");
            }
            
        }
        


        /* 2ND PART */

        // create regex
        if (regcomp(&reg, "[?&]+([^=&]+)=([^&]*)", REG_EXTENDED) != 0) {
            printf("Compilation error.\n");
        }

        regmatch_t groupArray2[maxGroups];
        unsigned int m;
        const char *source = sourceCopy + groupArray[3].rm_so;

        regoff_t last_match = 0;
        int Numparams = 0;
        std::string paramName;
        std::string paramValue;
        bool noMoreGroups = false;
        
        while(regexec(&reg, source + last_match, maxGroups, groupArray2, 0) == 0) {
            g = 0;
            Numparams += 1;
            for(g = 0; g < maxGroups; g++)
            {
                if(groupArray2[g].rm_so == (size_t)-1) {
                    noMoreGroups = true;
                    break; // No more groups
                }

                char sourceCopy[strlen(source) + last_match + 1];
                strcpy(sourceCopy, source);
                sourceCopy[groupArray2[g].rm_eo + last_match] = 0;
                if(g == 1) {
                    paramName = sourceCopy + groupArray2[g].rm_so + last_match;
                } else if(g == 2) {
                    paramValue = sourceCopy + groupArray2[g].rm_so + last_match;
                }

                /* printf("Group %u: [%2u-%2u]: %s\n",
                        g, groupArray2[g].rm_so + last_match, groupArray2[g].rm_eo + last_match,
                        sourceCopy + groupArray2[g].rm_so + last_match); */
                
            }
            //printf("\n");
            if(noMoreGroups == true) {
                noMoreGroups = false;
                lastRequestParams.push_back(std::make_pair(paramName, paramValue));
            }

            last_match += groupArray2[0].rm_so + 1;
        }

        if(Numparams == 0) {
            lastRequestStatus = LAST_REQUEST_STATUS::EMPTY;
        } else {
            lastRequestStatus = LAST_REQUEST_STATUS::GOOD;
        }

        regfree(&reg);

    } else {
        lastRequestStatus = LAST_REQUEST_STATUS::BAD;
        return -1;
    }

    regfree(&reg);

    return 0;
}

void WebHandler::handlerLoop() {
    
    while(1)
    {
        tcpServer->connectAClient();
        //printf("Client connected\n");

        std::vector<std::string> requestHeader = getRequestHeader();
        if(lastRequestHasTimedOut == false) {
            parseRequest(requestHeader);
            
            printf("%s\n", lastRequestType.c_str());
            if(lastRequestStatus == LAST_REQUEST_STATUS::GOOD) {
                if(lastRequestType.compare("GET") == 0)
                {
                    if(lastRequestEndpoint.compare("music") == 0) {
                        printf(" --------------- music request ---------------\n");
                        sendMusicFile();
                        printf("Disconnecting client...\n");
                        tcpServer->disconnectAClient();
                        printf("Client disconnected\n");
                    } else if(lastRequestEndpoint.compare("music-db") == 0) {
                        printf(" -------------- music-db request -------------\n");
                        sendMusicsDB();
                        printf("Disconnecting client...\n");
                        tcpServer->disconnectAClient();
                        printf("Client disconnected\n");
                    } else if(lastRequestEndpoint.compare("create-user") == 0) {
                        printf(" -------------- create-user request -------------\n");
                        createUser();
                        printf("Disconnecting client...\n");
                        tcpServer->disconnectAClient();
                        printf("Client disconnected\n");
                    } else if(lastRequestEndpoint.compare("get-user-infos") == 0) {
                        printf(" -------------- get-user-infos request -------------\n");
                        getUserInfos();
                        printf("Disconnecting client...\n");
                        tcpServer->disconnectAClient();
                        printf("Client disconnected\n");
                    } else {
                        printf(" ------------- Forbidden request -------------\n");
                        sendForbiddenResponse();
                        printf("Disconnecting client...\n");
                        tcpServer->disconnectAClient();
                        printf("Client disconnected\n");
                    }
                } else if(lastRequestType.compare("OPTIONS") == 0) {
                    //printf(" -------------- OPTIONS request --------------\n");
                    sendHTTPOptions();
                    //printf("Disconnecting client...\n");
                    tcpServer->disconnectAClient();
                    //printf("Client disconnected\n");
                } else {
                    //printf(" -------------- Bad request type -------------\n");
                    sendForbiddenResponse();
                    //printf("Disconnecting client...\n");
                    tcpServer->disconnectAClient();
                    //printf("Client disconnected\n");
                }
                
            } else {
                //printf(" ---------------- bad request ----------------\n");
                sendForbiddenResponse();
                //printf("Disconnecting client...\n");
                tcpServer->disconnectAClient();
                //printf("Client disconnected\n");
            }
        } else {
            //printf(" ---------------- Timed out ----------------\n");
            //printf("Disconnecting client...\n");
            tcpServer->disconnectAClient();
            //printf("Client disconnected\n");
        }
        
        
        
        /* printf("Disconnecting client...\n");
        tcpServer->disconnectAClient();
        printf("Client disconnected\n"); */
    }
    
}

void WebHandler::sendJson(nlohmann::json json) {

    std::string jsonDump = json.dump(4);

    std::uintmax_t totalFileSize = jsonDump.size();
    /* printf("file size : %d\n", totalFileSize); */

    // header sending
    std::string header;
    header = "HTTP/1.1 200 OK\r\n";
    header += "Access-Control-Allow-Origin: *\r\n";
    header += "Content-Type: application/json\r\n";
    header += "Content-Length: ";
    header += std::to_string(totalFileSize);
    header += "\r\n";
    header += "\r\n";

    // printf("%s", header.c_str());
    tcpServer->sendData((void *)header.c_str(), header.length());
    /* printf("header sent.\n"); */

    // file sending
    int sizeToRead = 50000;
    char lastBuffer[sizeToRead];
    std::uintmax_t readedSize = 0;
    while(readedSize < totalFileSize)
    {
        if((readedSize + sizeToRead) > totalFileSize) {
            sizeToRead = totalFileSize - readedSize;
        }
        
        tcpServer->sendData((void *)(jsonDump.c_str() + readedSize), sizeToRead);
        readedSize += sizeToRead;
    }
}


void WebHandler::sendForbiddenResponse() {
    // header sending
    //printf("sending data...");
    std::string header;
    // audio/ogg pour ogg et flac, audio/mpeg pour mp3
    header = "HTTP/1.1 403 Forbidden\r\n";
    header += "\r\n";

    //printf("%s\n", "403 Forbidden");
    tcpServer->sendData((void *)header.c_str(), header.length());
}

void WebHandler::sendMusicFile() {
    printf("\n\n");

    std::uintmax_t byteToStartFrom = 0;

    // file choosing
    int choosedID = 0;
    std::string choosedFile;
    std::string choosedFileExt;

    for(int i = 0; i < lastRequestParams.size(); i++)
    {
        if(lastRequestParams[i].first.compare("id") == 0) {
            choosedID = (int) strtol(lastRequestParams[i].second.c_str(), (char **)NULL, 10);
            //choosedID = std::stoi(lastRequestParams[i].second);
            /* printf("param: %s, value: %s\n", lastRequestParams[i].first.c_str(), lastRequestParams[i].second.c_str()); */
            printf("music id: %d\n", choosedID);
            break;
        }
    }
    if(choosedID == 0) {
        printf("Invalid ID, aborting...\n");
        sendForbiddenResponse();
        return;
    } else {
        char select[] = "SELECT * FROM musics WHERE id = ?";
        sqlite3_stmt *stmt;
        if(sqlite3_prepare_v2(db, select, -1, &stmt, NULL) != SQLITE_OK) {
            printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            sqlite3_finalize(stmt);

            printf("Error in SQL Prepare, aborting...\n");
            sendForbiddenResponse();
            return;
        }

        bool found = false;

        if(sqlite3_bind_int(stmt, 1, choosedID) != SQLITE_OK) {
            printf("Error in SQL Bind, aborting...\n");
            sendForbiddenResponse();
            return;
        }

        // execute sql statement
        int ret_code = 0;
        ret_code = sqlite3_step(stmt);
        if(ret_code == SQLITE_ROW) {
            std::string path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            choosedFile = path;
            printf("music: %s\n", path.c_str());

            std::string ext = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            choosedFileExt = ext;
            /* printf("ext: %s\n", ext.c_str()); */
            found = true;
        }

        if(!found) {
            printf("Incorrect ID, aborting...\n");
            sendForbiddenResponse();
            return;
        }

        sqlite3_finalize(stmt);
    }
    

    // Range checking
    if(lastRequestHasContentRange) {
        byteToStartFrom = lastRequestContentRange.first;
    }
    

    // file loading
    std::ifstream myFile;
    myFile.open(choosedFile.c_str(), std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    if(!myFile.is_open())
        return;
    
    // seek to start position
    if(byteToStartFrom > 0)
    {
        myFile.seekg(byteToStartFrom, std::ios::beg);
    }
    

    std::uintmax_t totalFileSize = std::filesystem::file_size(choosedFile.c_str());
    printf("file size : %d\n", totalFileSize);

    // header sending
    /* printf("sending data..."); */
    std::string header;
    // audio/ogg ou audio/flac pour ogg et flac, audio/mpeg pour mp3
    if(lastRequestHasContentRange) {
        header = "HTTP/1.1 206 Partial Content\r\n";
    } else {
        header = "HTTP/1.1 200 OK\r\n";
    }
    
    header += "Cache-Control: no-cache, private\r\n";
    if(choosedFileExt.compare(".flac") == 0)
    {
        header += "Content-Type: audio/flac\r\n";
    } else if(choosedFileExt.compare(".mp3") == 0) {
        header += "Content-Type: audio/mpeg\r\n";
    } else if(choosedFileExt.compare(".ogg") == 0) {
        header += "Content-Type: audio/ogg\r\n";
    }
    
    header += "Accept-Ranges: bytes\r\n";

    if(lastRequestHasContentRange)
    {
        char tempbuffer[150];
        sprintf(tempbuffer, "Content-Range: bytes %lu-%lu/%lu\r\n", byteToStartFrom, totalFileSize - 1, totalFileSize);
        std::string buffsprintf = tempbuffer;
        /* printf("%s\n", tempbuffer); */
        header += buffsprintf;
    }

    header += "Content-Length: ";
    header += std::to_string(totalFileSize - byteToStartFrom);
    header += "\r\n";

    header += "\r\n";

    // printf("%s", header.c_str());
    tcpServer->sendData((void *)header.c_str(), header.length());

    // file sending
    int sizeToRead = 50000;
    char lastBuffer[sizeToRead];
    std::uintmax_t readedSize = byteToStartFrom;
    while(readedSize < totalFileSize)
    {
        if((readedSize + sizeToRead) > totalFileSize) {
            sizeToRead = totalFileSize - readedSize;
        }
        if(!myFile.read(lastBuffer, sizeToRead)) {
            printf("Unable to read the file\n");
            printf("\n\n");
            return;
        }
        
        //printf("readed: %lu\n", readedSize);
        if (tcpServer->sendData((void *)lastBuffer, sizeToRead) == -1) {
            printf("Client closed connection during transfer (%.2fMb/%.2fMb).\n", (float)readedSize / 1000000, (float)totalFileSize / 1000000);
            printf("\n\n");
            return;
        }
        readedSize += sizeToRead;
    }
    myFile.close();
    printf("\n\n");
}

void WebHandler::sendMusicsDB() {
    /* printf("\n\n"); */

    char select[] = "SELECT * FROM musics";
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db, select, -1, &stmt, NULL) != SQLITE_OK) {
        printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        sqlite3_finalize(stmt);

        printf("Error in SQL Prepare, aborting...\n");
        sendForbiddenResponse();
        return;
    }

    nlohmann::json json;


    // execute sql statement to create json
    int ret_code = 0;
    int rownum = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {

        json["musics"][rownum]["id"] = sqlite3_column_int(stmt, 0);
        json["musics"][rownum]["filename"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        json["musics"][rownum]["album_id"] = sqlite3_column_int(stmt, 4);
        json["musics"][rownum]["genre"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        json["musics"][rownum]["track_number"] = sqlite3_column_int(stmt, 6);
        json["musics"][rownum]["comment"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        json["musics"][rownum]["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        json["musics"][rownum]["artist"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        json["musics"][rownum]["year"] = sqlite3_column_int(stmt, 10);

        rownum += 1;
    }

    sqlite3_finalize(stmt);

    sendJson(json);
    /* printf("\n\n"); */
}

void WebHandler::sendAlbumsDB() {
    
}

void WebHandler::sendHTTPOptions() {
    /* printf("\n\n"); */
    // header sending
    /* printf("sending data..."); */
    std::string header;

    header = "HTTP/1.1 204 No Content\r\n";
    header += "Allow: OPTIONS, GET\r\n";
    header += "Access-Control-Allow-Origin: *\r\n";
    header += "\r\n";

    printf("%s", header.c_str());
    tcpServer->sendData((void *)header.c_str(), header.length());

    /* printf("\n\n"); */
}

void WebHandler::createUser() {

    std::string user, password;

    for(int i = 0; i < lastRequestParams.size(); i++)
    {
        if(lastRequestParams[i].first.compare("username") == 0) {
            user = lastRequestParams[i].second;
            printf("user: %s\n", user.c_str());
            break;
        }
    }

    if(user.compare("") == 0) {
        nlohmann::json json;
        json["result_code"] = 2;
        json["message"] = "Error, no username provided.";

        sendJson(json);

        return;
    }

    for(int i = 0; i < lastRequestParams.size(); i++)
    {
        if(lastRequestParams[i].first.compare("password") == 0) {
            password = lastRequestParams[i].second;
            break;
        }
    }

    if(password.compare("") == 0) {
        nlohmann::json json;
        json["result_code"] = 3;
        json["message"] = "Error, no password provided.";

        sendJson(json);

        return;
    }

    std::string errMsg;
    if(dbHandler->createUser(user, password, &errMsg)) {

        nlohmann::json json;
        json["result_code"] = 0;
        json["message"] = "User created successfully.";

        sendJson(json);

        return;
        
    } else {

        nlohmann::json json;
        json["result_code"] = -1;
        json["message"] = errMsg.c_str();

        sendJson(json);

    }

    return;
}

void WebHandler::getUserInfos() {

    std::string user;

    for(int i = 0; i < lastRequestParams.size(); i++)
    {
        if(lastRequestParams[i].first.compare("username") == 0) {
            user = lastRequestParams[i].second;
            printf("user: %s\n", user.c_str());
            break;
        }
    }

    if(user.compare("") == 0) {
        nlohmann::json json;
        json["result_code"] = 2;
        json["message"] = "Error, no username provided.";

        sendJson(json);

        return;
    }

    char select[] = "SELECT * FROM users WHERE username = ?";
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db, select, -1, &stmt, NULL) != SQLITE_OK) {
        printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        sqlite3_finalize(stmt);

        nlohmann::json json;
        json["result_code"] = 3;
        json["message"] = "Request error.";

        sendJson(json);

        return;

    }

    sqlite3_bind_text(stmt, 1, user.c_str(), user.length(), NULL);

    // execute sql statement to create json
    int ret_code = 0;
    int rownum = 0;
    ret_code = sqlite3_step(stmt);
    
    if(ret_code == SQLITE_ROW) {

        nlohmann::json json;
        json["result_code"] = 0;

        json["informations"]["id"] = sqlite3_column_int(stmt, 0);

        int pathsSize = sqlite3_column_int(stmt, 4);

        const char* paths = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        std::vector<std::string> pathVector;
        dbHandler->deserialize(pathVector, paths, pathsSize);
        printf("%s\n", paths);
        printf("path size: %d\n", pathVector.size());

        for(int i = 0; i < pathVector.size(); i++) {
            json["informations"]["directories"][i] = pathVector[i].c_str();
            printf("%d: %s\n", i, pathVector[i].c_str());
        }

        json["informations"]["library_revision"] = sqlite3_column_int(stmt, 5);
        json["informations"]["creation_date"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

        sendJson(json);
    } else {

        nlohmann::json json;
        json["result_code"] = 4;
        json["message"] = "Username not found.";

        sendJson(json);

        return;
    }

    sqlite3_finalize(stmt);

    return;
}
