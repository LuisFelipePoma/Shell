#include <iostream>
#include <termios.h>
#include <unistd.h>

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

int main()
{
    configureTerminal();

    char c;

    std::cout << "Presiona Tab para detectarlo. Presiona Enter para salir." << std::endl;

    while (true)
    {
        c = getchar();

        // Verificar si el carácter es un Tab (código ASCII 9)
        if (static_cast<int>(c) == 9)
        {
            std::cout << "\nTecla Tab detectada. Terminando el programa.\n";
            break;
        }
        
        // Imprimir el carácter si no es un Tab
        std::cout << "Caracter ingresado: " << c << std::endl;
    }

    restoreTerminal();

    return 0;
}
