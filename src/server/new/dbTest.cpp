#include <stdio.h>
#include <sqlite3.h>
#include <string>
//#include "TagsHandler.h"
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavutil/dict.h>
    #include <libavutil/error.h>
}

int main(int argc, char **argv) {

    AVFormatContext* av_format_cx = avformat_alloc_context();
    AVDictionaryEntry *tag = NULL;
    int ret = 0;

    av_register_all();
    std::string filename = "/home/ooggle/code/c++/MUSIC-exclamation-mark/src/server/tests/musics/Yakuza Like A Dragon Karaoke Hit Song Collection/06. Bakamitai (Full Spec Edition).mp3";

    ret = avformat_open_input(&av_format_cx, filename.c_str(), NULL, NULL);
    if(ret != 0) {
        printf("Couldn't open file\n");
        char buff[255];
        av_strerror(ret, buff, 255);
        printf("err: %s\n", buff);
        return 1;
    }

    /* while(tag = av_dict_get(av_format_cx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)) {

        char buff[255] = {0};
        for(int i = 0; tag->key[i]; i++){
            buff[i] = tolower(tag->key[i]);
        }

        printf("%s = %s\n", buff, tag->value);
    } */
    tag = av_dict_get(av_format_cx->metadata, "tidfgtle", tag, AV_DICT_IGNORE_SUFFIX);
    if(tag != 0) {
        char buff[255] = {0};
        for(int i = 0; tag->key[i]; i++){
            buff[i] = tolower(tag->key[i]);
        }

        printf("%s = %s\n", buff, tag->value);
    }

    avformat_close_input(&av_format_cx);
    return 0;
}
