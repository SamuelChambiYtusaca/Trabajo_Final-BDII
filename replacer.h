#pragma  once
#include <cstdint>
#include <cstddef>
#include <iostream>

class Replacer
// La clase Replacer, funciona como una clase abstracta, de forma que todas sus implementaciones se encuentran delcaradas en clases derivadas
// Esto permite, que los direfentes algoritmos puedan ser implementados y utilizados por el buffer, de una forma mucho mas simple.
{
public:
    Replacer() = default;
    virtual ~Replacer() = default;
    virtual bool Victim(int32_t *frame_id) = 0; // Funcion encargada de encontrar el frame que debe ser remplazado.
    virtual void Pin(int32_t frame_id) = 0;     // Marca un frame del buffer como "En USO"
    virtual void Unpin(int32_t frame_id) = 0;   // Disminuye el pinnCount del frame, indicando que puede ser remplazado.
    virtual size_t Size() = 0;
};