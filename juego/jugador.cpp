#include "jugador.h"

Jugador::Jugador()
{
    carril = 1;
    tiempoInvulnerable = 0.0f;
    movimientoLibre = false;

    hitbox = {0.8f, 1.2f};
    posicion = {0.0f, 0.0f, 0.0f};
}

void Jugador::actualizar()
{
    if (!movimientoLibre)
        posicion.x = carriles[carril];

    if (tiempoInvulnerable > 0.0f)
        tiempoInvulnerable -= 0.016f;
}

void Jugador::renderizar(QPainter& painter)
{
    int screenX = 512 + posicion.x * 180 - 25;
    int screenY = 400;
    setPosicionReal(static_cast<float>(screenX), static_cast<float>(screenY));

    if (tiempoInvulnerable > 0.0f)
        painter.setBrush(Qt::yellow);
    else
        painter.setBrush(Qt::blue);

    painter.drawRect(screenX, screenY, 50, 50);
}

void Jugador::procesarInput(char tecla)
{
    if (tecla == 'a' || tecla == 'A')
        moverIzquierda();
    else if (tecla == 'd' || tecla == 'D')
        moverDerecha();
}

void Jugador::moverIzquierda()
{
    if (carril > 0)
        carril--;
}

void Jugador::moverDerecha()
{
    if (carril < 2)
        carril++;
}

int Jugador::getCarril() const
{
    return carril;
}
void Jugador::usarMovimientoLibre(bool estado)
{
    movimientoLibre = estado;
}