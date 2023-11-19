// export.h
#ifndef EXPORT_H
#define EXPORT_H
#include <iostream>

inline bool export_command(const char *name, const char *value)
{
    if (setenv(name, value, 1) != 0)
    {
        std::cout << "Error al establecer la variable de entorno";
        return false;
    }
/*     const char *nuevoValor = getenv(name);
    printf("Nuevo valor de %s: %s\n", name, nuevoValor); */

    return true;
}

// Obtener y mostrar el nuevo valor de la variable de entorno
// export a=b
#endif