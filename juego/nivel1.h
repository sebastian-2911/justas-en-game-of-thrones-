#ifndef NIVEL1_H
#define NIVEL1_H

#include "jugador.h"
#include "obstaculo.h"
#include "nivel.h"

#include <QPainter>
#include <QRect>
#include <QSoundEffect>
#include <QUrl>
#include <vector>

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
        float intervaloInicial;  // segundos entre spawns al inicio
        float intervaloMinimo;   // intervalo mas rapido posible
        float aceleracion;       // reduccion por segundo
        float velocidadMundo;    // pixeles/seg de scroll
    };

    Nivel1(Dificultad dificultadSeleccionada = NORMAL);
    ~Nivel1();

    void  iniciar()                              override;
    void  actualizar()                           override;
    void  renderizar(QPainter& painter)          override;
    void  configurarMovimiento(float nuevoDt, float nuevoScroll) override;
    float getVelocidadMundo() const              override;
    void  manejarClick(int x, int y)             override;
    bool  terminado()                            override;
    Jugador* getJugador()                        override;

    bool pidioSiguienteNivel()    const;
    bool aplicaDanioInicialNivel2() const;

private:
    // ---- entidades ----
    Jugador*                jugador;
    std::vector<Obstaculo*> obstaculos;

    // ---- dificultad ----
    Dificultad       dificultad;
    ConfigDificultad config;
    ConfigDificultad configs[2];

    float tiempoJuego;
    float tiempoTotalNivel;
    float tiempoGeneracion;
    float tiempoAlerta;
    float tiempoFinNivel;
    float intervaloActual;

    // ---- choques ----
    int choques;

    // ---- fases ----
    bool jefeFinalActivo;
    bool jugadorBloqueadoCentro;

    // ---- movimiento mundo ----
    float dt;
    float scrollMundo;

    // ---- estado ----
    bool finNivel;
    bool finalActivo;
    bool llegoAlFinal;
    bool pasoPorChoques;
    bool siguienteNivelSolicitado;

    // ---- posicion del jefe ----
    float xRealFinal;
    float yRealFinal;

    int patronActual;
    int contadorPatron;

    // ---- ui ----
    QRect botonReiniciar;

    // ---- carriles ----
    const float carriles[3] = { -1.3f, 0.0f, 1.0f };

    // ---- sonido daño ----
    QSoundEffect sonidoDanio;

private:
    void generarObstaculo();
    void verificarColisiones();
    void limpiarObstaculos();
    void escalarDificultad(float deltaTiempo);
    bool colision(Entidad* a, Entidad* b);
    bool colisionFinal();
    void activarJefeFinal();
    void restarTiempo(float segundos);
    void dibujarTemporizador(QPainter& painter);
    void dibujarJefeFinal(QPainter& painter);
};

#endif // NIVEL1_H
