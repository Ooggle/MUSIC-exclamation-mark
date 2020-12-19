#ifndef TAGSHANDLER_H
#define TAGSHANDLER_H

#include <string>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavutil/dict.h>
    #include <libavutil/error.h>
}

class TagsHandler
{
    private:
        

    public:
        TagsHandler();

        ~TagsHandler();

        std::string getTagsFromFile(std::string file);
};

#endif