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
#include <vector>
#include <cstdlib> // for getenv


// Función para configurar el modo de entrada no canónico y desactivar el eco
void configureTerminal()
{
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag &= ~(ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Función para restaurar la configuración original de la terminal
void restoreTerminal()
{
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag |= (ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main(int argc, char **argv)
{
	shellVisitor visitor;
	const char *username = getenv("USER");
	if (username == nullptr)
		username = "user";

	std::string line = "";
	// Prompt del shell
	std::cout << "[" << username << "] $ ";
	while (true)
	{
		// Function to not need to press enter
		configureTerminal();

		// Read character from terminal
		char c = ' ';
		c = getchar();

		// Check if the character is a Tab (ASCII code 9)
		if (static_cast<int>(c) == 9)
		{
			// Clean the line
			std::cout << "\033[2K\r";

			std::vector<std::string> commandOutput;

			FILE *fp = popen("ls", "r");
			if (fp == nullptr)
				std::cerr << "Error al ejecutar el comando.\n";
			else
			{
				// Save the output of the command ls
				char buffer[128];
				while (fgets(buffer, sizeof(buffer), fp) != nullptr)
				{
					// Remove the \n from the buffer
					buffer[strcspn(buffer, "\n")] = '\0';
					commandOutput.push_back(buffer);
				}
				// Close the pipe
				pclose(fp);
			}
			// Create a trie
			Trie trie;
			TrieNode *a = trie.getroot();

			// Insert the output of the command ls into the trie
			for (const auto &line : commandOutput)
			{
				trie.insert(line);
			}
			int lastSpacePos = line.find_last_of(' ');
			if (lastSpacePos == std::string::npos)
				lastSpacePos = 0;
			else
				lastSpacePos = lastSpacePos + 1;

			std::string lastWord = line.substr(lastSpacePos, line.size() + 1);

			trie.autoComplete(lastWord);

			std::vector<std::string> words = trie.getWords();
			restoreTerminal();

			// Autocomplete the command if there is only one word
			if (words.size() == 1)
			{
				std::cout << "[" << username << "] $ ";
				std::string wordToReplace = words[0];
				lastWord = wordToReplace;
				std::string newWord = line.substr(0, lastSpacePos) + wordToReplace;
				line = newWord;
				std::cout << "" << line << std::flush;
			}
			else if (words.size() > 1)
			{
				for (int i = 0; i < words.size(); i++)
				{
					if (i == words.size() - 1)
					{
						std::cout << words[i] << "\n";
					}
					else
					{
						std::cout << words[i] << "\t";
					}
				}
				line = "";
				std::cout << "[" << username << "] $ ";
			}
		}
		else if (c != '\n')
		{
			restoreTerminal();
			line += c;
		}
		else
		{
			restoreTerminal();
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
			std::cout << "[" << username << "] $ ";
			line = "";
		}
	}
	return 0;
}