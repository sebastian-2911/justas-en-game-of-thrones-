#include "nivel2.h"

#include <QDebug>

Nivel2::Nivel2()
{
    jugador = nullptr;

    fondoCargado = false;
    pisoCargado = false;

    velocidadJugador = 12.0f;

    limiteIzquierdo = 80.0f;
    limiteDerecho = 900.0f;

    finNivel = false;
}

Nivel2::~Nivel2()
{
    delete jugador;
}

void Nivel2::iniciar()
{
    delete jugador;

    jugador = new Jugador();

    jugador->usarMovimientoLibre(true);

    jugador->setPosicion(200.0f, 0.0f, 0.0f);

    fondoCargado = fondo.load("campo_batalla.png");

    pisoCargado = piso.load("piso.png");

    finNivel = false;
}

void Nivel2::actualizar()
{
    if (!jugador)
        return;

    jugador->actualizar();

    Vector3 pos = jugador->getPosicion();

    if (pos.x < limiteIzquierdo)
        pos.x = limiteIzquierdo;

    if (pos.x > limiteDerecho)
        pos.x = limiteDerecho;

    jugador->setPosicion(pos.x, pos.y, pos.z);
}

void Nivel2::renderizar(QPainter& painter)
{
    if (fondoCargado)
    {
        painter.drawPixmap(
            0,
            0,
            1024,
            768,
            fondo
            );
    }

    if (pisoCargado)
    {
        painter.drawPixmap(
            0,
            550,
            1024,
            220,
            piso
            );
    }

    if (jugador)
    {
        Vector3 pos = jugador->getPosicion();

        painter.setBrush(Qt::blue);

        painter.drawRect(
            pos.x,
            450,
            80,
            100
            );
    }
}

bool Nivel2::terminado()
{
    return finNivel;
}

Jugador* Nivel2::getJugador()
{
    return jugador;
}

void Nivel2::moverJugadorIzquierda()
{
    if (!jugador)
        return;

    Vector3 pos = jugador->getPosicion();

    pos.x -= velocidadJugador;

    jugador->setPosicion(
        pos.x,
        pos.y,
        pos.z
        );
}

void Nivel2::moverJugadorDerecha()
{
    if (!jugador)
        return;

    Vector3 pos = jugador->getPosicion();

    pos.x += velocidadJugador;

    jugador->setPosicion(
        pos.x,
        pos.y,
        pos.z
        );
}
void Nivel2::configurarMovimiento(
    float dt,
    float scroll
    )
{
}

float Nivel2::getVelocidadMundo() const
{
    return 0.0f;
}

void Nivel2::manejarClick(int x, int y)
{
}

bool Nivel2::pidioReinicio() const
{
    return false;
}

bool Nivel2::pidioSiguienteNivel() const
{
    return false;
}