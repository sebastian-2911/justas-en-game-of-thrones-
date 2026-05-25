#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "jugador.h"
#include <QPixmap>
#include <QPainter>

class Nivel2 : public Nivel
{
public:
    Nivel2();
    ~Nivel2();

    // Nivel (overrides)
    void  iniciar()                                  override;
    void  actualizar()                               override;
    void  renderizar(QPainter& painter)              override;
    bool  terminado()                                override;
    void  configurarMovimiento(float dt, float scroll) override;
    float getVelocidadMundo()                  const override;
    void  manejarClick(int x, int y)                 override;
    bool  pidioReinicio()                      const override;
    bool  pidioSiguienteNivel()                const override;
    Jugador* getJugador()                            override;

    // jugadores
    Jugador* getJugador1();
    Jugador* getJugador2();

    // controles jugador
    void moverJugadorIzquierda();
    void moverJugadorDerecha();
    void saltoJugador();
    void ataqueJugador();
    void bloqueoJugador(bool activo);

    // colisión
    bool hayColision();

    // fondos
    bool    isFondoCargado() const;
    bool    isPisoCargado()  const;
    QPixmap getFondo()       const;
    QPixmap getPiso()        const;

private:
    //  jugadores
    Jugador* jugador1;
    Jugador* jugador2;

    // fondos
    QPixmap fondo;
    QPixmap piso;
    bool    fondoCargado;
    bool    pisoCargado;

    // estado
    bool  finNivel;
    float dt;
    float tiempoEscudo;
    float limiteIzquierdo;
    float limiteDerecho;

    //  física
    float velY1;
    float velY2;
    bool  enSuelo1;
    bool  enSuelo2;

    // combate
    bool  bloqueando1;
    float cooldownAtaque1;
    float cooldownAtaque2;

    //  helpers privados
    void actualizarIA();
    void limitarJugador(Jugador* j);
    void aplicarGravedad(Jugador* j, float& velY, bool& enSuelo);
    void aplicarDanioDirecto(Jugador* objetivo);
    void aplicarDanio(Jugador* atacante, Jugador* defensor, bool bloqueando);
};

#endif // NIVEL2_H
