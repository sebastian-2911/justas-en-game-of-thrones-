#ifndef NIVEL_H
#define NIVEL_H

#include <QPainter>

class Jugador;
class Nivel
{
public:
    Nivel() = default;
    virtual ~Nivel() = default;

    // ciclo del nivel
    virtual void iniciar() = 0;
    virtual void actualizar() = 0;
    virtual void renderizar(QPainter& painter) = 0;

    // configurar movimiento
    virtual void configurarMovimiento(
        float dt,
        float scroll
        ) = 0;
    virtual float getVelocidadMundo() const = 0;

    // estado
    virtual bool terminado() = 0;
    virtual void manejarClick(int x, int y) = 0;
    virtual bool pidioSiguienteNivel() const = 0;

    // conectar al jugador
    virtual Jugador* getJugador() = 0;
};

#endif
