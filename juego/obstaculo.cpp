#include "obstaculo.h"

Obstaculo::Obstaculo(float x, float posicionYReal)
{
    posicion = {x, 0.0f, 0.0f};
    xReal = 512 + x * 180 - 25;
    yReal = posicionYReal;
    hitbox = {50.0f, 50.0f};
}

void Obstaculo::actualizar()
{
}

void Obstaculo::renderizar(QPainter& painter)
{
    renderizar(painter, 0.0f);
}

void Obstaculo::renderizar(QPainter& painter, float scrollMundo)
{
    int screenX = static_cast<int>(xReal);
    int screenY = static_cast<int>(yReal + scrollMundo);

    painter.setBrush(Qt::red);
    painter.drawRect(screenX, screenY, 50, 50);
}

QRect Obstaculo::getRect(float scrollMundo) const
{
    int screenX = static_cast<int>(xReal);
    int screenY = static_cast<int>(yReal + scrollMundo);

    return QRect(screenX, screenY, 50, 50);
}

bool Obstaculo::debeEliminarse() const
{
    return !activa;
}