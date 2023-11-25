#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <cstdlib> // for getenv

#include "grammar/libs/ShellExprLexer.h"
#include "grammar/libs/ShellExprParser.h"
#include "grammar/shellVisitor.h"
#include "antlr4-runtime.h"
#include "utils/suggestions.h"

#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define YELLOW_TEXT "\033[33m"
#define BLUE_TEXT "\033[34m"
#define MAGENTA_TEXT "\033[35m"
#define CYAN_TEXT "\033[36m"
#define WHITE_TEXT "\033[37m"
#define RESET_COLOR "\033[0m"

#define BOLD_TEXT "\033[1m"
#define RESET_BOLD_TEXT "\033[22m"

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

#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define YELLOW_TEXT "\033[33m"
#define BLUE_TEXT "\033[34m"
#define MAGENTA_TEXT "\033[35m"
#define CYAN_TEXT "\033[36m"
#define WHITE_TEXT "\033[37m"
#define RESET_COLOR "\033[0m"

#define BOLD_TEXT "\033[1m"
#define RESET_BOLD_TEXT "\033[22m"

int main(int argc, char **argv)
{
	std::string colors[] = {BLUE_TEXT, GREEN_TEXT, YELLOW_TEXT, RED_TEXT, MAGENTA_TEXT, WHITE_TEXT};
	int colorIndex = 0;

	shellVisitor visitor;
	const char *username = getenv("USER");

	if (username == nullptr)
		username = "user";

	std::string line = "";
	std::string lineCP;

	bool singleLine = true;
	std::string mulStr = "";
	std::cout << BOLD_TEXT;

	// Prompt del shell
	std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;

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
				std::string wordToReplace = words[0];
				lastWord = wordToReplace;
				std::string newWord = line.substr(0, lastSpacePos) + wordToReplace;
				line = newWord;
				if (singleLine)
				{
					std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
				}
				else
				{
					std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
				}
				std::cout << "" << line << std::flush;
			}
			else if (words.size() > 1)
			{
				for (int i = 0; i < words.size(); i++)
				{
					if (i == words.size() - 1)
						std::cout << words[i] << "\n";
					else
						std::cout << words[i] << "\t";
				}
				if (singleLine)
				{
					std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
				}
				else
				{
					std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
				}
				std::cout << "" << line << std::flush;
			}
			else
			{
				std::cout << "" << line << std::flush;
			}
		}
		// Verify if the character is a backspace (ASCII code 127)
		else if (static_cast<int>(c) == 127)
		{
			std::cout << "\033[2K\r";
			if (singleLine)
			{
				std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
			}
			else
			{
				std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
			}
			if (line.size() > 0)
			{
				line.pop_back();
				std::cout << "" << line << std::flush;
			}
		}
		else if (c == '\n')
		{

			lineCP = "";
			if (line == "exit")
				break;
			if (line.empty())
			{
				if (singleLine)
				{
					std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
				}
				else
				{
					std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
				}
				continue;
			}
			// Change color
			if (line == "!!")
			{
				colorIndex == std::size(colors) - 1 ? colorIndex = 0 : colorIndex++;
				if (singleLine)
				{
					std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
				}
				else
				{
					std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
				}
				line = "";
				continue;
			}
			restoreTerminal();
			if (singleLine)
			{
				if (line == "**")
				{
					std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
					singleLine = false;
					line = "";
					continue;
				}
				// std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
				lineCP = line;
			}
			else
			{
				if (line != "**")
				{
					std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
					mulStr += line + " ";
					line = "";
					continue;
				}
				else
				{
					if (mulStr == "")
						std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
					singleLine = true;
					lineCP = mulStr;
					mulStr = "";
				}
			}
			if (lineCP.empty() || lineCP == "\n")
			{
				line = "";
				continue;
			}
			// main.cpp
			antlr4::ANTLRInputStream input(lineCP);
			ShellExprLexer lexer(&input);
			antlr4::CommonTokenStream tokens(&lexer);
			ShellExprParser parser(&tokens);
			auto tree = parser.start();
			visitor.visitStart(tree);
			visitor.saveIR();
			if (singleLine)
			{
				std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
			}
			else
			{
				std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
			}
			line = "";
		}
		else
		{
			restoreTerminal();
			line += c;
		}
	}
	return 0;
}