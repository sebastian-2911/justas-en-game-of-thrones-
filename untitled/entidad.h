#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QPainter>

struct Vector3 {
    float x, y, z;
};

struct Hitbox {
    float ancho;
    float alto;
};

class Entidad {
protected:
    Vector3 posicion;
    Hitbox hitbox;
    bool activa;

public:
    Entidad();
    virtual ~Entidad() {}

    virtual void actualizar() = 0;
    virtual void renderizar(QPainter& painter) = 0;

    Vector3 getPosicion() const;
    void setPosicion(float x, float y, float z);

    Hitbox getHitbox() const;

    bool estaActiva() const;
    void desactivar();
};

#endif
