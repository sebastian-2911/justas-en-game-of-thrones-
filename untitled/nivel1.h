#ifndef NIVEL1_H
#define NIVEL1_H

#include <vector>
#include <QPainter>

class Jugador;
class Obstaculo;
class Entidad;

class Nivel1
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
    };

    Nivel1(Dificultad dif);
    ~Nivel1();

    void iniciar();
    void actualizar(float dt, float velocidadFondo);
    void renderizar(QPainter& painter);

    void manejarClick(int x, int y);

    bool terminado();
    Jugador* getJugador();

private:
    void generarObstaculo();
    void verificarColisiones();
    void limpiarObstaculos();
    void escalarDificultad(float dt);

    bool colision(Entidad* a, Entidad* b);

private:
    Jugador* jugador;
    std::vector<Obstaculo*> obstaculos;

    Dificultad dificultad;
    ConfigDificultad configs[2];

    float tiempoNivel;
    float tiempoTranscurrido;
    float tiempoGeneracion;
    float intervaloActual;
    float ultimaZGenerada;

    bool finNivel;

    float carriles[3] = {-0.5f, 1.5f, 2.0f};

    QRect botonReiniciar;
};

#endif
