#include "juego.h"

Game::Game()
{
    nivelActual = nullptr;
    ejecutando = false;
}

Game::~Game()
{
    delete nivelActual;
    nivelActual = nullptr;
}

void Game::iniciar()
{
    ejecutando = true;

    if (nivelActual)
        nivelActual->iniciar();
}

void Game::actualizar()
{
    if (!ejecutando || !nivelActual)
        return;

    nivelActual->actualizar();

    if (nivelActual->terminado())
        ejecutando = false;
}

void Game::cambiarNivel(Nivel* nuevoNivel)
{
    delete nivelActual;
    nivelActual = nuevoNivel;

    if (nivelActual)
        nivelActual->iniciar();
}

bool Game::estaEjecutando() const
{
    return ejecutando;
}