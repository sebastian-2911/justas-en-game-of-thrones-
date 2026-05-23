#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QPainter>

struct Vector3
{
    float x;
    float y;
    float z;
};

struct Hitbox
{
    float ancho;
    float alto;
};

class Entidad
{
public:
    Entidad();
    virtual ~Entidad() {}

    virtual void actualizar() = 0;
    virtual void renderizar(QPainter& painter) = 0;

    Vector3 getPosicion() const;
    void setPosicion(float x, float y, float z);
    float getXReal() const;
    float getYReal() const;
    void setPosicionReal(float x, float y);

    Hitbox getHitbox() const;

    bool estaActiva() const;
    void desactivar();

protected:
    Vector3 posicion;
    float xReal;
    float yReal;
    Hitbox hitbox;
    bool activa;
};

#endif
