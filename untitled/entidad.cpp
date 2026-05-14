#include "Entidad.h"

Entidad::Entidad() {
    posicion = {0.0f, 0.0f, 0.0f};
    hitbox = {1.0f, 1.0f};
    activa = true;
}

Vector3 Entidad::getPosicion() const {
    return posicion;
}

void Entidad::setPosicion(float x, float y, float z) {
    posicion = {x, y, z};
}

Hitbox Entidad::getHitbox() const {
    return hitbox;
}

bool Entidad::estaActiva() const {
    return activa;
}

void Entidad::desactivar() {
    activa = false;
}
