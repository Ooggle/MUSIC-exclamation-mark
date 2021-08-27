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
#include <fstream>
#include "TagsHandler.h"

int main(int argc, char **argv)
{

    TagsHandler *tagsHandler = new TagsHandler();

    std::vector<std::pair<std::string, std::string>> res;

    std::ofstream outfile("test.txt", std::ofstream::out);

    uint8_t *data;
    int dataSize = 0;

    /* printf("ret: %d\n", tagsHandler->getCoverImage("D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/Sonic Generations 20 Years of Sonic Music [FLAC]/03. Mushroom Hill - Act I (Sonic & Knuckles).flac", &data, &dataSize));

    FILE* image_file = fopen("image.jpg", "wb");
    int result = fwrite(data, dataSize, 1, image_file);
    fclose(image_file); */

    //char filename[] = "D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/Sonic Generations 20 Years of Sonic Music [FLAC]/03. Mushroom Hill - Act I (Sonic & Knuckles).flac";
    wchar_t filename[] = L"D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/cosMo@暴走P feat_初音ミク/Re_Start/04 千本桜.mp3";

    printf("ret: %d\n", tagsHandler->getTags("D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/cosMo@暴走P feat_初音ミク/Re_Start/04 千本桜.mp3", res));
    for(int i = 0; i < res.size(); i++)
    {
        printf("%s = %s\n", res[i].first.c_str(), res[i].second.c_str());
        outfile.write(res[i].second.c_str(), res[i].second.length());
        outfile.write("\n", 1);
    }

    delete tagsHandler;
    outfile.close();

    return 0;
}



/* using native Windows COM objects1:
 * g++ .\TagsHandler.cpp .\tagTest.cpp -lssl -lole32 -lpropsys -lstrmiids -lz -lm -lpthread -o tagTest
 */
/* #include <stdio.h>
#include <shobjidl.h>   // SHGetPropertyStoreFromParsingName, etc
#include <Initguid.h>
#include <propkey.h>    // PKEY_Music_AlbumArtist
#include <icodecapi.h>
#include <propvarutil.h>// InitPropVariantFromString, needs shlwapi.lib

int main() // error-checking removed
{
    // initialize the COM library
    CoInitialize(NULL);
    HRESULT res = 0;

    // get a property store for the mp3 file
    IPropertyStore* store = NULL;
    res = SHGetPropertyStoreFromParsingName(L"D:\\Ooggle\\DL Files\\_Pers\\CODE\\c++\\MUSIC-exclamation-mark\\src\\server\\tests\\musics\\Sonic Generations 20 Years of Sonic Music [FLAC]\\03. Mushroom Hill - Act I (Sonic & Knuckles).flac", 
        NULL, GPS_DEFAULT, __uuidof(IPropertyStore), (void**)&store);
    printf("hello?\n");
    if(res == S_OK || res == INPLACE_S_TRUNCATED)
    {
        printf("success!\n");
    }
    printf("hum: %x\n", res);

    // get the existing album artist ("Elvis Presley")
    PROPVARIANT variant;
    printf("hello?\n");
    res = store->GetValue(PKEY_Music_AlbumArtist, &variant);
    printf("hello?\n");
    if(res == S_OK || res == INPLACE_S_TRUNCATED)
    {
        printf("success!: %S\n", variant.pcVal);
    }

    printf("hum: %ld\n", res);

    

    // set it to something else
    InitPropVariantFromString(L"ZZ Top", &variant);
    store->SetValue(PKEY_Music_AlbumArtist, variant);
    store->Commit();

    // very important undocumented method
    store->Release();

    return 0;
}
 */