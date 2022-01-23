#include <iostream>
#include <string>
#include <istream>
#include <vector>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits>
#include <string.h>
#include <termios.h>
#include "trie.h"
//! to implement auto complete for paths provided in input
//! implement backspace key 

void ParseString(const std::string &inputString, std::vector<std::string> &tokens)
{
    int n = inputString.size();
    std::string currString;
    for (int i = 0; i < n; i += 1)
    {
        if (inputString[i] == ' ')
        {
            tokens.emplace_back(currString);
            currString = "";
            continue;
        }
        currString += inputString[i];
    }
    tokens.emplace_back(currString);
    return;
}

void ParsePath(const std::string &path, std::vector<std::string> &individualPaths)
{
    std::string currString;
    int n = path.size();
    for (int i = 0; i < n; i += 1)
    {
        if (path[i] == ':')
        {
            individualPaths.emplace_back(currString);
            currString = "";
            continue;
        }
        currString += path[i];
    }
    individualPaths.emplace_back(currString);
    return;
}

bool RecursivelyCheckIfExecutableExist(const std::string &command, std::string path)
{
    // std::cout << path << std::endl;
    DIR *dir_point = opendir(path.c_str());
    dirent *entry = readdir(dir_point);
    while (entry)
    {
        if (entry->d_type == DT_DIR)
        {
            std::string currName = entry->d_name;
            if (currName != "." && currName != "..")
            {
                return RecursivelyCheckIfExecutableExist(command, path + '/' + currName);
            }
        }
        else if (entry->d_type == DT_REG)
        {
            if (entry->d_name == command)
                return true;
        }
        entry = readdir(dir_point);
    }
    return false;
}

int ForkAndExec(const std::vector<std::string> &tokens, const std::string &commandPath, bool parentRequired)
{
    pid_t pid = fork();
    int status = 0;
    //! child process
    //! fork and exec
    if (pid == 0)
    {
        char *argv[tokens.size() + 2];
        memset(argv, '\0', sizeof(argv));
        for (int i = 0; i < tokens.size(); i += 1)
        {
            argv[i] = const_cast<char *>(tokens[i].c_str());
        }
        const char *binaryString;
        const std::string tempString(commandPath + '/' + tokens[0]);
        binaryString = tempString.c_str();
        if (execv(binaryString, argv) < 0)
        {
            printf("ERROR: exec failed\n");
            exit(1);
        }
    }
    else
    {
        if(parentRequired)
        {
            int returnStatus;
            waitpid(pid, &returnStatus, 0);
            if (returnStatus == 0)
            {
                printf("completed\n");
            }
            else
            {
                printf("The child process terminated with an error!.\n");
            }
        }
    }
    return 0;
}

void AutoCompleteCall(const std::string &inputString, bool isCommand, Trie *Programs)
{
    if (isCommand)
    {
        std::vector<std::string> result = Programs->searchString(inputString);
        for (int i = 0; i < result.size(); i += 1)
        {
            std::cout << result[i] << " ";
        }
        std::cout << std::endl;
    }
    else
    {

    }
}

void printShellName()
{
    printf("bhaskar-shell > ");
}

bool getInputString(std::string &inputString, Trie *Programs)
{
    int c;
    static struct termios oldt, newt;
    //! Get the old terminal attributes
    tcgetattr(STDIN_FILENO, &oldt);
    //! set the current attribute as it is
    newt = oldt;
    /* !Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately */
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    bool isCommand = true;
    bool requireParent = true;
    while ((c = getchar()) != '\n')
    {
        putc(c, stdout);
        if (c == '\t')
        {
            std::cout << std::endl;
            AutoCompleteCall(inputString, isCommand, Programs);
            printShellName();
            std::cout << inputString;
            continue;
        }
        else if (c == ' ')
        {
            isCommand = false;
        }
        if (c != '\t')
            inputString += c;
        if(c == '&')
        {
            requireParent = false;
        }
    }
    //! restore the old settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return requireParent;
}

void buildTriePrograms(std::string path, Trie *Programs)
{
    DIR *dir_point = opendir(path.c_str());
    dirent *entry = readdir(dir_point);
    while (entry)
    {
        if (entry->d_type == DT_DIR)
        {
            std::string currName = entry->d_name;
            if (currName != "." && currName != "..")
            {
                buildTriePrograms(path + '/' + currName, Programs);
            }
        }
        else if (entry->d_type == DT_REG)
        {
            const std::string currString(entry->d_name);
            Programs->insertString(currString);
        }
        entry = readdir(dir_point);
    }
}



void Shell()
{
    const std::string &enviroment = getenv("PATH");
    std::vector <std::string> paths;
    std::vector <std::string> history;
    ParsePath(enviroment, paths);
    Trie *Programs = new Trie('#');
    for (int i = 0; i < paths.size(); i += 1)
    {
        buildTriePrograms(paths[i], Programs);
    }
    while (1)
    {
        printShellName();
        std::string inputString;
        bool parentRequired = getInputString(inputString, Programs);
        // std::getline(std::cin, inputString);
        if (inputString == "exit" || inputString == "q")
        {
            break;
        }
        else if(inputString == "history")
        {
            if(history.size() == 0)
            {
                std::cout << "No commands executed yet !!!\n";
            }
            else 
            {
                for(int i = 0; i < history.size(); i += 1)
                {
                    std::cout << history[i] << std::endl;
                }
            }
            history.emplace_back(inputString);
            continue;
        }
        history.emplace_back(inputString);
        // std::cout << "INPUT " << inputString << std::endl;
        std::vector<std::string> tokens;
        ParseString(inputString, tokens);
        int index = -1;
        for (int i = 0; i < paths.size(); i += 1)
        {
            // std::cout << paths[i] << std::endl;
            if (RecursivelyCheckIfExecutableExist(tokens[0], paths[i]))
            {
                index = i;
                // std::cout << paths[i] << std::endl;
                // std::cout << "FOUND" << std::endl;
                break;
            }
        }
        if (index == -1)
        {
            char tmp[256];
            getcwd(tmp, 256);
            std::string currPath(tmp);
            if (RecursivelyCheckIfExecutableExist(tokens[0], currPath))
            {
            }
            else
            {
                std::cout << "Enter the path where executable exist!!!!\n";
                std::string newPath;
                std::cin >> newPath;
            }
        }
        else
        {
            ForkAndExec(tokens, paths[index], parentRequired);
        }
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main()
{
    Shell();
    return 0;
}