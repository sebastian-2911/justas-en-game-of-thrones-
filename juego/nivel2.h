#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "jugador.h"
#include "obstaculo.h"
#include <QPixmap>
#include <QPainter>
#include <QRandomGenerator>
#include <vector>

class Nivel2 : public Nivel
{
public:
    Nivel2();
    ~Nivel2();

    // Nivel (overrides)
    void  iniciar()                                    override;
    void  actualizar()                                 override;
    void  renderizar(QPainter& painter)                override;
    bool  terminado()                                  override;
    void  configurarMovimiento(float dt, float scroll) override;
    float getVelocidadMundo()                    const override;
    void  manejarClick(int x, int y)                   override;
    bool  pidioReinicio()                        const override;
    bool  pidioSiguienteNivel()                  const override;
    Jugador* getJugador()                              override;

    // jugadores
    Jugador* getJugador1();
    Jugador* getJugador2();

    // controles jugador
    void moverJugadorIzquierda();
    void moverJugadorDerecha();
    void saltoJugador();
    void ataqueJugador();
    void bloqueoJugador(bool activo);

    // colision jugadores
    bool hayColision();

    // fondos
    bool    isFondoCargado() const;
    bool    isPisoCargado()  const;
    QPixmap getFondo()       const;
    QPixmap getPiso()        const;

    // flechas (para que MainWindow las dibuje)
    const std::vector<Obstaculo*>& getFlechas() const { return flechas; }

private:
    // jugadores
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

    // fisica
    float velY1;
    float velY2;
    bool  enSuelo1;
    bool  enSuelo2;

    // combate
    bool  bloqueando1;
    float cooldownAtaque1;
    float cooldownAtaque2;

    // flechas
    std::vector<Obstaculo*> flechas;
    float tiempoSiguienteOleada;
    static constexpr float DANIO_FLECHA = 10.0f;

    // helpers privados
    void actualizarIA();
    void limitarJugador(Jugador* j);
    void aplicarGravedad(Jugador* j, float& velY, bool& enSuelo);
    void aplicarDanioDirecto(Jugador* objetivo);
    void aplicarDanio(Jugador* atacante, Jugador* defensor, bool bloqueando);

    // flechas helpers
    void generarOleadaFlechas();
    void actualizarFlechas();
    void comprobarColisionesFlechas();
};

#endif // NIVEL2_H
