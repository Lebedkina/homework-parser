#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>
#include <vector>
namespace fs = std::filesystem;

const auto pattern_LogFileName = R"(\w+\.log)"; // Регулярка для поиска только логов
//const auto pattern_LogFileName = R"(\w+(\.log|\.txt))"; // Регулярка для поиска логов и txt файлов

bool is_LogFile(std::string FileName, std::string_view rg)
{
    std::regex r(rg.data());
    return  regex_match(FileName, r);
}

void openThisFile(std::string path)
{
    std::ifstream inputFile(path);
    if (inputFile.is_open())
    {
        std::string textFromFile;
        while (getline(inputFile, textFromFile))
        {
            //std::cout << textFromFile << std::endl;
        }
        inputFile.close();
    }
    else
        std::cout << "Failed to open file: " << path << "\n";
}

int main()
{
    std::cout << "Enter the path to the directory with the log file or to the log file itself\n";
    std::cout << "Path: ";
    std::string path = "";
    std::getline(std::cin, path);

    if (fs::is_directory(fs::status(path))) // Проверяем что в конце ссылки действителньо существующая директория
    {
        std::cout << "It is a directory\n"; //----------------
        for (fs::directory_iterator PathWithFileName = fs::directory_iterator(path); PathWithFileName != fs::directory_iterator(); ++PathWithFileName) // Проходим по директории в поиске файлов
        {
            fs::path CurrentPath = PathWithFileName->path();
            if (fs::is_regular_file(fs::status(CurrentPath))) // Проверяем, что итератор является именно файлом 
            {
                if (is_LogFile(CurrentPath.filename().string(), pattern_LogFileName)) //Проверяем что файл имеет определенный тип по регулярному выражению (.log)
                {
                    std::cout << CurrentPath.string() << " is a regular LOG file\n";

                    openThisFile(CurrentPath.string());

                }
                else
                {
                    std::cout << CurrentPath.string() << " is a regular file\n";
                }
            }
        }
    }
    else
    {
        std::cout << "It is not a directory path\n";

        openThisFile(path);
    }
    return 0;
}
