/*
    date: 2020-08-16
    compile: g++ first.cpp `sdl2-config --libs` -lSDL2_mixer `taglib-config --libs` `pkg-config --cflags --libs taglib` -lz
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unistd.h>
#include <zlib.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>

int main() {

    int temp_ret = 0;

    /* MIXER VERSION */	
    SDL_version compile_version;
    const SDL_version *link_version=Mix_Linked_Version();
    SDL_MIXER_VERSION(&compile_version);
    printf("compiled with SDL_mixer version: %d.%d.%d\n", 
            compile_version.major,
            compile_version.minor,
            compile_version.patch);
    printf("running with SDL_mixer version: %d.%d.%d\n", 
            link_version->major,
            link_version->minor,
            link_version->patch);

    /* INIT */
    temp_ret = Mix_Init(MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_OGG);
    printf("Mix_Init: %d\n", temp_ret - (MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_OGG));

    if(SDL_Init(SDL_INIT_AUDIO)==-1) {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        exit(2);
    }
    
    printf("There are %d music deocoders available\n", Mix_GetNumMusicDecoders());

    // load the MP3 file "music.mp3" to play as music
    Mix_Music *music;
    music=Mix_LoadMUS("bakamitai.mp3");
    if(!music) {
        printf("Mix_LoadMUS(\"bakamitai.mp3\"): %s\n", Mix_GetError());
        // this might be a critical error...
    }

    if(Mix_PlayMusic(music, -1) == -1) {
        printf("Mix_PlayMusic: %s\n", Mix_GetError());
        // well, there's no music, but most games don't break without music...
    }


    TagLib::FileRef f("bakamitai.mp3");
    TagLib::String artist = f.tag()->artist(); // artist == "Frank Zappa"


    int z = 0;
    scanf("%d", &z);

    Mix_RewindMusic();
    printf("hey\n");
    if(Mix_SetMusicPosition(66.0)==-1) {
        printf("Mix_SetMusicPosition: %s\n", Mix_GetError());
    }

    while(getchar() == 0);
    scanf("%d", &z);
    
    /* CLOSE ALL */
    Mix_CloseAudio();
    SDL_Quit();
    Mix_Quit();
    return 0;
}
