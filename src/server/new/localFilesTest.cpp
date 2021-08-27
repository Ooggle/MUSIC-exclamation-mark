#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string.h>

#include "localFilesHandler.h"

int main() {
    std::ofstream outfile("test.txt", std::ofstream::out);
    std::string path = "D:/Ooggle/DL Files/_Pers/CODE/c++/MUSIC-exclamation-mark/src/server/tests/musics/";
    
    LocalFilesHandler* filesHandler = new LocalFilesHandler;
    std::vector<std::vector<std::string>> filenames = filesHandler->getFilesInDirectory(path);

    for(int i = 0; i < filenames.size(); i++)
    {
        char *string = strstr(filenames[i][0].c_str(), "550");
        outfile.write(filenames[i][0].c_str(), filenames[i][0].length());
        outfile.write("\n", 1);
    }

    outfile.close();
}
