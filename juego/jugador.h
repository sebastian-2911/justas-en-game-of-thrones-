#ifndef JUGADOR_H
#define JUGADOR_H

#include "entidad.h"
#include <QPainter>
#include <QPixmap>
#include <QRect>

enum TipoJugador
{
    JUGADOR_NIVEL1,
    JUGADOR_NIVEL2
};

class Jugador : public Entidad
{
public:
    Jugador(TipoJugador tipo = JUGADOR_NIVEL1);

    void actualizar();
    void actualizarNivel2(float dt);
    void renderizar(QPainter& painter);
    void procesarInput(char tecla);

    // nivel 1
    void moverIzquierda();
    void moverDerecha();

    // nivel 2
    void moverIzquierdaLibre();
    void moverDerechaLibre();
    void saltar();
    void atacar(Jugador* enemigo);
    bool colisiona(Jugador* otro);

    // stats
    float getVida()   const;
    float getEscudo() const;
    void  setVida(float valor);     // ← NUEVO
    void  setEscudo(float valor);

    int  getCarril() const;
    void usarMovimientoLibre(bool estado);

private:
    TipoJugador tipoJugador;

    // nivel 1
    int  carril;
    bool movimientoLibre;

    // nivel 2
    float velocidadY;
    float gravedad;
    bool  enSuelo;
    float vida;
    float escudo;

    // invulnerabilidad
    float tiempoInvulnerable;

    // hitbox
    Vector3 hitbox;

    // animación
    static const int TOTAL_FRAMES = 4;
    QPixmap frames[TOTAL_FRAMES];
    int     frameActual;
    float   tiempoFrame;
    static constexpr float DURACION_FRAME = 0.15f;

    // carriles nivel 1
    const float carriles[3] = { -1.5f, 0.0f, 1.5f };
};

#endif // JUGADOR_H
