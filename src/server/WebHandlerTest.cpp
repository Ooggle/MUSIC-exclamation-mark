#include <string>
#include <vector>
#include "WebHandler.h"

int main() {

    WebHandler handler("192.168.0.44", 80);
    handler.handlerLoop();

    return EXIT_SUCCESS;
}
