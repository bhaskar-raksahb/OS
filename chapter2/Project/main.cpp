#include "trie.h"
#include <string>
#include <iostream>
void printVector(const std::vector <std::string> &result)
{
    for(int i = 0; i < result.size(); i += 1)
    std::cout << result[i] << std::endl;
}
int main()
{
    Trie *root = new Trie('#');
    root -> insertString("Bhaskar");
    std::vector <std::string> result = root -> searchString("B");
    printVector(result);
    return 0;
}