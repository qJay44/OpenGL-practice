#include <fstream>
#include <iostream>
#include <string>

static const std::string readFromFile(const std::string& pathToFile) {
    std::string content;
    std::ifstream fileStream(pathToFile, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << pathToFile << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    /* std::cout << "'" << content << "'" << std::endl; */

    return content;
}

