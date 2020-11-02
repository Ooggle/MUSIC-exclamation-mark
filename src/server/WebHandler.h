#ifndef WEBHANDLER_H
#define WEBHANDLER_H

// c++ includes
#include <string>
#include <vector>
#include <thread>

// c includes
#include <sqlite3.h>

// TCP handler class
#include "TcpServer.h"

#include "DatabaseHandler.h"
#include "json.hpp"

enum LAST_REQUEST_STATUS {
    NON_INITIALIZED, // at the beginning
    BAD,
    EMPTY,
    GOOD
};

class WebHandler {
    private:
        TcpServer *tcpServer;

        LAST_REQUEST_STATUS lastRequestStatus;
        std::string lastRequestEndpoint;
        std::string lastRequestType;
        std::vector<std::pair<std::string, std::string>> lastRequestParams;
        bool lastRequestHasContentRange;
        std::pair<std::uintmax_t, std::uintmax_t> lastRequestContentRange;
        bool lastRequestHasTimedOut;

        sqlite3 *db;
        DatabaseHandler *dbHandler;

    public:
        WebHandler(std::string address, int port, sqlite3 *db);
        ~WebHandler();

        std::string getRequestLine();
        std::vector<std::string> getRequestHeader();
        int parseRequest(std::vector<std::string> strSource);
        void handlerLoop();

        void sendJson(nlohmann::json json);
        void sendAlbumsCover(int album_id);
        bool getIntFromHeader(std::string paramName, int &paramValue);
        bool getStringFromHeader(std::string paramName, std::string &paramValue);

        void sendForbiddenResponse();
        void sendMusicFile();
        void sendMusicsDB();
        void sendAlbumsDB();
        void sendArtistsDB();
        void sendHTTPOptions();

        void createUser();
        void getUserInfos();
};

#endif
