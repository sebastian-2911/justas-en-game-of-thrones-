#include "entidad.h"

Entidad::Entidad()
{
    posicion = {0.0f, 0.0f, 0.0f};
    xReal = 0.0f;
    yReal = 0.0f;
    hitbox = {1.0f, 1.0f};
    activa = true;
}

Vector3 Entidad::getPosicion() const
{
    return posicion;
}

void Entidad::setPosicion(float x, float y, float z)
{
    posicion = {x, y, z};
}

float Entidad::getXReal() const
{
    return xReal;
}

float Entidad::getYReal() const
{
    return yReal;
}

void Entidad::setPosicionReal(float x, float y)
{
    xReal = x;
    yReal = y;
}

Hitbox Entidad::getHitbox() const
{
    return hitbox;
}

bool Entidad::estaActiva() const
{
    return activa;
}

void Entidad::desactivar()
{
    activa = false;
}
