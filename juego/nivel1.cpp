// ========================== nivel1.cpp ==========================
#include "entidad.h"
#include "jugador.h"
#include "nivel1.h"
#include "obstaculo.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <QString>
#include <QUrl>


static constexpr float FASE_FIN_SPAWN   = 47.0f; // dejan de spawnear enemigos
static constexpr float FASE_JEFE        = 50.0f; // jefe aparece + bloqueo central
static constexpr float DURACION_NIVEL   = 60.0f; // fin por tiempo

// --
Nivel1::Nivel1(Dificultad dificultadSeleccionada)
{
    dificultad = dificultadSeleccionada;

    // NORMAL: i
    configs[NORMAL] =
        {
            1.8f,   // intervaloInicial
            0.9f,   // intervaloMinimo
            0.004f, // aceleracion
            400.0f  // velocidadMundo
        };

    // DIFICIL:
    configs[DIFICIL] =
        {
            1.2f,
            0.6f,
            0.006f,
            600.0f
        };

    config = configs[dificultad];

    jugador                  = nullptr;
    finNivel                 = false;
    finalActivo              = false;
    llegoAlFinal             = false;
    perdioNivel              = false;
    reinicioSolicitado       = false;
    siguienteNivelSolicitado = false;
    jefeFinalActivo          = false;
    jugadorBloqueadoCentro   = false;

    tiempoFinNivel            = 0.0f;
    tiempoPerdidoPorColisiones = 0.0f;

    dt          = 1.0f / 60.0f;
    scrollMundo = 0.0f;

    xRealFinal = 477.0f;
    yRealFinal = -100.0f;

    patronActual  = 0;
    contadorPatron = 0;
}

Nivel1::~Nivel1()
{
    delete jugador;
    for (auto o : obstaculos)
        delete o;
    obstaculos.clear();
}

//
void Nivel1::iniciar()
{
    delete jugador;
    jugador = new Jugador(JUGADOR_NIVEL1);
    jugador->setPosicion(carriles[1], 0.0f, 2.5f);

    for (auto o : obstaculos)
        delete o;
    obstaculos.clear();

    tiempoJuego               = 0.0f;
    tiempoTotalNivel          = DURACION_NIVEL;
    tiempoGeneracion          = 0.0f;
    tiempoAlerta              = 0.0f;
    tiempoFinNivel            = 0.0f;
    tiempoPerdidoPorColisiones = 0.0f;

    intervaloActual = config.intervaloInicial;

    finNivel                 = false;
    finalActivo              = false;
    llegoAlFinal             = false;
    perdioNivel              = false;
    reinicioSolicitado       = false;
    siguienteNivelSolicitado = false;
    jefeFinalActivo          = false;
    jugadorBloqueadoCentro   = false;

    patronActual   = 0;
    contadorPatron = 0;

    // Cargar sonido de daño
    sonidoDanio.setSource(QUrl::fromLocalFile("dano.wav"));
    sonidoDanio.setVolume(0.8f);
}

//
void Nivel1::actualizar()
{
    if (!jugador)
        return;

    // ---- Pantalla de fin: espera 1.5 s y transiciona ----
    if (finNivel)
    {
        tiempoFinNivel += dt;
        if (tiempoFinNivel >= 1.5f)
        {
            if (llegoAlFinal)
                siguienteNivelSolicitado = true;
            else
                reinicioSolicitado = true;
        }
        return;
    }

    // ---- Avanzar reloj ----
    tiempoJuego      += dt;
    tiempoGeneracion += dt;

    // ---- Si el jugador esta bloqueado forzar carril central ----
    if (jugadorBloqueadoCentro)
        jugador->setPosicion(carriles[1], 0.0f, 2.5f);

    jugador->actualizar();

    if (tiempoAlerta > 0.0f)
        tiempoAlerta -= dt;

    // ---- Derrota por penalizacion acumulada ----
    if (tiempoPerdidoPorColisiones >= 30.0f)
    {
        perdioNivel    = true;
        finNivel       = true;
        tiempoFinNivel = 0.0f;
        return;
    }

    // ---- Fase jefe: activa a los 50 s ----
    if (tiempoJuego >= FASE_JEFE && !jefeFinalActivo)
        activarJefeFinal();

    // ---- Spawn de enemigos: solo antes de los 47 s ----
    if (!jefeFinalActivo
        && tiempoJuego < FASE_FIN_SPAWN
        && tiempoGeneracion >= intervaloActual)
    {
        generarObstaculo();
        tiempoGeneracion = 0.0f;
    }

    verificarColisiones();
    limpiarObstaculos();

    if (!jefeFinalActivo)
        escalarDificultad(dt);

    // ---- Colision con el jefe -> GANA (llego al jefe) ----
    if (jefeFinalActivo && colisionFinal())
    {
        llegoAlFinal   = true;
        finNivel       = true;
        tiempoFinNivel = 0.0f;
        return;
    }

    // ---- Fin por tiempo sin haber tocado al jefe -> PIERDE ----
    if (tiempoJuego >= DURACION_NIVEL)
    {
        if (!llegoAlFinal)
            perdioNivel = true;

        finNivel       = true;
        tiempoFinNivel = 0.0f;
    }
}

//
void Nivel1::renderizar(QPainter& painter)
{
    if (!jugador)
        return;

    jugador->renderizar(painter);

    for (auto o : obstaculos)
        if (o)
            o->renderizar(painter, scrollMundo);

    if (jefeFinalActivo)
        dibujarJefeFinal(painter);

    dibujarTemporizador(painter);

    // HUD penalizacion
    if (tiempoPerdidoPorColisiones > 0.0f)
    {
        painter.setPen(QColor(255, 80, 80));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        QString txt = QString("Penalizacion: -%1s / -30s")
                          .arg((int)tiempoPerdidoPorColisiones);
        painter.drawText(QRect(0, 60, 1024, 30), Qt::AlignCenter, txt);
    }

    // Aviso de jefe
    if (jugadorBloqueadoCentro)
    {
        painter.setPen(QColor(255, 200, 0));
        painter.setFont(QFont("Arial", 18, QFont::Bold));
        painter.drawText(QRect(0, 95, 1024, 30), Qt::AlignCenter,
                         "¡JEFE FINAL! No puedes moverte del centro");
    }
}

//
void Nivel1::configurarMovimiento(float nuevoDt, float nuevoScroll)
{
    dt          = nuevoDt;
    scrollMundo = nuevoScroll;
}

float Nivel1::getVelocidadMundo() const
{
    return config.velocidadMundo;
}

void Nivel1::manejarClick(int x, int y)
{
    if (finNivel && botonReiniciar.contains(x, y))
    {
        if (llegoAlFinal)
            siguienteNivelSolicitado = true;
        else
            reinicioSolicitado = true;
    }
}

bool Nivel1::pidioReinicio() const       { return reinicioSolicitado; }
bool Nivel1::pidioSiguienteNivel() const { return siguienteNivelSolicitado; }
bool Nivel1::terminado()                 { return finNivel; }
Jugador* Nivel1::getJugador()            { return jugador; }


void Nivel1::generarObstaculo()
{
    // Pares de carriles bloqueados para cada patron
    const int bloqueados[3][2] = {
        {1, 2},  // patron 0 → libre el 0
        {0, 2},  // patron 1 → libre el 1
        {0, 1}   // patron 2 → libre el 2
    };

    contadorPatron++;
    if (contadorPatron >= 5)
    {
        contadorPatron = 0;
        // Elige patron al azar pero diferente al actual para variar
        int nuevo;
        do { nuevo = rand() % 3; } while (nuevo == patronActual);
        patronActual = nuevo;
    }

    // Spawn en los DOS carriles bloqueados → el tercero queda libre
    float yReal = -80.0f - scrollMundo;
    for (int i = 0; i < 2; ++i)
    {
        float x = carriles[bloqueados[patronActual][i]];
        obstaculos.push_back(new Obstaculo(x, yReal));
    }
}

// ---------------------------------------------------------------
void Nivel1::verificarColisiones()
{
    for (auto o : obstaculos)
    {
        if (o && o->estaActiva() && colision(jugador, o))
        {
            // Sonido de daño
            sonidoDanio.play();

            // Penalizacion de 5 s
            tiempoPerdidoPorColisiones += 5.0f;
            tiempoAlerta = 1.2f;
            o->desactivar();
        }
    }
}

void Nivel1::limpiarObstaculos()
{
    obstaculos.erase(
        std::remove_if(
            obstaculos.begin(), obstaculos.end(),
            [this](Obstaculo* o)
            {
                if (!o || !o->estaActiva() || o->getRect(scrollMundo).top() > 768)
                {
                    delete o;
                    return true;
                }
                return false;
            }),
        obstaculos.end());
}

void Nivel1::escalarDificultad(float deltaTiempo)
{
    intervaloActual -= config.aceleracion * deltaTiempo * 30.0f;
    if (intervaloActual < config.intervaloMinimo)
        intervaloActual = config.intervaloMinimo;
}

bool Nivel1::colision(Entidad* a, Entidad* b)
{
    if (!a || !b)
        return false;

    Obstaculo* obstaculo = dynamic_cast<Obstaculo*>(b);
    if (!obstaculo)
        return false;

    Vector3 pos = a->getPosicion();
    QRect rectJugador(
        static_cast<int>(512 + pos.x * 180 - 25),
        400,
        50, 50);

    return rectJugador.intersects(obstaculo->getRect(scrollMundo));
}

bool Nivel1::colisionFinal()
{
    Vector3 pos = jugador->getPosicion();
    QRect rectJugador(
        static_cast<int>(512 + pos.x * 180 - 25),
        400,
        50, 50);

    QRect rectJefe(
        static_cast<int>(xRealFinal),
        static_cast<int>(yRealFinal + scrollMundo),
        100, 100);

    return rectJugador.intersects(rectJefe);
}


void Nivel1::activarJefeFinal()
{
    jefeFinalActivo        = true;
    finalActivo            = true;
    jugadorBloqueadoCentro = true;

    // Coloca el jefe en la parte superior central de la pantalla
    xRealFinal = 462.0f;          // centrado en 1024 px (512 - 50)
    yRealFinal = -150.0f - scrollMundo;
}


void Nivel1::restarTiempo(float segundos)
{
    // Ya no se usa directamente, la penalizacion se acumula en
    // tiempoPerdidoPorColisiones y provoca derrota al llegar a 30 s.
    (void)segundos;
}


void Nivel1::dibujarTemporizador(QPainter& painter)
{
    float restante = tiempoTotalNivel - tiempoJuego;
    if (restante < 0.0f) restante = 0.0f;

    // Rojo cuando quedan 10 s o menos
    painter.setPen(restante <= 10.0f ? QColor(255, 80, 80) : Qt::white);
    painter.setFont(QFont("Arial", 26, QFont::Bold));

    QString texto = QString::number((int)std::ceil(restante));
    painter.drawText(QRect(0, 20, 1024, 40), Qt::AlignCenter, texto);

    // Aviso de colision reciente
    if (tiempoAlerta > 0.0f)
    {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 18, QFont::Bold));
        painter.drawText(QRect(0, 62, 1024, 32), Qt::AlignCenter, "-5 segundos");
    }
}

// ---------------------------------------------------------------
void Nivel1::dibujarJefeFinal(QPainter& painter)
{
    int sx = static_cast<int>(xRealFinal);
    int sy = static_cast<int>(yRealFinal + scrollMundo);

    // Cuerpo
    painter.setBrush(QColor(200, 0, 0));
    painter.setPen(QPen(Qt::white, 3));
    painter.drawRect(sx, sy, 100, 100);

    // Ojos
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(sx + 15, sy + 20, 22, 22);
    painter.drawEllipse(sx + 55, sy + 20, 22, 22);

    // Boca
    painter.setPen(QPen(Qt::yellow, 3));
    painter.drawLine(sx + 20, sy + 68, sx + 80, sy + 68);

    // Etiqueta
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 13, QFont::Bold));
    painter.drawText(sx, sy - 14, 100, 20, Qt::AlignCenter, "¡JEFE!");
}
