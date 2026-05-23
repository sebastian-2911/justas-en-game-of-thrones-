#ifndef NIVEL1_H
#define NIVEL1_H

#include "entidad.h"
#include "nivel.h"

#include <QPainter>
#include <QRect>
#include <vector>

class Obstaculo;
class Entidad;

class Nivel1 : public Nivel
{
public:
    enum Dificultad
    {
        NORMAL,
        DIFICIL
    };

    explicit Nivel1(Dificultad dificultad);
    ~Nivel1() override;

    void iniciar() override;
    void actualizar() override;
    void renderizar(QPainter& painter) override;
    void configurarMovimiento(
        float dt,
        float scroll
        ) override;
    float getVelocidadMundo() const override;

    void manejarClick(int x, int y) override;
    bool pidioReinicio() const override;
    bool pidioSiguienteNivel() const override;

    bool terminado() override;
    Jugador* getJugador() override;

private:
    struct ConfigDificultad
    {
        float intervaloInicial;
        float intervaloMinimo;
        float aceleracion;
        float velocidadMundo;
    };

    void generarObstaculo();
    void verificarColisiones();
    void limpiarObstaculos();
    void escalarDificultad(float dt);
    void dibujarTemporizador(QPainter& painter);
    void dibujarFinal(QPainter& painter);
    void activarFinal();
    void restarTiempo(float segundos);

    bool colision(Entidad* a, Entidad* b);
    bool colisionFinal();

private:
    // objetos
    Jugador* jugador;
    std::vector<Obstaculo*> obstaculos;

    // dificultad del nivel
    Dificultad dificultad;
    ConfigDificultad config;
    ConfigDificultad configs[2];

    // tiempo
    float tiempoTotalNivel;
    float tiempoRestante;
    float tiempoJuego;
    float tiempoGeneracion;
    float tiempoAlerta;
    float dt;
    float scrollMundo;
    float intervaloActual;

    bool finNivel;
    bool finalActivo;
    float tiempoFinNivel;
    bool llegoAlFinal;
    bool reinicioSolicitado;
    bool siguienteNivelSolicitado;

    // carriles
    float carriles[3] = {-1.0f, 0.0f, 1.0f};

    // final
    float xRealFinal;
    float yRealFinal;


    QRect botonReiniciar;
};

#endif
