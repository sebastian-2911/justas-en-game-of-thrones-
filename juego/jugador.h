#ifndef JUGADOR_H
#define JUGADOR_H

#include "entidad.h"

#include <QPainter>

class Jugador : public Entidad
{
public:
    Jugador();

    void actualizar();
    void renderizar(QPainter& painter);

    void procesarInput(char tecla);

    int getCarril() const;
    void usarMovimientoLibre(bool estado);
private:
    // estado
    int carril;

    float tiempoInvulnerable;

    // movimiento
    void moverIzquierda();
    void moverDerecha();
    bool movimientoLibre;

    float carriles[3] = {-1.0f, 0.0f, 1.0f};
};

#endif
