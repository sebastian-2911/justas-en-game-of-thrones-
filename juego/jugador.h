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
    void moverIzquierda();
    void moverDerecha();
    void frenar();
    void saltar();

    bool colisiona(Jugador* otro);

    float  getVida()      const;
    float  getEscudo()    const;
    float  getVelY()      const;
    float  getVelMovX()   const;
    bool   estaEnSuelo()  const;
    int    getCarril()    const;

    float& refVelY();
    bool&  refEnSuelo();

    void setVida(float valor);
    void setEscudo(float valor);
    void setVelY(float valor);
    void setEnSuelo(bool valor);
    void usarMovimientoLibre(bool estado);

private:
    TipoJugador tipoJugador;

    int  carril;
    bool movimientoLibre;

    float vida;
    float escudo;
    float tiempoInvulnerable;

    float velMovX;
    float velY;
    bool  enSuelo;

    Vector3 hitbox;

    static const int TOTAL_FRAMES = 4;
    QPixmap frames[TOTAL_FRAMES];
    int     frameActual;
    float   tiempoFrame;

    static constexpr float DURACION_FRAME = 0.15f;
    static constexpr float VEL_MOV        = 310.0f;
    static constexpr float FUERZA_SALTO   = -620.0f;
    static constexpr float GRAVEDAD       = 1800.0f;

    const float carriles[3] = { -1.5f, 0.0f, 1.5f };
};

#endif // JUGADOR_H
