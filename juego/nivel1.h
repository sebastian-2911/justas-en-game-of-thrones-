#ifndef NIVEL1_H
#define NIVEL1_H

#include "jugador.h"
#include "obstaculo.h"
#include <QPainter>
#include <QRect>
#include <QSoundEffect>
#include <QUrl>
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
        float intervaloInicial;  // segundos entre spawns al inicio
        float intervaloMinimo;   // intervalo mas rapido posible
        float aceleracion;       // reduccion por segundo
        float velocidadMundo;    // pixeles/seg de scroll
    };

    Nivel1(Dificultad dificultadSeleccionada = NORMAL);
    ~Nivel1();

    void iniciar();
    void actualizar();
    void renderizar(QPainter& painter);
    void configurarMovimiento(float nuevoDt, float nuevoScroll);
    float getVelocidadMundo() const;
    void manejarClick(int x, int y);
    bool pidioSiguienteNivel() const;
    bool aplicaDanioInicialNivel2() const;
    bool terminado();
    Jugador* getJugador();

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

    // ---- choques
    int choques;

    // ---- fases
    bool jefeFinalActivo;
    bool jugadorBloqueadoCentro;

    // ---- movimiento mundo
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

    // ---- ui
    QRect botonReiniciar;

    // ---- carriles
    const float carriles[3] = { -1.5f, 0.0f, 1.5f };

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
