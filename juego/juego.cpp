#include "juego.h"
#include "jugador.h"

Juego::Juego()
{
    nivelActual = nullptr;
    ejecutando = false;
}

Juego::~Juego()
{
    limpiarNivel();
}

void Juego::iniciarNivel1(Nivel1::Dificultad dificultad)
{
    cambiarNivel(new Nivel1(dificultad));
}

void Juego::iniciarNivel2()
{
    cambiarNivel(new Nivel2());
}

void Juego::limpiarNivel()
{
    delete nivelActual;
    nivelActual = nullptr;
    ejecutando = false;
}

Juego::ResultadoActualizacion Juego::actualizar(float dt, float scrollMundo)
{
    if (!ejecutando || !nivelActual)
        return SinCambios;

    nivelActual->configurarMovimiento(dt, scrollMundo);
    nivelActual->actualizar();

    if (jugadorGanoNivel2())
        return VictoriaNivel2;

    if (nivelActual->pidioSiguienteNivel())
        return SiguienteNivel;

    if (jugadorPerdioNivel1() || jugadorPerdioNivel2())
        return Derrota;

    return SinCambios;
}

void Juego::manejarTeclaPresionada(int key)
{
    if (!nivelActual)
        return;

    Nivel2* nivel2 = getNivel2();

    if (nivel2)
    {
        if (key == Qt::Key_A || key == Qt::Key_D || key == Qt::Key_W)
            nivel2->keyPresado(key);

        if (key == Qt::Key_Z)
            nivel2->ataqueJugador();

        if (key == Qt::Key_X)
            nivel2->bloqueoJugador(true);

        if (key == Qt::Key_I)
            nivel2->setIAActiva(!nivel2->getIAActiva());

        return;
    }

    Jugador* jugador = nivelActual->getJugador();
    if (!jugador)
        return;

    if (key == Qt::Key_A)
        jugador->procesarInput('a');

    if (key == Qt::Key_D)
        jugador->procesarInput('d');
}

void Juego::manejarTeclaSoltada(int key)
{
    Nivel2* nivel2 = getNivel2();
    if (!nivel2)
        return;

    if (key == Qt::Key_A || key == Qt::Key_D)
        nivel2->keySoltado(key);

    if (key == Qt::Key_X)
        nivel2->bloqueoJugador(false);
}

void Juego::manejarClick(int x, int y)
{
    if (nivelActual)
        nivelActual->manejarClick(x, y);
}

void Juego::cambiarNivel(Nivel* nuevoNivel)
{
    delete nivelActual;
    nivelActual = nuevoNivel;
    ejecutando = nivelActual != nullptr;

    if (nivelActual)
        nivelActual->iniciar();
}

bool Juego::estaEjecutando() const
{
    return ejecutando;
}

bool Juego::tieneNivel() const
{
    return nivelActual != nullptr;
}

float Juego::getVelocidadMundo() const
{
    if (!nivelActual)
        return 0.0f;

    return nivelActual->getVelocidadMundo();
}

Nivel* Juego::getNivel()
{
    return nivelActual;
}

Nivel2* Juego::getNivel2()
{
    return dynamic_cast<Nivel2*>(nivelActual);
}

bool Juego::jugadorPerdioNivel1() const
{
    return false;
}

bool Juego::jugadorPerdioNivel2() const
{
    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivelActual);
    if (!nivel2 || !nivel2->terminado())
        return false;

    Jugador* jugador = nivel2->getJugador1();
    return jugador && jugador->getVida() <= 0;
}

bool Juego::jugadorGanoNivel2() const
{
    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivelActual);
    if (!nivel2 || !nivel2->terminado())
        return false;

    Jugador* jugador = nivel2->getJugador1();
    Jugador* ia = nivel2->getJugador2();

    return jugador && jugador->getVida() > 0 && ia && ia->getVida() <= 0;
}
