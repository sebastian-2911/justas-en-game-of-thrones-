// ========================== nivel1.cpp ==========================
#include "entidad.h"
#include "jugador.h"
#include "nivel1.h"
#include "obstaculo.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <QString>

Nivel1::Nivel1(Dificultad dificultadSeleccionada)
{
    dificultad = dificultadSeleccionada;

    configs[NORMAL] =
        {
            2.0f,
            0.7f,
            0.02f,
            400.0f
        };

    configs[DIFICIL] =
        {
            0.55f,
            0.18f,
            0.08f,
            650.0f
        };

    config = configs[dificultad];

    jugador = nullptr;

    finNivel = false;

    finalActivo = false;

    llegoAlFinal = false;

    reinicioSolicitado = false;

    siguienteNivelSolicitado = false;

    tiempoFinNivel = 0.0f;

    dt = 1.0f / 60.0f;

    scrollMundo = 0.0f;

    xRealFinal = 477.0f;

    yRealFinal = -100.0f;
}

Nivel1::~Nivel1()
{
    delete jugador;

    for (auto o : obstaculos)
        delete o;

    obstaculos.clear();
}

void Nivel1::iniciar()
{
    // jugador NIVEL 1
    delete jugador;

    jugador = new Jugador(JUGADOR_NIVEL1);

    jugador->setPosicion(
        carriles[1],
        0.0f,
        2.5f
        );

    for (auto o : obstaculos)
        delete o;

    obstaculos.clear();

    // tiempo
    tiempoTotalNivel = 70.0f;

    tiempoRestante = tiempoTotalNivel;

    tiempoJuego = 0.0f;

    tiempoGeneracion = 0.0f;

    tiempoAlerta = 0.0f;

    intervaloActual = config.intervaloInicial;

    // estado
    finNivel = false;

    finalActivo = false;

    llegoAlFinal = false;

    reinicioSolicitado = false;

    siguienteNivelSolicitado = false;

    tiempoFinNivel = 0.0f;
}

void Nivel1::actualizar()
{
    if (!jugador)
        return;

    if (finNivel)
    {
        tiempoFinNivel += dt;

        if (tiempoFinNivel >= 2.0f)
            siguienteNivelSolicitado = true;

        return;
    }

    // relojes
    tiempoJuego += dt;

    tiempoRestante -= dt;

    tiempoGeneracion += dt;

    // actualizar jugador nivel1
    jugador->actualizar();

    if (tiempoAlerta > 0.0f)
        tiempoAlerta -= dt;

    if (tiempoJuego >= 50.0f && !finalActivo)
        activarFinal();

    // obstaculos
    if (tiempoGeneracion >= intervaloActual
        && tiempoJuego < 48.0f)
    {
        generarObstaculo();

        tiempoGeneracion = 0.0f;
    }

    verificarColisiones();

    limpiarObstaculos();

    escalarDificultad(dt);

    // final
    if (finalActivo && colisionFinal())
    {
        llegoAlFinal = true;

        finNivel = true;

        tiempoFinNivel = 0.0f;
    }

    if (tiempoRestante <= 0.0f)
    {
        tiempoRestante = 0.0f;

        finNivel = true;

        tiempoFinNivel = 0.0f;
    }
}

void Nivel1::renderizar(QPainter& painter)
{
    if (!jugador)
        return;

    // jugador
    jugador->renderizar(painter);

    // obstaculos
    for (auto o : obstaculos)
    {
        if (o)
            o->renderizar(painter, scrollMundo);
    }

    // final
    if (finalActivo)
        dibujarFinal(painter);

    // tiempo
    dibujarTemporizador(painter);

    if (finNivel)
    {
        painter.setPen(Qt::white);

        painter.setFont(
            QFont(
                "Arial",
                32,
                QFont::Bold
                )
            );

        if (llegoAlFinal)
        {
            painter.drawText(
                250,
                250,
                "NIVEL COMPLETADO"
                );
        }
        else
        {
            painter.drawText(
                280,
                250,
                "TIEMPO AGOTADO"
                );
        }

        painter.setFont(
            QFont(
                "Arial",
                20
                )
            );

        painter.drawText(
            260,
            320,
            "Pasando al nivel 2..."
            );
    }
}

void Nivel1::configurarMovimiento(
    float nuevoDt,
    float nuevoScroll
    )
{
    dt = nuevoDt;

    scrollMundo = nuevoScroll;
}

float Nivel1::getVelocidadMundo() const
{
    return config.velocidadMundo;
}

void Nivel1::manejarClick(int x, int y)
{
    if (finNivel
        && botonReiniciar.contains(x, y))
    {
        if (llegoAlFinal)
            siguienteNivelSolicitado = true;
        else
            reinicioSolicitado = true;
    }
}

bool Nivel1::pidioReinicio() const
{
    return reinicioSolicitado;
}

bool Nivel1::pidioSiguienteNivel() const
{
    return siguienteNivelSolicitado;
}

bool Nivel1::terminado()
{
    return finNivel;
}

Jugador* Nivel1::getJugador()
{
    return jugador;
}

void Nivel1::generarObstaculo()
{
    int carril = rand() % 3;

    float x = carriles[carril];

    float yReal =
        -80.0f - scrollMundo;

    obstaculos.push_back(
        new Obstaculo(x, yReal)
        );
}

void Nivel1::verificarColisiones()
{
    for (auto o : obstaculos)
    {
        if (o
            && o->estaActiva()
            && colision(jugador, o))
        {
            restarTiempo(5.0f);

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

            [this](Obstaculo* o)
            {
                if (!o
                    || !o->estaActiva()
                    || o->getRect(scrollMundo).top() > 768)
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
    if (intervaloActual
        > config.intervaloMinimo)
    {
        intervaloActual -=
            config.aceleracion * dt * 30.0f;
    }

    if (intervaloActual
        < config.intervaloMinimo)
    {
        intervaloActual =
            config.intervaloMinimo;
    }
}

bool Nivel1::colision(
    Entidad* a,
    Entidad* b
    )
{
    if (!a || !b)
        return false;

    Obstaculo* obstaculo =
        dynamic_cast<Obstaculo*>(b);

    if (!obstaculo)
        return false;

    Vector3 posJugador =
        a->getPosicion();

    QRect rectJugador(
        static_cast<int>(
            512 + posJugador.x * 180 - 25
            ),
        400,
        50,
        50
        );

    return rectJugador.intersects(
        obstaculo->getRect(scrollMundo)
        );
}

bool Nivel1::colisionFinal()
{
    Vector3 posJugador =
        jugador->getPosicion();

    QRect rectJugador(
        static_cast<int>(
            512 + posJugador.x * 180 - 25
            ),
        400,
        50,
        50
        );

    QRect rectFinal(
        static_cast<int>(xRealFinal),
        static_cast<int>(
            yRealFinal + scrollMundo
            ),
        70,
        70
        );

    return rectJugador.intersects(
        rectFinal
        );
}

void Nivel1::activarFinal()
{
    finalActivo = true;

    xRealFinal = 477.0f;

    yRealFinal =
        -100.0f - scrollMundo;
}

void Nivel1::restarTiempo(float segundos)
{
    tiempoRestante -= segundos;

    tiempoAlerta = 1.2f;

    if (tiempoRestante < 0.0f)
        tiempoRestante = 0.0f;
}

void Nivel1::dibujarTemporizador(
    QPainter& painter
    )
{
    painter.setPen(Qt::white);

    painter.setFont(
        QFont(
            "Arial",
            26,
            QFont::Bold
            )
        );

    QString texto =
        QString::number(
            (int)std::ceil(tiempoRestante)
            );

    painter.drawText(
        QRect(0, 20, 1024, 40),
        Qt::AlignCenter,
        texto
        );

    if (tiempoAlerta > 0.0f)
    {
        painter.setPen(Qt::yellow);

        painter.setFont(
            QFont(
                "Arial",
                18,
                QFont::Bold
                )
            );

        painter.drawText(
            QRect(0, 62, 1024, 32),
            Qt::AlignCenter,
            "-5 segundos"
            );
    }
}

void Nivel1::dibujarFinal(QPainter& painter)
{
    int screenX =
        static_cast<int>(xRealFinal);

    int screenY =
        static_cast<int>(
            yRealFinal + scrollMundo
            );

    painter.setBrush(
        QColor(255, 140, 0)
        );

    painter.setPen(Qt::white);

    painter.drawRect(
        screenX,
        screenY,
        70,
        70
        );
}
