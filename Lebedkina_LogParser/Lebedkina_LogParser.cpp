#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

//--Объявление вводимых из файла значений
std::string pattern_LogFileName = ""; // Регулярка для поиска только файлов определенного типа
std::string pattern_StringFilter = ""; // Регулярка для поиска определенных строк
std::string path = "";
//--

std::string inputArgs_FName = "input_args.txt";
std::string outResult_FName = "ParserResult.txt";

void descriptionRegexError(const std::regex_error& e, const std::string& with);

bool enterArgs(std::string& LogFName_regex, std::string& StrFilter_regex, std::string& pathToLogs)
{
    std::cout << "Reading the file "<< inputArgs_FName <<" to get initial arguments\n";
    std::ifstream inputArgsFile(inputArgs_FName);
    if (inputArgsFile.is_open())
    {
        short int counter = 0;
        std::string lineFrFile = "";
        while (getline(inputArgsFile, lineFrFile))
        {
            counter++;
            if (counter == 1)
                LogFName_regex = std::quoted(lineFrFile)._Str;
            else if (counter == 2)
                StrFilter_regex = std::quoted(lineFrFile)._Str;
            else if (counter == 3) {
                pathToLogs = std::quoted(lineFrFile)._Str;
                break;
            }
        }
        std::cout << "The file "<< inputArgs_FName <<" was successfully read\n";
        inputArgsFile.close();
        return true;
    }
    else 
    {
        std::cout << "Failed to open the file: "<< inputArgs_FName << std::endl;
        return false;
    }
}

bool is_MatchesRegex(const std::string& varRegex, const std::string& inpString)
{
    try
    {
        std::regex r(varRegex);
        if (regex_match(inpString, r))
            return true;
        else
            return false;
    }
    catch (const std::regex_error& e)
    {
        descriptionRegexError(e, varRegex);
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
        std::ofstream outFile(outResult_FName, std::ios::app);
        outFile << "All finded lines in file: (" << path << "):\n";
        std::list<std::string> SearchedLines;
        std::string textFromFile;
        std::cout << "Start searching for strings\n";
        while (getline(inputFile, textFromFile))
        {
            if (is_MatchesRegex(pattern_StringFilter, textFromFile))
            {
                countOfFindedLines++;
                std::cout << "Finded: " << countOfFindedLines << " lines\n";
                outFile << " " << countOfFindedLines << " finded line: " << textFromFile << std::endl;
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
    if (enterArgs(pattern_LogFileName, pattern_StringFilter, path))
    {
        std::cout << "\nEntered arguments:\n";
        std::cout << "Regex to search by file type: " << pattern_LogFileName << std::endl;
        std::cout << "Regex to search for the required lines: " << pattern_StringFilter << std::endl;
        std::cout << "Path to file: " << path << std::endl << std::endl;

        std::ofstream outFile(outResult_FName); // Нужно чтобы в файле удалился прошлый результат и новые даннные данные записывались с начала
        outFile.close();

        if (fs::is_directory(fs::status(path))) // Проверяем что в конце ссылки действителньо существующая директория
        {
            std::cout << "The entered path is the path to the directory\n";
            for (fs::directory_iterator PathWithFileName = fs::directory_iterator(path); PathWithFileName != fs::directory_iterator(); ++PathWithFileName) // Проходим по директории в поиске файлов
            {
                fs::path CurrentPath = PathWithFileName->path();
                if (fs::is_regular_file(fs::status(CurrentPath))) // Проверяем, что итератор является именно файлом
                {
                    if (is_MatchesRegex(pattern_LogFileName, CurrentPath.filename().string())) //Проверяем что файл имеет определенный тип по регулярному выражению (pattern_LogFileName)
                    {
                        std::cout << std::endl << CurrentPath.string() << " is a LOG file\n";
                        openFileAndSearch(CurrentPath.string());
                        std::cout << "The search in the file (" << path << ") is over\n";
                    }
                    else
                        std::cout << CurrentPath.string() << " is just a file\n";
                }
            }
            std::cout << "The search in the directory is over\n";
        }
        else
        {
            openFileAndSearch(path);
            std::cout << "The search is over\n";
        }
    }
    return 0;
}

void descriptionRegexError(const std::regex_error& e, const std::string& with)
{
    std::cerr << "std::regex`s error with \"" << with << "\": " << e.what() << std::endl;

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
    exit(EXIT_FAILURE);
}
