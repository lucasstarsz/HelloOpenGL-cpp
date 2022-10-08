#include <string>
#include "FileUtils.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace LearnOpenGL::Utilities
{
    std::string loadFile(const std::string& fileLocation)
    {
        std::ifstream file(fileLocation);

        if (!file.is_open())
        {
            std::cerr << "Unable to find file at " << fileLocation << " to open.\n";
            return "FILE_NOT_FOUND";
        }

        std::cerr << "Found file at " << fileLocation << " to open.\n";

        try
        {
            std::stringstream fileContents;
            fileContents << file.rdbuf();

            file.close();

            return fileContents.str();
        }
        catch (std::ifstream::failure& exception)
        {
            std::cerr << "Error while reading file at " << fileLocation << ".\n" << exception.what() << '\n';
            return "CANNOT_READ_FILE";
        }
    }
}
