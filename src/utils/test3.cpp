#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 100

char *knownCommands[] = {"ls", "cd", "echo", "pwd", "exit"};

// Función para buscar sugerencias
void suggestCommand(const char *input)
{
    printf("Sugerencias: ");
    for (int i = 0; i < sizeof(knownCommands) / sizeof(knownCommands[0]); i++)
    {
        if (strstr(knownCommands[i], input) == knownCommands[i])
        {
            printf("%s ", knownCommands[i]);
        }
    }
    printf("\n");
}

int main()
{
    char input[MAX_INPUT_SIZE];

    while (1)
    {
        printf("MiShell> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // Eliminar el carácter de nueva línea

        // Lógica de autosugerencias
        suggestCommand(input);

        // Lógica para ejecutar comandos (puedes implementar esto)

        // Si el usuario ingresa "exit", salimos del bucle
        if (strcmp(input, "exit") == 0)
        {
            break;
        }
    }

    return 0;
}
