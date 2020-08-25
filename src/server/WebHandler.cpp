#include <string>
#include <fstream>
#include <filesystem>
#include <string.h>
#include <regex.h>
#include "WebHandler.h"

WebHandler::WebHandler(std::string address, int port) {
    tcpServer = new TcpServer();
    tcpServer->openServer(address, port);
    printf("Server opened\n");

    lastRequestStatus = LAST_REQUEST_STATUS::NON_INITIALIZED;  
    lastRequestEndpoint = "";
    lastRequestParams.clear();
}

WebHandler::~WebHandler() {
    tcpServer->closeServer();
    delete tcpServer;
}

std::string WebHandler::getRequestLine() {
    uint8_t data = 0;
    std::string line = "";

    while(data != 0x0A)
    {
        tcpServer->getData(&data, 1);

        if(data != 0x0A)
            line += data;
    }

    return line;
}

std::vector<std::string> WebHandler::getRequestHeader() {
    std::vector<std::string> request;

    do
    {
        request.push_back(getRequestLine());
        /* printf("length: %d\n", request.back().length());
        printf("%s\n", request.back().c_str()); */
    } while(request.back().length() > 2);
    
    return request;
}

int WebHandler::parseRequest(std::string strSource) {
    lastRequestParams.clear();
    lastRequestEndpoint = "";
    
    regex_t reg;

    // Function call to create regex
    if (regcomp(&reg, "^GET\\s\\/(music|database)(\\?.*)\\sHTTP", REG_EXTENDED) != 0) {
        printf("Compilation error.\n");
    }
    
    size_t maxMatches = 10;
    size_t maxGroups = 15;
    regmatch_t groupArray[maxGroups];
    const char *source = strSource.c_str();
    
    if(regexec(&reg, source, maxGroups, groupArray, 0) == 0) {
        unsigned int g = 0;
        for(g = 0; g < maxGroups; g++)
        {
            if(groupArray[g].rm_so == (size_t)-1)
                break; // No more groups

            char sourceCopy[strlen(source) + 1];
            strcpy(sourceCopy, source);
            sourceCopy[groupArray[g].rm_eo] = 0;
            /* printf("Group %u: [%2u-%2u]: %s\n",
                    g, groupArray[g].rm_so, groupArray[g].rm_eo,
                    sourceCopy + groupArray[g].rm_so); */

        }
        // ALL PARAMS
        char sourceCopy[strlen(source) + 1];
        strcpy(sourceCopy, source);
        sourceCopy[groupArray[2].rm_eo] = 0;
        //printf("param source: %s\n", sourceCopy + groupArray[2].rm_so);

        // ENDPOINT
        char sourceCopy2[strlen(source) + 1];
        strcpy(sourceCopy2, source);
        sourceCopy2[groupArray[1].rm_eo] = 0;
        //printf("endpoint: %s\n", sourceCopy2 + groupArray[1].rm_so);
        const char *endTemp = sourceCopy2 + groupArray[1].rm_so;
        lastRequestEndpoint = endTemp;
        printf("%s %s %s\n", lastRequestEndpoint.c_str(), lastRequestEndpoint.c_str(), lastRequestEndpoint.c_str());

        /* 2ND PART */

        // create regex
        if (regcomp(&reg, "[?&]+([^=&]+)=([^&]*)", REG_EXTENDED) != 0) {
            printf("Compilation error.\n");
        }

        regmatch_t groupArray2[maxGroups];
        unsigned int m;
        const char *source = sourceCopy + groupArray[2].rm_so;

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
        printf("Client connected\n");

        std::vector<std::string> requestHeader = getRequestHeader();
        parseRequest(requestHeader.at(0).c_str());
        if(lastRequestStatus == LAST_REQUEST_STATUS::GOOD)
        {
            printf("%s\n", lastRequestEndpoint.c_str());
            if(lastRequestEndpoint.compare("music") == 0) {
                printf("music\n");
                sendMusicFile();
            } else if(lastRequestEndpoint.compare("database") == 0) {
                printf("database\n");
                sendForbiddenResponse();
            } else {
                printf("sendForbiddenResponse\n");
                sendForbiddenResponse();
            }
            
        } else {
            sendForbiddenResponse();
        }
        

        tcpServer->disconnectAClient();
    }
    
}

void WebHandler::sendForbiddenResponse() {
    // header sending
    printf("sending data...");
    std::string header;
    // audio/ogg pour ogg et flac, audio/mpeg pour mp3
    header = "HTTP/1.1 403 Forbidden\r\n";
    header += "\r\n";

    printf("%s\n", "403 Forbidden");
    tcpServer->sendData((void *)header.c_str(), header.length());
}

void WebHandler::sendMusicFile() {
    // file loading
    std::ifstream myFile;
    myFile.open("tests/musics/sample.mp3", std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    if(!myFile.is_open())
        return;

    std::uintmax_t totalFileSize = std::filesystem::file_size("tests/musics/sample.mp3");
    printf("file size : %d\n", totalFileSize);

    // header sending
    printf("sending data...");
    std::string header;
    // audio/ogg pour ogg et flac, audio/mpeg pour mp3
    header = "HTTP/1.1 200 OK\r\n";
    header += "Connection: keep-alive\r\n";
    header += "Content-Type: audio/mpeg\r\n";
    header += "Content-Length: ";
    header += std::to_string(totalFileSize);
    header += "\r\n";
    header += "\r\n";

    printf("%s", header.c_str());
    tcpServer->sendData((void *)header.c_str(), header.length());

    // file sending
    int sizeToRead = 5000;
    char FileBuffer[totalFileSize];
    char lastBuffer[sizeToRead];
    std::uintmax_t readedSize = 0;
    while(readedSize < totalFileSize)
    {
        if((readedSize + sizeToRead) > totalFileSize) {
            sizeToRead = totalFileSize - readedSize;
        }
        if(!myFile.read(lastBuffer, sizeToRead))
            return;
        
        tcpServer->sendData((void *)lastBuffer, sizeToRead);
        readedSize += sizeToRead;
    }
}
