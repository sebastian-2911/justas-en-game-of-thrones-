#include "Obstaculo.h"

// ------------------ CONSTRUCTOR ------------------
Obstaculo::Obstaculo(float x, float y, float z) {
    posicion = {x, y, z};
    velocidad = 0.2f;

    hitbox = {1.5f, 1.5f};
}

// ------------------ ACTUALIZAR ------------------
void Obstaculo::actualizar() {
    zAnterior = posicion.z; //  SIEMPRE antes

    posicion.z -= velocidad;
}
// ------------------ RENDER ------------------
void Obstaculo::renderizar(QPainter& painter) {
    int screenX = 300 + posicion.x * 80;
    int screenY = 450 - posicion.z * 20;

    painter.setBrush(Qt::red);
    painter.drawRect(screenX, screenY, 50, 50);

    // DEBUG HITBOX (opcional)
    // painter.setPen(Qt::green);
    // painter.drawRect(getRect());
}

// ------------------ RECT ------------------
QRect Obstaculo::getRect() const {
    int screenX = 300 + posicion.x * 80;
    int screenY = 450 - posicion.z * 20;

    return QRect(screenX, screenY, 50, 50);
}

// ------------------ ESTADO ------------------
bool Obstaculo::debeEliminarse() const {
    return !activa;
}
