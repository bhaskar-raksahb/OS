#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>        
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <vector>
void PrintChoices()
{
    printf("0: Open Another directory\n");
    printf("1: Copy File\n");
    printf("2: Paste File\n");
    printf("3: Exit\n");
}
int ScanChoice()
{
    static int choice;
    scanf("%d", &choice);
    return choice;
}
std::string GeneratePathString(const std::vector <std::string> &Directories)
{
    std::string retVal = "/";
    int n = Directories.size();
    for(int i = 0; i < n; i += 1)
    {
        retVal += Directories[i];
        retVal += "/";
    }
    return retVal;
}
int ForkAndExec(const std::string &CopiedFilePath, const std::string PastePath)
{
    pid_t pid = fork();
    pid_t wpid;
    int status = 0;
    if(pid == 0)
    {
        char *args[5];
        char command[] = "/bin/cp";
        char Recursive[] = "-r"; 
        args[0] = command;
        args[1] = Recursive;
        args[2] = const_cast<char *> (CopiedFilePath.c_str());
        args[3] = const_cast<char *> (PastePath.c_str());
        args[4] = NULL;
        if(execvp(command, args) < 0)
        {
            for(int i = 0; i < 4; i += 1)
            printf("%s\n", args[i]);
            printf("ERROR: exec failed\n");
            exit(1);
        }
    }
    else 
    {
        while ((wpid = wait(&status)) > 0);
    }
    return 0;
}
int main()
{
    DIR *d;
    struct dirent *dir;
    int choice, fileChoice;
    std::vector <std::string> currentDirectory;
    std::string CopiedFileLocation = "";
    while(1)
    {
        d = opendir(GeneratePathString(currentDirectory).c_str());
        if(!d)
        {
            printf("Selected file is not a directory. Going Back");
        }
        std::vector <std::string> directories;
        while ((dir = readdir(d)) != NULL)
        {
            directories.emplace_back(dir -> d_name);
        }
        int n = directories.size();
        for(int i = 0; i < n; i += 1)
        {
            printf("%d %s\n", i + 1, directories[i].c_str());
        }
        PrintChoices();
        choice = ScanChoice();
        switch(choice)
        {
            case 0:
            {
                printf("Enter the directory Number\n");
                choice = ScanChoice();
                while(choice - 1 >= directories.size())
                {
                    printf("Invalid!!! Not a valid directory\n");
                    choice = ScanChoice();
                }
                if(directories[choice - 1] == ".")
                {
                    break;
                }
                if(directories[choice - 1] == "..")
                {
                    currentDirectory.erase(currentDirectory.end() - 1);
                }
                else 
                {
                    currentDirectory.emplace_back(directories[choice - 1]);
                }
                break;
            }
            case 1:
            {
                printf("Enter the File to Copy Number\n");
                choice = ScanChoice();
                while(choice - 1 >= directories.size())
                {
                    printf("Invalid!!! Not a valid directory\n");
                    choice = ScanChoice();
                }
                CopiedFileLocation = GeneratePathString(currentDirectory);
                CopiedFileLocation += directories[choice - 1];
                printf("File Successfully Copied!!!\n");
                break;
            }
            case 2:
            {
                if(CopiedFileLocation.size() > 0)
                {
                    int retVal = ForkAndExec(CopiedFileLocation, GeneratePathString(currentDirectory));
                    if(retVal != 0)
                    {
                        printf("ERROR Copying file\n");
                    }
                }
                else 
                {
                    printf("No File Copied. Invalid Paste Operation\n");
                }
                break;
            }
            case 3:
            {
                exit(0);
            }
        }
        closedir(d);
    }
    return 0;
}