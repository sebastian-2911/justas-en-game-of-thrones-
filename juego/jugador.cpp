#include "jugador.h"

Jugador::Jugador(TipoJugador tipo)
{
    tipoJugador        = tipo;
    carril             = 1;
    movimientoLibre    = false;
    tiempoInvulnerable = 0.0f;
    hitbox             = {0.8f, 1.2f, 0.0f};
    posicion           = {0.0f, 0.0f, 0.0f};
    vida               = 100.0f;
    escudo             = 100.0f;
    velMovX            = 0.0f;
    velY               = 0.0f;
    enSuelo            = true;
    frameActual        = 0;
    tiempoFrame        = 0.0f;

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
    if (tipoJugador != JUGADOR_NIVEL1) return;

    if (!movimientoLibre)
        posicion.x = carriles[carril];

    if (tiempoInvulnerable > 0.0f)
        tiempoInvulnerable -= 0.016f;

    tiempoFrame += 0.016f;
    if (tiempoFrame >= DURACION_FRAME)
    {
        tiempoFrame -= DURACION_FRAME;
        frameActual  = (frameActual + 1) % TOTAL_FRAMES;
    }
}

void Jugador::actualizarNivel2(float dt)
{
    if (tipoJugador != JUGADOR_NIVEL2) return;

    posicion.x += velMovX * dt;

    velY       += GRAVEDAD * dt;
    posicion.y += velY * dt;

    if (posicion.y >= 0.0f)
    {
        posicion.y = 0.0f;
        velY       = 0.0f;
        enSuelo    = true;
    }
    else
    {
        enSuelo = false;
    }

    tiempoFrame += dt;
    if (tiempoFrame >= DURACION_FRAME)
    {
        tiempoFrame = 0.0f;
        frameActual = (frameActual + 1) % TOTAL_FRAMES;
    }
}

void Jugador::renderizar(QPainter& painter)
{
    int screenX, screenY;

    if (tipoJugador == JUGADOR_NIVEL1)
    {
        screenX = 512 + static_cast<int>(posicion.x * 180) - 140;
        screenY = 400;
    }
    else
    {
        screenX = static_cast<int>(posicion.x);
        screenY = 450 + static_cast<int>(posicion.y);
    }

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
    if (tecla == 'a' || tecla == 'A') moverIzquierda();
    else if (tecla == 'd' || tecla == 'D') moverDerecha();
}

void Jugador::moverIzquierda()
{
    if (tipoJugador == JUGADOR_NIVEL1) { if (carril > 0) carril--; return; }
    velMovX = -VEL_MOV;
}

void Jugador::moverDerecha()
{
    if (tipoJugador == JUGADOR_NIVEL1) { if (carril < 2) carril++; return; }
    velMovX = VEL_MOV;
}

void Jugador::frenar()
{
    velMovX = 0.0f;
}

void Jugador::saltar()
{
    if (!enSuelo) return;
    velY    = FUERZA_SALTO;
    enSuelo = false;
}

bool Jugador::colisiona(Jugador* otro)
{
    if (!otro) return false;
    QRect r1(static_cast<int>(posicion.x), static_cast<int>(posicion.y), 120, 220);
    QRect r2(static_cast<int>(otro->posicion.x), static_cast<int>(otro->posicion.y), 120, 220);
    return r1.intersects(r2);
}

float  Jugador::getVida()     const { return vida; }
float  Jugador::getEscudo()   const { return escudo; }
float  Jugador::getVelY()     const { return velY; }
float  Jugador::getVelMovX()  const { return velMovX; }
bool   Jugador::estaEnSuelo() const { return enSuelo; }
int    Jugador::getCarril()   const { return carril; }
float& Jugador::refVelY()           { return velY; }
bool&  Jugador::refEnSuelo()        { return enSuelo; }

void Jugador::setVida(float valor)
{
    vida = valor;
    if (vida < 0.0f) vida = 0.0f;
}

void Jugador::setEscudo(float valor)
{
    escudo = valor;
    if (escudo > 100.0f) escudo = 100.0f;
    if (escudo < 0.0f)   escudo = 0.0f;
}

void Jugador::setVelY(float valor)   { velY = valor; }
void Jugador::setEnSuelo(bool valor) { enSuelo = valor; }

void Jugador::usarMovimientoLibre(bool estado)
{
    movimientoLibre = estado;
}
