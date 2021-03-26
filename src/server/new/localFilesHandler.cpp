#include "platform.h"

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

#if defined(WINDOWS)
#include <locale>
#include <codecvt>
#include <fcntl.h>
#endif

#if defined(WINDOWS)
std::wstring stringToWstring(const std::string& t_str)
{
    //setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}
#endif

#if defined(WINDOWS)
std::vector<std::vector<std::wstring>> getFilesInDirectory(std::wstring path)
{
    std::vector<std::vector<std::wstring>> filenames;
    int i = 0;

    for(const auto & entry : std::filesystem::directory_iterator(path))
    {
        //std::cout << entry.path() << ", directory: " << entry.is_directory() << std::endl;
        if(entry.is_directory()) {
            std::vector<std::vector<std::wstring>> filenamesTemp = getFilesInDirectory(entry.path().wstring());
            filenames.insert(filenames.end(), filenamesTemp.begin(), filenamesTemp.end());
        } else {
            if((entry.path().extension() == ".mp3") ||
            (entry.path().extension() == ".ogg") ||
            (entry.path().extension() == ".flac")) {

                std::vector<std::wstring> vectTemp;
                vectTemp.push_back(entry.path().wstring());
                std::wstring filenameTemp = entry.path().filename().wstring();
                size_t lastindexTemp = filenameTemp.find_last_of('.');
                vectTemp.push_back(filenameTemp.substr(0, lastindexTemp));
                vectTemp.push_back(entry.path().extension().wstring());

                filenames.push_back(vectTemp);
            }
        }
        i += 1;
    }
    return filenames;
}
#elif defined(POSIX)
std::vector<std::vector<std::string>> getFilesInDirectory(std::string path)
{
    std::vector<std::vector<std::string>> filenames;
    int i = 0;

    for(const auto & entry : std::filesystem::directory_iterator(path))
    {
        //std::cout << entry.path() << ", directory: " << entry.is_directory() << std::endl;
        if(entry.is_directory()) {
            std::vector<std::vector<std::string>> filenamesTemp = getFilesInDirectory(entry.path().string());
            filenames.insert(filenames.end(), filenamesTemp.begin(), filenamesTemp.end());
        } else {
            if((entry.path().extension() == ".mp3") ||
            (entry.path().extension() == ".ogg") ||
            (entry.path().extension() == ".flac")) {

                std::vector<std::string> vectTemp;
                vectTemp.push_back(entry.path().string());
                std::string filenameTemp = entry.path().filename().string();
                size_t lastindexTemp = filenameTemp.find_last_of('.');
                vectTemp.push_back(filenameTemp.substr(0, lastindexTemp));
                vectTemp.push_back(entry.path().extension().string());

                filenames.push_back(vectTemp);
            }
        }
        i += 1;
    }
    return filenames;
}
#endif

/* compile on Linux (-lstdc++fs not needed if GCC > 9): 
 * g++ -std=c++17 localFilesHandler.cpp -o test -lstdc++fs
 * 
 * compile on Windows:
 * g++ -std=c++17 .\localFilesHandler.cpp -o test
 */
int main()
{
    #if defined(WINDOWS)
    std::wofstream f;
    f.open("temp.txt", std::ios::trunc);
    // Here the magic happens
    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
    f.imbue(utf8_locale);
    // for terminal output, but very slooooow so no.
    //_setmode(_fileno(stdout), _O_U16TEXT);
    #elif defined(POSIX)
    std::ofstream f;
    f.open("temp.txt", std::ios::trunc);
    #endif

    #if defined(WINDOWS)
    std::vector<std::vector<std::wstring>> filenames = getFilesInDirectory(stringToWstring("D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/"));
    #elif defined(POSIX)
    std::vector<std::vector<std::string>> filenames = getFilesInDirectory("../tests/musics/");
    #endif

    for(int i = 0; i < filenames.size(); i++)
    {
        #if defined(WINDOWS)
        wchar_t  *wstring = _wcsdup(filenames[i][0].c_str());
        f.write(filenames[i][0].c_str(), filenames[i][0].length());
        f.write(L"\n", 1);
        #elif defined(POSIX)
        f.write(filenames[i][0].c_str(), filenames[i][0].length());
        f.write("\n", 1);
        #endif

    }
    f.close();
    
    return 0;
}
