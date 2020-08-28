#include <string>
#include <vector>
#include "WebHandler.h"

int main() {

    WebHandler handler("127.0.0.1", 81);
    handler.handlerLoop();

    return EXIT_SUCCESS;
}
