#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <termios.h>
#include <unistd.h>

#include "grammar/libs/ShellExprLexer.h"
#include "grammar/libs/ShellExprParser.h"
#include "grammar/shellVisitor.h"
#include "antlr4-runtime.h"
#include "utils/suggestions.h"

#include <cstdlib> // for getenv

// Función para configurar el modo de entrada no canónico y desactivar el eco
void configureTerminal()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Función para restaurar la configuración original de la terminal
void restoreTerminal()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main(int argc, char **argv)
{
    shellVisitor visitor;
    Trie trie;
    std::string line;
    std::string program = "";
    const char *username = getenv("USER"); // Get the username
    if (username == nullptr)
    { // If the USER environment variable is not set, use a default value
        username = "user";
    }
    while (true)
    {

        std::cout << "[" << username << "] $ "; // Print the prompt with the username
        // Print the prompt.
        configureTerminal();
        char c = getchar(); // Leer un solo carácter

        // Verificar si el carácter es un Tab (código ASCII 9)
        if (static_cast<int>(c) == 9)
        {
            std::cout << "\033[2K\r";
            TrieNode *a = trie.getroot();
            for (const std::pair<const char, TrieNode *> &entry : a->children)
            {
                trie.autoComplete(std::string(1, entry.first));
            }
            std::cout << "\n";
        }
        else
        {
            restoreTerminal();
            std::cout << "" << c << std::flush;

            // Devolver el carácter a la terminal
            ungetc(c, stdin);

            std::getline(std::cin, line);

            // std::cout << "" << std::flush;
            trie.insert(line);

            if (line == "exit")
            {
                break;
            }

            if (line.empty())
            {
                continue;
            }

            antlr4::ANTLRInputStream input(line);
            ShellExprLexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);
            ShellExprParser parser(&tokens);
            auto tree = parser.start();
            visitor.visitStart(tree);
            visitor.test();
        }
    }
}