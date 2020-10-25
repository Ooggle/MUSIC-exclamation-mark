/*
    Compilation : g++ main.cpp -std=c++17 `sdl2-config --libs` -lSDL2_mixer `taglib-config --libs` `pkg-config --cflags --libs taglib` -lz
    Some flags are useless for now.
*/

#include "DatabaseHandler.h"
#include "WebHandler.h"
#include <stdio.h>

#define SERVER_VERSION "0.1"

int main()
{
    printf("Welcome to MUSIC! server %s.\n", SERVER_VERSION);

    return EXIT_SUCCESS;
}
