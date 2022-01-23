#include "trie.h"
#include <iostream>
void Trie::insertString(const std::string &inputString) 
{
    int n = inputString.size();
    Trie *root = this;
    for(int i = 0; i < n; i += 1)
    {
        if(root -> children[inputString[i]] == NULL)
        {
            Trie *newTrie = new Trie(inputString[i]);
            root -> children[inputString[i]] = newTrie;
        }
        root = root -> children[inputString[i]];
    }
    root -> isEnd = true;
}
void Trie::dfs(Trie *root, std::vector <std::string> &retVal, std::string conCatString)
{
    if(root == NULL)
    return;
    if(root -> isEnd)
    {
        retVal.emplace_back(conCatString + root -> value);
        return;
    }
    
    for(int i = 0; i < 127; i += 1)
    {
        if(root -> children[i] != NULL)
        dfs(root -> children[i], retVal, conCatString + root -> value);
    }
    return;
}
std::vector <std::string> Trie::searchString(const std::string &searchString)
{
    std::vector <std::string> possibleStrings;
    int n = searchString.size();
    Trie *root = this;
    for(int i = 0; i < n; i += 1)
    {
        root = root -> children[searchString[i]];
    }
    if(root == NULL)
    {
        possibleStrings.emplace_back(searchString);
        return possibleStrings;
    }
    for(int i = 0; i < 127; i += 1)
    {
        if(root -> children[i] != NULL)
        dfs(root -> children[i], possibleStrings, searchString);
    }
    return possibleStrings;
}