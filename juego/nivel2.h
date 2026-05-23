#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "jugador.h"

#include <QPainter>
#include <QPixmap>

class Nivel2 : public Nivel
{
public:

    Nivel2();
    ~Nivel2() override;

    void iniciar() override;

    void actualizar() override;

    void renderizar(QPainter& painter) override;

    bool terminado() override;

    Jugador* getJugador() override;

    void moverJugadorIzquierda();

    void moverJugadorDerecha();

private:

    Jugador* jugador;

    QPixmap fondo;
    QPixmap piso;

    bool fondoCargado;
    bool pisoCargado;

    float velocidadJugador;

    float limiteIzquierdo;
    float limiteDerecho;
    void configurarMovimiento(
        float dt,
        float scroll
        ) override;

    float getVelocidadMundo() const override;

    void manejarClick(int x, int y) override;

    bool pidioReinicio() const override;

    bool pidioSiguienteNivel() const override;

    bool finNivel;
};

#endif