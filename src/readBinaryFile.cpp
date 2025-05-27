#include <filesystem>
#include <fstream>
/*
CREATE A FUNCTION THAT READS BINARY FILES AND TURN THEM INTO VECTORS
*/

// https://www.geeksforgeeks.org/fread-function-in-c/
void readBinaryFiles(std::filesystem::path dataPath) {
    for (std::filesystem::directory_iterator dirIt(dataPath); dirIt != std::filesystem::directory_iterator(); ++dirIt) {
        std::filesystem::directory_entry entry = *dirIt;
        std::ifstream in(entry.path());

    }
}