#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "Entidad.h"
#include <QPainter>
#include <QRect>

class Obstaculo : public Entidad {
public:
    // ------------------ CONSTRUCTOR ------------------
    Obstaculo(float x, float y, float z);

    // ------------------ LÓGICA ------------------
    void actualizar();

    // ------------------ RENDER ------------------
    void renderizar(QPainter& painter);

    // ------------------ ESTADO ------------------
    bool debeEliminarse() const;

    // ------------------ COLISIÓN (pantalla) ------------------
    QRect getRect() const;

    //  necesario para colisión por cruce
    float zAnterior;

private:
    float velocidad;
};

#endif // OBSTACULO_H
