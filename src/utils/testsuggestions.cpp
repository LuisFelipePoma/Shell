#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#define ALPHABET_SIZE 26

// Estructura para un nodo en el árbol de búsqueda
struct TrieNode
{
    struct TrieNode *children[ALPHABET_SIZE];
    int isEndOfWord;
};

// Inicializar un nuevo nodo del árbol de búsqueda
struct TrieNode *createNode()
{
    struct TrieNode *node = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    if (node)
    {
        node->isEndOfWord = 0;
        for (int i = 0; i < ALPHABET_SIZE; i++)
        {
            node->children[i] = NULL;
        }
    }
    return node;
}

// Insertar una palabra en el árbol de búsqueda
void insertWord(struct TrieNode *root, const char *word)
{
    struct TrieNode *current = root;
    for (int i = 0; i < strlen(word); i++)
    {
        int index = word[i] - 'a';
        if (!current->children[index])
        {
            current->children[index] = createNode();
        }
        current = current->children[index];
    }
    current->isEndOfWord = 1;
}

// Función auxiliar para imprimir palabras a partir de un nodo dado
void printWords(struct TrieNode *node, const char *prefix)
{
    if (node->isEndOfWord)
    {
        printf("%s\n", prefix);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (node->children[i])
        {
            char newPrefix[strlen(prefix) + 2];
            sprintf(newPrefix, "%s%c", prefix, 'a' + i);
            printWords(node->children[i], newPrefix);
        }
    }
}

// Buscar palabras que coincidan con un prefijo dado
void autoComplete(struct TrieNode *root, const char *prefix)
{
    struct TrieNode *current = root;

    // Buscar el nodo correspondiente al prefijo
    for (int i = 0; i < strlen(prefix); i++)
    {
        int index = prefix[i] - 'a';
        if (!current->children[index])
        {
            // El prefijo no existe, no hay sugerencias
            return;
        }
        current = current->children[index];
    }

    // Imprimir todas las palabras que comienzan con el prefijo
    printWords(current, prefix);
}

// Función principal
int main()
{
    while (true)
    {
        char c = std::cin.get();

        // Verificar si el carácter es un Tab (código ASCII 9)
        if (static_cast<int>(c) == 9)
        {
            std::cout<<"YA GYM\n";
            struct TrieNode *root = createNode();

            // Insertar palabras de ejemplo
            insertWord(root, "apple");
            insertWord(root, "app");
            insertWord(root, "application");
            insertWord(root, "banana");
            insertWord(root, "bat");

            // Autocompletar con un prefijo
            autoComplete(root, "app");
        }

        // Imprimir el carácter si no es un Tab
        std::cout << "Caracter ingresado: " << c << std::endl;
    }
    
    return 0;
}
