// ========================== nivel1.h ==========================
#ifndef NIVEL1_H
#define NIVEL1_H

#include "jugador.h"
#include "obstaculo.h"

#include <QPainter>
#include <QRect>
#include <vector>
#include "nivel.h"
class Nivel1 : public Nivel
{
public:

    enum Dificultad
    {
        NORMAL,
        DIFICIL
    };

    struct ConfigDificultad
    {
        float intervaloInicial;
        float intervaloMinimo;
        float aceleracion;
        float velocidadMundo;
    };

    Nivel1(
        Dificultad dificultadSeleccionada
        = NORMAL
        );

    ~Nivel1();

    void iniciar();

    void actualizar();

    void renderizar(QPainter& painter);

    void configurarMovimiento(
        float nuevoDt,
        float nuevoScroll
        );

    float getVelocidadMundo() const;

    void manejarClick(int x, int y);

    bool pidioReinicio() const;

    bool pidioSiguienteNivel() const;

    bool terminado();

    Jugador* getJugador();

private:

    // jugador
    Jugador* jugador;

    // obstaculos
    std::vector<Obstaculo*> obstaculos;

    // dificultad
    Dificultad dificultad;

    ConfigDificultad config;

    ConfigDificultad configs[2];

    // tiempo
    float tiempoTotalNivel;

    float tiempoRestante;

    float tiempoJuego;

    float tiempoGeneracion;

    float tiempoAlerta;

    float tiempoFinNivel;

    float intervaloActual;

    // movimiento
    float dt;

    float scrollMundo;

    // estado
    bool finNivel;

    bool finalActivo;

    bool llegoAlFinal;

    bool reinicioSolicitado;

    bool siguienteNivelSolicitado;

    // final
    float xRealFinal;

    float yRealFinal;

    // ui
    QRect botonReiniciar;

    // carriles
    const float carriles[3] =
        {
            -1.5f,
            0.0f,
            1.5f
    };

private:

    void generarObstaculo();

    void verificarColisiones();

    void limpiarObstaculos();

    void escalarDificultad(float dt);

    bool colision(
        Entidad* a,
        Entidad* b
        );

    bool colisionFinal();

    void activarFinal();

    void restarTiempo(float segundos);

    void dibujarTemporizador(
        QPainter& painter
        );

    void dibujarFinal(
        QPainter& painter
        );
};

#endif
