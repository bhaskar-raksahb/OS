#pragma once 
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
class Trie
{
    private:
        char value;
        Trie *children[127];
        bool isEnd;
    public:
        Trie(char val)
        {
            value = val;
            for(int i = 0; i < 127; i += 1)
            this -> children[i] = NULL;
            isEnd = false;
        }

        //! insert string into the trie
        void insertString(const std::string &inputString);

        //! search a common prefix of the string and print the values
        std::vector <std::string> searchString(const std::string &searchString);

        //! depth first search
        void dfs(Trie *root, std::vector <std::string> &retVal, std::string conCatString);
};