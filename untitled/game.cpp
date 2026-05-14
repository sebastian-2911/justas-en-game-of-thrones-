#include "Game.h"

// ------------------ CONSTRUCTOR ------------------
Game::Game() {
    nivelActual = nullptr;
    ejecutando = false;
}

// ------------------ DESTRUCTOR ------------------
Game::~Game() {
    delete nivelActual;
    nivelActual = nullptr;
}

// ------------------ INICIAR ------------------
void Game::iniciar() {
    ejecutando = true;

    if (nivelActual) {
        nivelActual->iniciar();
    }
}

// ------------------ ACTUALIZAR ------------------
void Game::actualizar() {
    if (!ejecutando || !nivelActual) return;

    nivelActual->actualizar();

    if (nivelActual->terminado()) {
        ejecutando = false;
    }
}

// ------------------ CAMBIAR NIVEL ------------------
void Game::cambiarNivel(Nivel* nuevoNivel) {
    delete nivelActual;
    nivelActual = nuevoNivel;

    if (nivelActual) {
        nivelActual->iniciar();
    }
}

// ------------------ ESTADO ------------------
bool Game::estaEjecutando() const {
    return ejecutando;
}
