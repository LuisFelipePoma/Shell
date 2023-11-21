#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#define ALPHABET_SIZE 26

// Estructura para un nodo en el árbol de búsqueda
class TrieNode
{
public:
    std::unordered_map<char, TrieNode *> children;
    bool isEndOfWord;

    TrieNode()
    {
        isEndOfWord = false;
    }
};

// Clase para el Trie
class Trie
{
private:
    TrieNode *root;
    int size;
    std::vector<std::string> words;

public:
    Trie()
    {
        root = new TrieNode();
    }

    TrieNode* getroot(){
        return root;
    }
    int getSize(){
        return size;
    }
    std::vector<std::string> getWords(){
        return words;
    }
    // Inserta una palabra en el Trie
    void insert(std::string word)
    {
        TrieNode *current = root;

        for (char c : word)
        {
            if (current->children.find(c) == current->children.end())
            {
                current->children[c] = new TrieNode();
            }

            current = current->children[c];
        }
        size = size + 1;
        current->isEndOfWord = true;
    }

    void printWords(TrieNode *node, std::string prefix)
    {
        if (node->isEndOfWord)
        {
            words.push_back(prefix);
        }

        for (const std::pair<const char, TrieNode *> &entry : node->children)
        {
            std::string newPrefix = prefix;
            newPrefix += entry.first;
            // In c it functions
            // char newPrefix[prefix.length() + 2];
            // sprintf(newPrefix, "%s%c", prefix.c_str(), entry.first);
            printWords(entry.second, newPrefix);
        }
    }

    // Buscar palabras que coincidan con un prefijo dado
    void autoComplete(std::string prefix)
    {
        TrieNode *current = root;

        // Buscar el nodo correspondiente al prefijo
        for (int i = 0; i < prefix.length(); i++)
        {
            char c = prefix[i];
            if (!current->children[c])
            {
                // El prefijo no existe, no hay sugerencias
                return;
            }
            current = current->children[c];
        }

        // Imprimir todas las palabras que comienzan con el prefijo
        printWords(current, prefix);
        
    }
};
