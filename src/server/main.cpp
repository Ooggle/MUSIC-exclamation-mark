/*
    Compilation : g++ main.cpp -std=c++17 `sdl2-config --libs` -lSDL2_mixer `taglib-config --libs` `pkg-config --cflags --libs taglib` -lz
    Some flags are useless for now.
*/

#include "JsonDatabase.h"
#include "DBConfig.h"
#include "DBMusic.h"
#include <stdio.h>

#define SERVER_VERSION 0.1

int main()
{
    printf("Welcome to MUSIC! server %2.1F.\n", SERVER_VERSION);

    return EXIT_SUCCESS;
}
