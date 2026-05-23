#ifndef JUGADOR_H
#define JUGADOR_H

#include "entidad.h"
#include <QPainter>
#include <QPixmap>

class Jugador : public Entidad
{
public:
    Jugador();
    void actualizar();
    void renderizar(QPainter& painter);
    void procesarInput(char tecla);
    int getCarril() const;
    void usarMovimientoLibre(bool estado);

private:
    int carril;
    float tiempoInvulnerable;
    void moverIzquierda();
    void moverDerecha();
    bool movimientoLibre;
    float carriles[3] = {-1.0f, 0.0f, 1.0f};

    // animacion
    QPixmap frames[4];
    int frameActual;
    float tiempoFrame;

    static constexpr float DURACION_FRAME = 1.0f / 16.0f;
    static constexpr int TOTAL_FRAMES = 4;
};

#endif
