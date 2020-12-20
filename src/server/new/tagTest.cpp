#include <stdio.h>
//#include <sqlite3.h>
#include <string>
#include "TagsHandler.h"

int main(int argc, char **argv) {

    TagsHandler *tagsHandler = new TagsHandler();

    std::vector<std::pair<std::string, std::string>> res;

    uint8_t *data;
    int dataSize = 0;

    printf("ret: %d\n", tagsHandler->getImageCover("/home/ooggle/code/c++/MUSIC-exclamation-mark/src/server/tests/musics/Sonic Generations 20 Years of Sonic Music [FLAC]/01. SEGA.flac", &data, &dataSize));

    FILE* image_file = fopen("image.jpg", "wb");
    int result = fwrite(data, dataSize, 1, image_file);
    fclose(image_file);

    delete tagsHandler;

    return 0;
}
