#include <filesystem>
#include <locale>
#include <stdio.h>
#include <codecvt>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <fcntl.h>

std::wstring stringToWstring(const std::string& t_str)
{
    //setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}

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

int main()
{
    std::wofstream f;
    f.open("temp.txt", std::ios::trunc);
    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
    //_setmode(_fileno(stdout), _O_U16TEXT);
    f.imbue(utf8_locale);
    
    std::vector<std::vector<std::wstring>> filenames = getFilesInDirectory(stringToWstring("D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/"));
    for(int i = 0; i < filenames.size(); i++)
    {
        wchar_t  *wstring = _wcsdup(filenames[i][0].c_str());

        //char resp[255];
        //wcstombs(resp, filenames[i][0].c_str(), 255);
        f.write(filenames[i][0].c_str(), filenames[i][0].length());
        f.write(L"\n", 1);
        wprintf(L"Strings in field (2): %ls\n", wstring);
    }
    f.close();
    
    return 0;
}
