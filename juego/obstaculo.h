#ifndef OBSTACULO_H
#define OBSTACULO_H
#include "entidad.h"
#include <QPainter>
#include <QRect>
#include <cmath>

class Obstaculo : public Entidad
{
public:
    // constructor obstáculo normal
    Obstaculo(float x, float yReal);

    // constructor flecha
    Obstaculo(float xPantalla, float yPantalla, float vx0, float vy0, bool esFlecha);

    void actualizar();
    void actualizar(float dt);

    void renderizar(QPainter& painter) override;
    void renderizar(QPainter& painter, float scrollMundo);

    bool  debeEliminarse() const;
    QRect getRect(float scrollMundo = 0.0f) const;

    bool getEsFlecha() const { return esFlecha_; }

private:
    bool  esFlecha_ = false;
    float vx_       = 0.0f;
    float vy_       = 0.0f;

    static constexpr float GRAVEDAD = 800.0f;
    static constexpr float SUELO_Y  = 560.0f;
};

#endif
