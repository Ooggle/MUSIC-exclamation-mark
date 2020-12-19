#include <stdio.h>
//#include <sqlite3.h>
#include <string>
#include "TagsHandler.h"

int main(int argc, char **argv) {

    TagsHandler *tagsHandler = new TagsHandler();

    std::vector<std::pair<std::string, std::string>> res;

    if(tagsHandler->getTags("/home/ooggle/code/c++/MUSIC-exclamation-mark/src/server/tests/musics/Sonic Generations 20 Years of Sonic Music [FLAC]/02. Angel Island - Act I (Sonic the Hedgehog 3).flac", res) == 0) {
        for(int i = 0; i < res.size(); i++) {
            printf("%s: %s\n", res[i].first.c_str(), res[i].second.c_str());
        }
        
    }



    delete tagsHandler;

    return 0;
}
