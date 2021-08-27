#include "TagsHandler.h"

TagsHandler::TagsHandler()
{
    // No more needed in ffmpeg 4.0
    //av_register_all();
}

TagsHandler::~TagsHandler()
{

}

int TagsHandler::getSpecificTag(std::string filename, std::string tagName, std::string *value)
{
    AVFormatContext* av_format_cx = avformat_alloc_context();
    AVDictionaryEntry *tag = NULL;
    int ret = 0;

    ret = avformat_open_input(&av_format_cx, filename.c_str(), NULL, NULL);
    if(ret != 0) {
       /*  printf("Couldn't open file\n");
        char buff[255];
        av_strerror(ret, buff, 255);
        printf("err: %s\n", buff); */
        ret = -1;
    }

    if(ret == 0) {
        tag = av_dict_get(av_format_cx->metadata, tagName.c_str(), tag, AV_DICT_IGNORE_SUFFIX);
        if(tag != 0) {
            *value = tag->value;
        } else {
            ret = -1;
        }
        avformat_close_input(&av_format_cx);
    }
    return ret;
}

int TagsHandler::getTags(std::string filename, std::vector< std::pair<std::string, std::string> > &values)
{
    AVFormatContext* av_format_cx = avformat_alloc_context();
    AVDictionaryEntry *tag = NULL;
    int ret = 0;

    ret = avformat_open_input(&av_format_cx, filename.c_str(), NULL, NULL);
    if(ret != 0) {
        printf("Couldn't open file\n");
        char buff[255];
        av_strerror(ret, buff, 255);
        printf("err: %s\n", buff);
        ret = -1;
    }

    if(ret == 0) {
        while(tag = av_dict_get(av_format_cx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)) {
            char buff[100] = {0};
            for(int i = 0; tag->key[i]; i++){
                buff[i] = tolower(tag->key[i]);
            }
            values.push_back(std::make_pair(std::string(buff), std::string(tag->value)));
        }
        avformat_close_input(&av_format_cx);
    }
    return ret;
}

int TagsHandler::getCoverImage(std::string filename, uint8_t **data, int *dataSize)
{
    AVFormatContext* av_format_cx = avformat_alloc_context();
    AVPacket pkt;
    int ret = 0;

    ret = avformat_open_input(&av_format_cx, filename.c_str(), NULL, NULL);
    if(ret != 0) {
        ret = -1;
    }

    if(ret == 0) {
        ret = av_read_frame(av_format_cx, &pkt);

        if(pkt.stream_index > 0) {
            *data = pkt.data;
            *dataSize = pkt.size;
        } else {
            ret = -1;
        }
    }
    return ret;
}
