#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "entidad.h"

#include <QPainter>
#include <QRect>

class Obstaculo : public Entidad
{
public:
    Obstaculo(float x, float yReal);

    void actualizar();
    void renderizar(QPainter& painter) override;
    void renderizar(QPainter& painter, float scrollMundo);

    bool debeEliminarse() const;
    QRect getRect(float scrollMundo) const;
};

#endif
