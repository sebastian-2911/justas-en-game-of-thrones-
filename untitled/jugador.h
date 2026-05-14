#ifndef JUGADOR_H
#define JUGADOR_H

#include "Entidad.h"
#include <QPainter>

class Jugador : public Entidad {
public:
    // ------------------ CONSTRUCTOR ------------------
    Jugador();

    // ------------------ LÓGICA ------------------
    void actualizar();

    // ------------------ RENDER ------------------
    void renderizar(QPainter& painter);

    // ------------------ INPUT ------------------
    void procesarInput(char tecla);

    // ------------------ VIDA ------------------
    int getVida() const;
    void recibirDanio(int d);

    // ------------------ CARRIL ------------------
    int getCarril() const;

private:
    int vida;
    int carril;

    float tiempoInvulnerable;

    void moverIzquierda();
    void moverDerecha();

    float carriles[3] = {-0.6f, 2.2f, 4.5f};
};

#endif
