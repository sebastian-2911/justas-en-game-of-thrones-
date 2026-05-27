#include "obstaculo.h"

// ── constructor normal ────────────────────────────────────────────────────────
Obstaculo::Obstaculo(float x, float posicionYReal)
{
    posicion  = {x, 0.0f, 0.0f};
    xReal     = 512 + x * 180 - 25;
    yReal     = posicionYReal;
    hitbox    = {50.0f, 50.0f};
    esFlecha_ = false;
    vx_       = 0.0f;
    vy_       = 0.0f;
}

// ── constructor flecha
Obstaculo::Obstaculo(float xPantalla, float yPantalla, float vx0, float vy0, bool esFlecha)
{
    posicion  = {0.0f, 0.0f, 0.0f};
    xReal     = xPantalla;
    yReal     = yPantalla;
    hitbox    = {40.0f, 10.0f};
    activa    = true;
    esFlecha_ = esFlecha;
    vx_       = vx0;
    vy_       = vy0;
}

// ── actualizar  (obstáculo normal) ─
void Obstaculo::actualizar()
{
    // obstáculo estático: nada
}

// ── actualizar con(flecha: cinemática) ───────────────────────────────────
void Obstaculo::actualizar(float dt)
{
    if (!activa) return;

    if (esFlecha_)
    {
        vy_   += GRAVEDAD * dt;   // aceleración gravitacional
        xReal += vx_ * dt;
        yReal += vy_ * dt;

        if (yReal > SUELO_Y)
            activa = false;
    }
}

// ── renderizar
void Obstaculo::renderizar(QPainter& painter)
{
    renderizar(painter, 0.0f);
}

void Obstaculo::renderizar(QPainter& painter, float scrollMundo)
{
    if (!activa) return;

    int screenX = static_cast<int>(xReal);
    int screenY = static_cast<int>(yReal + (esFlecha_ ? 0.0f : scrollMundo));

    if (esFlecha_)
    {
        // flecha nivel 2: cuadrado
        painter.setBrush(QColor(255, 220, 0));
        painter.setPen(Qt::NoPen);
        painter.drawRect(screenX - 8, screenY - 8, 16, 16);
    }
    else
    {
        // obstaculo normal nivel 1: cuadrado rojo
        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        painter.drawRect(screenX, screenY, 50, 50);
    }
}

// ── getRect
QRect Obstaculo::getRect(float scrollMundo) const
{
    int screenX = static_cast<int>(xReal);
    int screenY = static_cast<int>(yReal + (esFlecha_ ? 0.0f : scrollMundo));

    if (esFlecha_)
        return QRect(screenX - 20, screenY - 5, 40, 10);
    else
        return QRect(screenX, screenY, 50, 50);
}

bool Obstaculo::debeEliminarse() const { return !activa; }
