#ifndef TAGSHANDLER_H
#define TAGSHANDLER_H

#include <string>
#include <vector>
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
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

        // Get the tag referenced by tagName and store it in value. return !0 if error
        int getSpecificTag(std::string filename, std::string tagName, std::string *value);

        int getTags(std::string filename, std::vector<std::pair<std::string, std::string>> &values);

        int getCoverImage(std::string filename, uint8_t **data, int *dataSize);
};

#endif