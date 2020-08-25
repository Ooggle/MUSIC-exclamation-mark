#include <fstream>
#include <filesystem>

int main() {
    // file loading
    std::ifstream myFile;
    myFile.open("musics/sample.mp3", std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    if(!myFile.is_open())
        return EXIT_FAILURE;

    std::filesystem::path p{"musics/sample.mp3"};
    printf("file size : %d\n", std::filesystem::file_size(p));

    char car;
    for(int i = 0; i < 50; i++)
    {
        if(!myFile.read(&car, 1))
            return EXIT_SUCCESS;
        printf("%02X ", car);
    }
    

    return EXIT_SUCCESS;
}
