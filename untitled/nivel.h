#ifndef NIVEL_H
#define NIVEL_H

#include <QPainter>

// Forward declaration
class Jugador;

class Nivel {
public:
    Nivel();
    virtual ~Nivel();

    // ciclo del nivel
    virtual void iniciar() = 0;
    virtual void actualizar() = 0;
    virtual void renderizar(QPainter& painter) = 0;

    // estado
    virtual bool terminado() = 0;

    // acceso a jugador
    virtual Jugador* getJugador() = 0;
};

#endif
