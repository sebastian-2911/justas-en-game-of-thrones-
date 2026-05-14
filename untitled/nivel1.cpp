#include "Nivel1.h"
#include "Jugador.h"
#include "obstaculo.h"
#include "Entidad.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

// ========================== CONSTRUCTOR ==========================
Nivel1::Nivel1(Dificultad dif)
{
    dificultad = dif;

    jugador = nullptr;
    finNivel = false;

    configs[NORMAL]  = {2.0f, 0.7f, 0.02f};
    configs[DIFICIL] = {1.2f, 0.4f, 0.04f};
}

// ========================== DESTRUCTOR ==========================
Nivel1::~Nivel1()
{
    delete jugador;

    for (auto o : obstaculos)
        delete o;

    obstaculos.clear();
}

// ========================== INICIAR ==========================
void Nivel1::iniciar()
{
    delete jugador;
    jugador = new Jugador();

    jugador->setPosicion(carriles[1], 0.0f, 2.5f);

    for (auto o : obstaculos)
        delete o;

    obstaculos.clear();

    tiempoNivel = 30.0f;
    tiempoTranscurrido = 0.0f;
    tiempoGeneracion = 0.0f;

    intervaloActual = configs[dificultad].intervaloInicial;
    ultimaZGenerada = 20.0f;

    finNivel = false;
}

// ========================== ACTUALIZAR ==========================
void Nivel1::actualizar(float dt, float velocidadFondo)
{
    if (finNivel || !jugador)
        return;

    tiempoTranscurrido += dt;
    tiempoGeneracion += dt;

    jugador->actualizar();

    // ==========================
    // GENERAR OBSTÁCULOS
    // ==========================
    if (tiempoGeneracion >= intervaloActual)
    {
        generarObstaculo();
        tiempoGeneracion = 0.0f;
    }

    // ==========================
    // MOVER OBSTÁCULOS CON EL MAPA
    // ==========================
    for (auto o : obstaculos)
    {
        if (o)
        {
            auto pos = o->getPosicion();
            pos.z -= velocidadFondo * dt;
            o->setPosicion(pos.x, pos.y, pos.z);
        }
    }

    verificarColisiones();
    limpiarObstaculos();
    escalarDificultad(dt);

    if (jugador->getVida() <= 0 ||
        tiempoTranscurrido >= tiempoNivel)
    {
        finNivel = true;
    }
}

// ========================== RENDER ==========================
void Nivel1::renderizar(QPainter& painter)
{
    if (!jugador)
        return;

    jugador->renderizar(painter);

    for (auto o : obstaculos)
        if (o) o->renderizar(painter);

    int vida = jugador->getVida();

    painter.setBrush(Qt::gray);
    painter.drawRect(20, 20, 200, 20);

    if (vida > 60)
        painter.setBrush(Qt::green);
    else if (vida > 30)
        painter.setBrush(Qt::yellow);
    else
        painter.setBrush(Qt::red);

    painter.drawRect(20, 20, vida * 2, 20);

    if (finNivel && vida <= 0)
    {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 28, QFont::Bold));
        painter.drawText(150, 200, "GAME OVER");

        botonReiniciar = QRect(200, 250, 200, 50);

        painter.setBrush(Qt::darkGray);
        painter.drawRect(botonReiniciar);

        painter.setPen(Qt::white);
        painter.drawText(240, 285, "REINICIAR");
    }
}

// ========================== RESTO ==========================
void Nivel1::manejarClick(int x, int y)
{
    if (finNivel && botonReiniciar.contains(x, y))
        iniciar();
}

bool Nivel1::terminado() { return finNivel; }
Jugador* Nivel1::getJugador() { return jugador; }

void Nivel1::generarObstaculo()
{
    int carril = rand() % 3;
    float z = ultimaZGenerada + 8.0f + (rand() % 5);

    ultimaZGenerada = z;

    float x = carriles[carril];

    obstaculos.push_back(new Obstaculo(x, 0.0f, z));
}

void Nivel1::verificarColisiones()
{
    for (auto o : obstaculos)
    {
        if (o && o->estaActiva() && colision(jugador, o))
        {
            jugador->recibirDanio(20);
            o->desactivar();
        }
    }
}

void Nivel1::limpiarObstaculos()
{
    obstaculos.erase(
        std::remove_if(
            obstaculos.begin(),
            obstaculos.end(),
            [](Obstaculo* o)
            {
                if (!o || !o->estaActiva() ||
                    o->getPosicion().z < -10.0f)
                {
                    delete o;
                    return true;
                }
                return false;
            }),
        obstaculos.end()
        );
}

void Nivel1::escalarDificultad(float dt)
{
    auto cfg = configs[dificultad];

    if (intervaloActual > cfg.intervaloMinimo)
        intervaloActual -= cfg.aceleracion * dt * 30.0f;

    if (intervaloActual < cfg.intervaloMinimo)
        intervaloActual = cfg.intervaloMinimo;
}

bool Nivel1::colision(Entidad* a, Entidad* b)
{
    Jugador* j = dynamic_cast<Jugador*>(a);
    Obstaculo* o = dynamic_cast<Obstaculo*>(b);

    if (!j || !o)
        return false;

    float zJugador = 2.5f;
    float zActual = o->getPosicion().z;

    if (std::abs(zActual - zJugador) > 0.5f)
        return false;

    return std::abs(
               j->getPosicion().x -
               o->getPosicion().x
               ) < 0.1f;
}
