/* compile on linux:
 * g++ tagTest.cpp TagsHandler.cpp `pkg-config --cflags --libs libavformat libavutil` -o tagTest
 * 
 * compile on linux for Windows
 * TODO
 * 
 * compile on Windows:
 * g++ .\TagsHandler.cpp .\tagTest.cpp -lavformat -lavcodec -lavutil  -lz -lm -lpthread -o tagTest
 */
#include <stdio.h>
//#include <sqlite3.h>
#include <string>
#include "TagsHandler.h"

int main(int argc, char **argv)
{

    TagsHandler *tagsHandler = new TagsHandler();

    std::vector<std::pair<std::string, std::string>> res;

    uint8_t *data;
    int dataSize = 0;

    /* printf("ret: %d\n", tagsHandler->getCoverImage("D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/Sonic Generations 20 Years of Sonic Music [FLAC]/03. Mushroom Hill - Act I (Sonic & Knuckles).flac", &data, &dataSize));

    FILE* image_file = fopen("image.jpg", "wb");
    int result = fwrite(data, dataSize, 1, image_file);
    fclose(image_file); */

    printf("ret: %d\n", tagsHandler->getTags("D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/Sonic Generations 20 Years of Sonic Music [FLAC]/03. Mushroom Hill - Act I (Sonic & Knuckles).flac", res));
    for (int i = 0; i < res.size(); i++)
    {
        printf("%s = %s\n", res[i].first.c_str(), res[i].second.c_str());
    }

    delete tagsHandler;

    return 0;
}
