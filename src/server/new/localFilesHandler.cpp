#include "localFilesHandler.h"


std::vector<std::vector<std::string>> LocalFilesHandler::getFilesInDirectory(std::string path)
{
    std::vector<std::vector<std::string>> filenames;
    int i = 0;

    for(const auto & entry : std::filesystem::recursive_directory_iterator(path))
    {
        //std::cout << entry.path() << ", directory: " << entry.is_directory() << std::endl;
        if(entry.is_directory()) {
            // nothing to do
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

/* compile on Linux (-lstdc++fs not needed if GCC > 9): 
 * g++ -std=c++17 localFilesHandler.cpp -o test -lstdc++fs
 * 
 * compile on Windows:
 * g++ -std=c++17 .\localFilesHandler.cpp -o test
 */
/* int main()
{
    #if defined(WINDOWS)
    std::fstream f;
    f.open("temp.txt", std::ios::trunc);
    // Here the magic happens
    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<char>());
    f.imbue(utf8_locale);
    // for terminal output, but very slooooow so no.
    //_setmode(_fileno(stdout), _O_U16TEXT);
    #elif defined(POSIX)
    std::ofstream f;
    f.open("temp.txt", std::ios::trunc);
    #endif

    std::vector<std::vector<std::string>> filenames = getFilesInDirectory("D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/");

    for(int i = 0; i < filenames.size(); i++)
    {
        #if defined(WINDOWS)
        char *string = strstr(filenames[i][0].c_str(), "550");
        f.write(filenames[i][0].c_str(), filenames[i][0].length());
        f.write("\n", 1);
        #elif defined(POSIX)
        f.write(filenames[i][0].c_str(), filenames[i][0].length());
        f.write("\n", 1);
        #endif

    }
    f.close();
    
    return 0;
} */
