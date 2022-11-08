#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

std::string pattern_LogFileName = R"(\w+\.log)"; // Регулярка для поиска только лог-файлов
//const auto pattern_LogFileName = R"(\w+(\.log|\.txt))"; // Регулярка для поиска логов и txt файлов
//const auto pattern_LogFileName = R"(\w+\.log(.txt)"; // Регулярка для проверки на ошибки и ловлю исключений

std::string pattern_StringFilter = R"(.*([Ee][Rr][Rr][Oo][Rr]).*)"; // Регулярка для поиска определенных строк (строк, содержащих слово error в независимости от регистра)

void descriptionRegexError(const std::regex_error& e, std::string with, std::string where);

char is_LogFile_RE(std::string FileName)
{
    char result;
    try
    {
        std::regex r(pattern_LogFileName);
        if (regex_match(FileName, r))
            result = 'y';
        else
            result = 'n';
        return result;
    }
    catch (const std::regex_error& e)
    {
        descriptionRegexError(e, pattern_LogFileName, "is_LogFile_RE()");
        return result = 'e';
    }
}

char is_NecessaryString_RE(std::string line)
{
    char result;
    try
    {
        std::regex r(pattern_StringFilter);
        if (regex_match(line, r))
            result = 'y';
        else
            result = 'n';
        return result;
    }
    catch (const std::regex_error& e)
    {
        descriptionRegexError(e, pattern_StringFilter, "is_NecessaryString_RE()");
        return result = 'e';
    }
}
void openFileAndSearch(std::string path)
{
    std::cout << "Trying to open the file: " << path << std::endl;
    std::ifstream inputFile(path);
    int countOfFindedLines = 0;
    if (inputFile.is_open())
    {
        std::cout << "The file (" << path << ") was opened successfully\n";
        std::ofstream outFile("ParserResult.txt", std::ios::app);
        outFile << "All finded lines in file: (" << path << "):\n";
        std::list<std::string> SearchedLines;
        std::string textFromFile;
        std::cout << "Start searching for strings\n";
        while (getline(inputFile, textFromFile))
        {
            char isNesString = is_NecessaryString_RE(textFromFile);
            if (isNesString == 'y')
            {
                countOfFindedLines++;
                std::cout << "Finded: " << countOfFindedLines << " lines\n";
                outFile << " " << countOfFindedLines << " finded line: " << textFromFile << std::endl;
            }
            else if (isNesString == 'e')
            {
                exit(EXIT_FAILURE);
            }
        }
        outFile.close();
        inputFile.close();
    }
    else
        std::cout << "Failed to open the file: " << path << std::endl;
}

int main()
{
    std::cout << "Enter the path to the directory with the log file or to the log file itself\n";
    std::cout << "Path: ";
    std::string path = "";
    std::getline(std::cin, path);

    std::ofstream outFile("ParserResult.txt");
    outFile.close();
    if (fs::is_directory(fs::status(path))) // Проверяем что в конце ссылки действителньо существующая директория
    {
        std::cout << "It is a directory\n";
        for (fs::directory_iterator PathWithFileName = fs::directory_iterator(path); PathWithFileName != fs::directory_iterator(); ++PathWithFileName) // Проходим по директории в поиске файлов
        {
            fs::path CurrentPath = PathWithFileName->path();
            if (fs::is_regular_file(fs::status(CurrentPath))) // Проверяем, что итератор является именно файлом 
            {
                char isLogFile = is_LogFile_RE(CurrentPath.filename().string()); //Проверяем что файл имеет определенный тип по регулярному выражению (pattern_LogFileName)
                if (isLogFile == 'y')
                {
                    std::cout << CurrentPath.string() << " is a LOG file\n";
                    openFileAndSearch(CurrentPath.string());
                    std::cout << "The search in the file (" << path << ") is over\n";
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
        std::cout << "The search in the directory is over\n";
    }
    else
    {
        openFileAndSearch(path);
        std::cout << "The search is over\n";
    }
    return 0;
}

void descriptionRegexError(const std::regex_error& e, std::string with, std::string where)
{
    std::cerr << "std::regex`s error in (" << where << "), with \"" << with << "\": " << e.what() << std::endl;

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
