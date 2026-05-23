#include "jugador.h"

Jugador::Jugador()
{
    carril = 1;
    tiempoInvulnerable = 0.0f;
    movimientoLibre = false;
    hitbox = {0.8f, 1.2f};
    posicion = {0.0f, 0.0f, 0.0f};

    frameActual = 0;
    tiempoFrame = 0.0f;

    QPixmap sheet;
    sheet.load(":/robert nivel1.png");

    if (!sheet.isNull())
    {
        frames[0] = sheet.copy(0,   0, 115, 330);
        frames[1] = sheet.copy(115, 0, 127, 330);
        frames[2] = sheet.copy(242, 0, 129, 330);
        frames[3] = sheet.copy(371, 0, 132, 330);
    }
}

void Jugador::actualizar()
{
    if (!movimientoLibre)
        posicion.x = carriles[carril];

    if (tiempoInvulnerable > 0.0f)
        tiempoInvulnerable -= 0.016f;

    tiempoFrame += 0.016f;
    if (tiempoFrame >= DURACION_FRAME)
    {
        tiempoFrame -= DURACION_FRAME;
        frameActual = (frameActual + 1) % TOTAL_FRAMES;
    }
}

void Jugador::renderizar(QPainter& painter)
{
    int screenX = 512 + posicion.x * 180 - 140;
    int screenY = 400;
    setPosicionReal(static_cast<float>(screenX), static_cast<float>(screenY));

    if (!frames[frameActual].isNull())
    {
        painter.drawPixmap(screenX, screenY, 150, 250, frames[frameActual]);

        if (tiempoInvulnerable > 0.0f)
        {
            painter.setOpacity(0.35);
            painter.fillRect(screenX, screenY, 280, 380, Qt::yellow);
            painter.setOpacity(1.0);
        }
    }
    else
    {
        painter.setBrush(tiempoInvulnerable > 0.0f ? Qt::yellow : Qt::blue);
        painter.drawRect(screenX, screenY, 50, 50);
    }
}

void Jugador::procesarInput(char tecla)
{
    if (tecla == 'a' || tecla == 'A')
        moverIzquierda();
    else if (tecla == 'd' || tecla == 'D')
        moverDerecha();
}

void Jugador::moverIzquierda()
{
    if (carril > 0)
        carril--;
}

void Jugador::moverDerecha()
{
    if (carril < 2)
        carril++;
}

int Jugador::getCarril() const
{
    return carril;
}

void Jugador::usarMovimientoLibre(bool estado)
{
    movimientoLibre = estado;
}
