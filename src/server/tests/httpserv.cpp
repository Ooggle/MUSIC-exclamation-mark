#include <unistd.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "ServeurTcp.h"

int regexTwo(std::string paramsSource);

std::string getRequestLine(ServeurTcp *server) {
    uint8_t data = 0;
    std::string line = "";

    while(data != 0x0A)
    {
        server->recevoirData(&data, 1);

        if(data != 0x0A)
            line += data;
    }

    return line;
}

std::vector<std::string> getRequestHeader(ServeurTcp *server) {
    std::vector<std::string> request;

    do
    {
        request.push_back(getRequestLine(server));
        /* printf("length: %d\n", request.back().length());
        printf("%s\n", request.back().c_str()); */
    } while(request.back().length() > 2);
    

    return request;
}

int regexOne(std::string strSource) {
    regex_t reg;

    // Function call to create regex
    if (regcomp(&reg, "^GET\\s\\/(music|database)(\\?.*)\\sHTTP", REG_EXTENDED) != 0) {
        printf("Compilation error.\n");
    }
    
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
            printf("Group %u: [%2u-%2u]: %s\n",
                    g, groupArray[g].rm_so, groupArray[g].rm_eo,
                    sourceCopy + groupArray[g].rm_so);

        }
        printf("\n");
        char sourceCopy[strlen(source) + 1];
        strcpy(sourceCopy, source);
        sourceCopy[groupArray[2].rm_eo] = 0;
        regexTwo(sourceCopy + groupArray[2].rm_so);
    } else {
        printf("no pattern found\n");
    }

    regfree(&reg);

    return 0;
}

int regexTwo(std::string paramsSource) {
    regex_t reg;

    // create regex
    if (regcomp(&reg, "[?&]+([^=&]+)=([^&]*)", REG_EXTENDED) != 0) {
        printf("Compilation error.\n");
    }

    size_t maxMatches = 5;
    size_t maxGroups = 15;
    regmatch_t groupArray[maxGroups];
    unsigned int m;
    const char *srcConst = paramsSource.c_str();
    char *source;
    strcpy(source, srcConst);

    regoff_t last_match = 0;
    int Numparams = 0;
    
    printf("avant\n");
    while(regexec(&reg, source + last_match, maxGroups, groupArray, 0) == 0) {
        printf("après\n");
        printf("0\n");
        unsigned int g = 0;
        Numparams += 1;
        for(g = 0; g < maxGroups; g++)
        {
            if(groupArray[g].rm_so == (size_t)-1)
                break; // No more groups

            char sourceCopy[strlen(source) + last_match + 1];
            strcpy(sourceCopy, source);
            sourceCopy[groupArray[g].rm_eo + last_match] = 0;
            /*printf("Group %u: [%2u-%2u]: %s\n",
                    g, groupArray[g].rm_so + last_match, groupArray[g].rm_eo + last_match,
                    sourceCopy + groupArray[g].rm_so + last_match);*/
            
        }
        if(groupArray[g].rm_so != (size_t)-1) {
            printf("1\n");
            char sourceParamName[strlen(source) + last_match + 1];
            printf("2\n");
            strcpy(sourceParamName, source);
            printf("3\n");
            sourceParamName[groupArray[1].rm_eo + last_match] = 0;
            printf("4\n");

            char sourceParamValue[strlen(source) + last_match + 1];
            strcpy(sourceParamValue, source);
            sourceParamValue[groupArray[2].rm_eo + last_match] = 0;

            printf("5\n");
            printf("Param %d: %s = %s\n", Numparams, sourceParamName + groupArray[1].rm_so + last_match, sourceParamValue + groupArray[2].rm_so + last_match);
        }
        last_match += groupArray[0].rm_so + 1;
    }

    regfree(&reg);

    return 0;
}

int main()
{
    ServeurTcp leServeur;

    leServeur.ouvrir("192.168.0.44", 80);
    printf("serveur ouvert\n");
    leServeur.connecterUnClient();
    printf("client connecte\n");


    uint8_t data;
    int i = 0;

    char getStr[255] = {0};
    int getSize = 0;
    printf("\n\n");

    std::vector<std::string> requestHeader = getRequestHeader(&leServeur);

    std::vector<std::pair<std::string, std::string>> URLInfos;

    regexOne(requestHeader.at(0).c_str());
    
    
    printf("request : %s\n", getStr);



    // file loading
    /*std::ifstream myFile;
    myFile.open("musics/sample.mp3", std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    if(!myFile.is_open())
        return EXIT_FAILURE;

    std::uintmax_t totalFileSize = std::filesystem::file_size("musics/sample.mp3");
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
    leServeur.emettreData((void *)header.c_str(), header.length());

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
            return EXIT_FAILURE;
        
        leServeur.emettreData((void *)lastBuffer, sizeToRead);
        readedSize += sizeToRead;
    }*/
    
    leServeur.deconnecterUnClient();
    leServeur.fermer();

    sleep(0.5);
    return EXIT_SUCCESS;
}
