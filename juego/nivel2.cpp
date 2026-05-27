#include "nivel2.h"

// constantes de fisica
static const float GRAVEDAD        = 1800.0f;
static const float FUERZA_SALTO    = -700.0f;
static const float SUELO_Y         = 0.0f;
static const float VELOCIDAD_MOV   = 300.0f;

// constantes de combate
static const float DANIO_ESCUDO    = 25.0f;
static const float DANIO_VIDA      = 30.0f;
static const float ESCUDO_MAX      = 100.0f;
static const float COOLDOWN_ATAQUE = 0.5f;

// constructor
Nivel2::Nivel2()
{
    jugador1        = nullptr;
    jugador2        = nullptr;
    fondoCargado    = false;
    pisoCargado     = false;
    finNivel        = false;
    dt              = 1.0f / 60.0f;
    tiempoEscudo    = 0.0f;
    limiteIzquierdo = 50.0f;
    limiteDerecho   = 900.0f;

    velY1    = 0.0f;
    enSuelo1 = true;
    velY2    = 0.0f;
    enSuelo2 = true;

    bloqueando1     = false;
    cooldownAtaque1 = 0.0f;
    cooldownAtaque2 = 0.0f;

    tiempoSiguienteOleada = 10.0f + static_cast<float>(
                                QRandomGenerator::global()->bounded(11));
}

// destructor
Nivel2::~Nivel2()
{
    delete jugador1;
    delete jugador2;

    for (auto* f : flechas) delete f;
    flechas.clear();
}

// iniciar
void Nivel2::iniciar()
{
    delete jugador1;
    delete jugador2;

    jugador1 = new Jugador(JUGADOR_NIVEL2);
    jugador2 = new Jugador(JUGADOR_NIVEL2);

    jugador1->setPosicion(200.0f, 0.0f, 0.0f);
    jugador2->setPosicion(700.0f, 0.0f, 0.0f);

    fondoCargado = fondo.load(":/fondo-nevel2.png");
    pisoCargado  = piso.load(":/piso.png");

    finNivel        = false;
    velY1           = 0.0f;
    enSuelo1        = true;
    velY2           = 0.0f;
    enSuelo2        = true;
    bloqueando1     = false;
    cooldownAtaque1 = 0.0f;
    cooldownAtaque2 = 0.0f;

    for (auto* f : flechas) delete f;
    flechas.clear();

    tiempoSiguienteOleada = 10.0f + static_cast<float>(
                                QRandomGenerator::global()->bounded(11));
}

// actualizar
void Nivel2::actualizar()
{
    if (!jugador1 || !jugador2) return;

    if (cooldownAtaque1 > 0.0f) cooldownAtaque1 -= dt;
    if (cooldownAtaque2 > 0.0f) cooldownAtaque2 -= dt;

    aplicarGravedad(jugador1, velY1, enSuelo1);
    aplicarGravedad(jugador2, velY2, enSuelo2);

    actualizarIA();

    limitarJugador(jugador1);
    limitarJugador(jugador2);

    // regenerar escudo cada 20
    tiempoEscudo += dt;
    if (tiempoEscudo >= 20.0f)
    {
        tiempoEscudo = 0.0f;
        float e1 = jugador1->getEscudo() + 20.0f;
        float e2 = jugador2->getEscudo() + 20.0f;
        jugador1->setEscudo(e1 > ESCUDO_MAX ? ESCUDO_MAX : e1);
        jugador2->setEscudo(e2 > ESCUDO_MAX ? ESCUDO_MAX : e2);
    }

    // oleadas de flechas
    tiempoSiguienteOleada -= dt;
    if (tiempoSiguienteOleada <= 0.0f)
    {
        generarOleadaFlechas();
        tiempoSiguienteOleada = 10.0f + static_cast<float>(
                                    QRandomGenerator::global()->bounded(11));
    }

    actualizarFlechas();
    comprobarColisionesFlechas();

    // fin de nivel
    if (jugador1->getVida() <= 0.0f || jugador2->getVida() <= 0.0f)
        finNivel = true;
}

// fisica: gravedad + suelo
void Nivel2::aplicarGravedad(Jugador* j, float& velY, bool& enSuelo)
{
    Vector3 pos = j->getPosicion();

    velY  += GRAVEDAD * dt;
    pos.y += velY * dt;

    if (pos.y >= SUELO_Y)
    {
        pos.y   = SUELO_Y;
        velY    = 0.0f;
        enSuelo = true;
    }
    else
    {
        enSuelo = false;
    }

    j->setPosicion(pos.x, pos.y, pos.z);
}

// renderizar (MainWindow lo hace)
void Nivel2::renderizar(QPainter& painter)
{
    (void)painter;
}

// combate
bool Nivel2::hayColision()
{
    if (!jugador1 || !jugador2) return false;

    Vector3 p1 = jugador1->getPosicion();
    Vector3 p2 = jugador2->getPosicion();

    float l1 = p1.x,        r1 = p1.x + 60.0f;
    float l2 = p2.x,        r2 = p2.x + 60.0f;
    float t1 = 450 + p1.y,  b1 = t1 + 100.0f;
    float t2 = 450 + p2.y,  b2 = t2 + 100.0f;

    return (l1 < r2 && r1 > l2 && t1 < b2 && b1 > t2);
}

void Nivel2::ataqueJugador()
{
    if (!jugador1 || !jugador2)  return;
    if (cooldownAtaque1 > 0.0f)  return;
    if (!hayColision())           return;

    cooldownAtaque1 = COOLDOWN_ATAQUE;
    aplicarDanioDirecto(jugador2);
}

void Nivel2::aplicarDanioDirecto(Jugador* objetivo)
{
    float escudo = objetivo->getEscudo();

    if (escudo > 0.0f)
    {
        escudo -= DANIO_ESCUDO;
        objetivo->setEscudo(escudo < 0.0f ? 0.0f : escudo);
    }
    else
    {
        float vida = objetivo->getVida() - DANIO_VIDA;
        objetivo->setVida(vida < 0.0f ? 0.0f : vida);
    }
}

void Nivel2::bloqueoJugador(bool activo)
{
    bloqueando1 = activo;
}

// IA
void Nivel2::actualizarIA()
{
    if (!jugador1 || !jugador2) return;
}

// movimiento jugador
void Nivel2::moverJugadorIzquierda()
{
    if (!jugador1) return;
    Vector3 pos = jugador1->getPosicion();
    pos.x -= VELOCIDAD_MOV * dt;
    jugador1->setPosicion(pos.x, pos.y, pos.z);
}

void Nivel2::moverJugadorDerecha()
{
    if (!jugador1) return;
    Vector3 pos = jugador1->getPosicion();
    pos.x += VELOCIDAD_MOV * dt;
    jugador1->setPosicion(pos.x, pos.y, pos.z);
}

void Nivel2::saltoJugador()
{
    if (!jugador1 || !enSuelo1) return;
    velY1    = FUERZA_SALTO;
    enSuelo1 = false;
}

// flechas: generar oleada
void Nivel2::generarOleadaFlechas()
{
    int cantidad = 1 + static_cast<int>(
                       QRandomGenerator::global()->bounded(3)); // 1, 2 o 3

    for (int i = 0; i < cantidad; i++)
    {
        float x = limiteIzquierdo + static_cast<float>(
                      QRandomGenerator::global()->bounded(
                          static_cast<int>(limiteDerecho - limiteIzquierdo)));

        float y = -30.0f; // fuera de pantalla arriba

        float vx = static_cast<float>(
                       QRandomGenerator::global()->bounded(200)) - 100.0f; // -100..+100

        float vy = 100.0f + static_cast<float>(
                       QRandomGenerator::global()->bounded(151)); // 100..250

        flechas.push_back(new Obstaculo(x, y, vx, vy, true));
    }
}

// flechas: actualizar posicion y limpiar
void Nivel2::actualizarFlechas()
{
    for (auto* f : flechas)
        f->actualizar(dt);

    auto it = flechas.begin();
    while (it != flechas.end())
    {
        if ((*it)->debeEliminarse())
        {
            delete *it;
            it = flechas.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// flechas: colisiones con jugadores
void Nivel2::comprobarColisionesFlechas()
{
    for (auto* f : flechas)
    {
        if (f->debeEliminarse()) continue;

        QRect rf = f->getRect();

        // colision con jugador1
        if (jugador1)
        {
            Vector3 p1 = jugador1->getPosicion();
            QRect rj1(
                static_cast<int>(p1.x),
                450 + static_cast<int>(p1.y),
                60, 100);

            if (rf.intersects(rj1))
            {
                float escudo = jugador1->getEscudo();
                if (escudo > 0.0f)
                    jugador1->setEscudo(escudo - DANIO_FLECHA < 0.0f ? 0.0f : escudo - DANIO_FLECHA);
                else
                    jugador1->setVida(jugador1->getVida() - DANIO_FLECHA < 0.0f ? 0.0f : jugador1->getVida() - DANIO_FLECHA);

                f->desactivar();
                continue;
            }
        }

        // colision con jugador2
        if (jugador2)
        {
            Vector3 p2 = jugador2->getPosicion();
            QRect rj2(
                static_cast<int>(p2.x),
                450 + static_cast<int>(p2.y),
                60, 100);

            if (rf.intersects(rj2))
            {
                float escudo = jugador2->getEscudo();
                if (escudo > 0.0f)
                    jugador2->setEscudo(escudo - DANIO_FLECHA < 0.0f ? 0.0f : escudo - DANIO_FLECHA);
                else
                    jugador2->setVida(jugador2->getVida() - DANIO_FLECHA < 0.0f ? 0.0f : jugador2->getVida() - DANIO_FLECHA);

                f->desactivar();
                continue;
            }
        }
    }
}

// helpers
bool  Nivel2::terminado()                { return finNivel; }
float Nivel2::getVelocidadMundo() const  { return 0.0f; }
void  Nivel2::manejarClick(int, int)     {}
bool  Nivel2::pidioReinicio()     const  { return false; }
bool  Nivel2::pidioSiguienteNivel() const { return false; }

void Nivel2::configurarMovimiento(float nuevoDt, float)
{
    dt = nuevoDt;
}

void Nivel2::limitarJugador(Jugador* j)
{
    Vector3 pos = j->getPosicion();
    if (pos.x < limiteIzquierdo) pos.x = limiteIzquierdo;
    if (pos.x > limiteDerecho)   pos.x = limiteDerecho;
    j->setPosicion(pos.x, pos.y, pos.z);
}

Jugador* Nivel2::getJugador()  { return jugador1; }
Jugador* Nivel2::getJugador1() { return jugador1; }
Jugador* Nivel2::getJugador2() { return jugador2; }
bool     Nivel2::isFondoCargado() const { return fondoCargado; }
bool     Nivel2::isPisoCargado()  const { return pisoCargado;  }
QPixmap  Nivel2::getFondo()       const { return fondo; }
QPixmap  Nivel2::getPiso()        const { return piso;  }

void Nivel2::aplicarDanio(Jugador*, Jugador*, bool) {}
