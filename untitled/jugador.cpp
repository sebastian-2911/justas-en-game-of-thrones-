#include "Jugador.h"

// ------------------ CONSTRUCTOR ------------------
Jugador::Jugador() {
    vida = 100;
    carril = 1;

    tiempoInvulnerable = 0.0f; //  inicializar

    hitbox = {1.5f, 1.5f};
    posicion = {0.0f, 0.0f, 0.0f};
}

// ------------------ ACTUALIZAR ------------------
void Jugador::actualizar() {
    posicion.x = carriles[carril];

    // reducir invulnerabilidad
    if (tiempoInvulnerable > 0.0f)
        tiempoInvulnerable -= 0.016f;
}

// ------------------ RENDER ------------------
void Jugador::renderizar(QPainter& painter) {
    int screenX = 300 + posicion.x * 80;
    int screenY = 400;

    // feedback visual de daño
    if (tiempoInvulnerable > 0.0f)
        painter.setBrush(Qt::yellow);//daño
    else
        painter.setBrush(Qt::blue);//normal

    painter.drawRect(screenX, screenY, 50, 50);
}

// ------------------ INPUT ------------------
void Jugador::procesarInput(char tecla) {
    if (tecla == 'a' || tecla == 'A')
        moverIzquierda();
    else if (tecla == 'd' || tecla == 'D')
        moverDerecha();
}

// ------------------ MOVIMIENTO ------------------
void Jugador::moverIzquierda() {
    if (carril > 0)
        carril--;
}

void Jugador::moverDerecha() {
    if (carril < 2)
        carril++;
}

// ------------------ VIDA ------------------
int Jugador::getVida() const {
    return vida;
}

void Jugador::recibirDanio(int d) {
    // evitar daño múltiple
    if (tiempoInvulnerable > 0.0f)
        return;

    vida -= d;

    if (vida < 0)
        vida = 0;

    // medio segundo invulnerable
    tiempoInvulnerable = 0.5f;
}

// ------------------ CARRIL ------------------
int Jugador::getCarril() const {
    return carril;
}
