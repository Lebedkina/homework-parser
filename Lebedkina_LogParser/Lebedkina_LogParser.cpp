#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>
#include <cassert>

namespace fs = std::filesystem;

const auto pattern_LogFileName = R"(\w+\.log)"; // Регулярка для поиска только логов
//const auto pattern_LogFileName = R"(\w+(\.log|\.txt))"; // Регулярка для поиска логов и txt файлов
//const auto pattern_LogFileName = R"(\w+\.log(.txt)"; // Регулярка для проверки на ошибки и ловлю исключений

void descriptionRegexError(const std::regex_error& e);

char is_LogFile_RE(std::string FileName, std::string_view rg)
{
    char result;
    try
    {
        std::regex r(rg.data());
        if (regex_match(FileName, r))
            result = 'y';
        else
            result = 'n';
        return result;
    }
    catch (const std::regex_error& e)
    {
        descriptionRegexError(e);
        return result = 'e';
    }
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
        std::cout << "It seems that your input:\"" << path << "\" does not look like a directory or file path, please try again\n";
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
                char isLogFile = is_LogFile_RE(CurrentPath.filename().string(), pattern_LogFileName); //Проверяем что файл имеет определенный тип по регулярному выражению (pattern_LogFileName)
                if (isLogFile == 'y')
                {
                    std::cout << CurrentPath.string() << " is a LOG file\n";

                    openThisFile(CurrentPath.string());
                }
                else if (isLogFile == 'e')
                {
                    exit(EXIT_FAILURE);
                }
                else
                {
                    std::cout << CurrentPath.string() << " is just a file\n";
                }
            }
        }
    }
    else
    {
        openThisFile(path);
    }
    return 0;
}

void descriptionRegexError(const std::regex_error& e)
{
    std::cerr << "std::regex`s error: " << e.what() << std::endl;

    if (e.code() == std::regex_constants::error_collate)
        std::cerr << "the expression contains an invalid collating element name\n";
    else if (e.code() == std::regex_constants::error_ctype)
        std::cerr << "the expression contains an invalid character class name\n";
    else if (e.code() == std::regex_constants::error_escape)
        std::cerr << "the expression contains an invalid escaped character or a trailing escape\n";
    else if (e.code() == std::regex_constants::error_backref)
        std::cerr << "the expression contains an invalid back reference\n";
    else if (e.code() == std::regex_constants::error_brack)
        std::cerr << "the expression contains mismatched square brackets ('[' and ']')\n";
    else if (e.code() == std::regex_constants::error_paren)
        std::cerr << "the expression contains mismatched parentheses ('(' and ')')\n";
    else if (e.code() == std::regex_constants::error_brace)
        std::cerr << "the expression contains mismatched curly braces ('{' and '}')\n";
    else if (e.code() == std::regex_constants::error_badbrace)
        std::cerr << "the expression contains an invalid range in a {} expression\n";
    else if (e.code() == std::regex_constants::error_space)
        std::cerr << "there was not enough memory to convert the expression into a finite state machine\n";
    else if (e.code() == std::regex_constants::error_badrepeat)
        std::cerr << "one of *?+{ was not preceded by a valid regular expression\n";
    else if (e.code() == std::regex_constants::error_complexity)
        std::cerr << "the complexity of an attempted match exceeded a predefined level\n";
    else if (e.code() == std::regex_constants::error_stack)
        std::cerr << "there was not enough memory to perform a match\n";

#ifdef _MSC_VER  //microsoft specific
    else if (e.code() == std::regex_constants::error_parse)
        std::cerr << "error_parse\n";
    else if (e.code() == std::regex_constants::error_syntax)
        std::cerr << "error_syntax\n";
#endif

    else
        std::cerr << "std::regex`s unkown error\n";
}
