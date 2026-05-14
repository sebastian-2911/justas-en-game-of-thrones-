#ifndef GAME_H
#define GAME_H

#include "Nivel.h"

class Game {
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
