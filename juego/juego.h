#ifndef JUEGO_H
#define JUEGO_H

#include "nivel.h"

class Game
{
public:
    Game();
    ~Game();

    void iniciar();
    void actualizar();

    void cambiarNivel(Nivel* nuevoNivel);

    bool estaEjecutando() const;

private:
    Nivel* nivelActual;
    bool ejecutando;
};

#endif
