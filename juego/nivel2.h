#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "jugador.h"
#include "obstaculo.h"
#include "ia.h"
#include <QPixmap>
#include <QPainter>
#include <QRandomGenerator>
#include <vector>

class Nivel2 : public Nivel
{
public:
    Nivel2();
    ~Nivel2();

    // ── Nivel (overrides) ──────────────────────────────────────────────────
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

    // ── Jugadores ──────────────────────────────────────────────────────────
    Jugador* getJugador1();
    Jugador* getJugador2();

    // ── Controles jugador 1 (Robert) ──────────────────────────────────────
    void moverJugadorIzquierda();
    void moverJugadorDerecha();
    void saltoJugador();
    void ataqueJugador();
    void bloqueoJugador(bool activo);

    // ── Colisión cuerpo a cuerpo ───────────────────────────────────────────
    bool hayColision();

    // ── Fondos ────────────────────────────────────────────────────────────
    bool    isFondoCargado() const;
    bool    isPisoCargado()  const;
    QPixmap getFondo()       const;
    QPixmap getPiso()        const;

    // ── Flechas (para que MainWindow las dibuje) ──────────────────────────
    const std::vector<Obstaculo*>& getFlechas() const { return flechas; }

private:
    // ── Jugadores ──────────────────────────────────────────────────────────
    Jugador* jugador1;   // Robert Baratheon  – pesado, lento
    Jugador* jugador2;   // Rhaegar Targaryen – ligero, rápido

    // ── IA de Rhaegar ─────────────────────────────────────────────────────
    IA ia;

    // ── Fondos ────────────────────────────────────────────────────────────
    QPixmap fondo;
    QPixmap piso;
    bool    fondoCargado;
    bool    pisoCargado;

    // ── Estado general ────────────────────────────────────────────────────
    bool  finNivel;
    float dt;
    float tiempoEscudo;
    float limiteIzquierdo;
    float limiteDerecho;

    // ── Física vertical ───────────────────────────────────────────────────
    float velY1;
    float velY2;
    bool  enSuelo1;
    bool  enSuelo2;

    // ── Física de empuje ──────────────────────────────────────────────────
    float velEmpuje1;
    float velEmpuje2;

    // ── Combate ───────────────────────────────────────────────────────────
    bool  bloqueando1;
    float cooldownAtaque1;
    float cooldownAtaque2;

    // ── Detector de patrón hit-and-run de Robert ──────────────────────────
    float timerPostAtaqueRobert;  // cuenta 0.5 s tras cada golpe de Robert
    bool  robertAcabaDeAtacar;    // flag activo mientras el timer corre

    // ── Flechas ───────────────────────────────────────────────────────────
    std::vector<Obstaculo*> flechas;
    float tiempoSiguienteOleada;

    // ── Constantes de diseño ──────────────────────────────────────────────
    static constexpr float MASA_ROBERT          = 2.0f;
    static constexpr float MASA_RHAEGAR         = 1.0f;
    static constexpr float VEL_ROBERT           = 220.0f;
    static constexpr float VEL_RHAEGAR          = 420.0f;
    static constexpr float FUERZA_EMPUJE        = 700.0f;
    static constexpr float FUERZA_SALTO_ROBERT  = -480.0f;
    static constexpr float FUERZA_SALTO_RHAEGAR = -720.0f;
    static constexpr float FRICCION_EMPUJE      = 8.0f;
    static constexpr float DANIO_FLECHA         = 10.0f;

    // ── Helpers privados ──────────────────────────────────────────────────
    void actualizarIA();
    void limitarJugador(Jugador* j);
    void aplicarGravedad(Jugador* j, float& velY, bool& enSuelo);
    void aplicarDanioDirecto(Jugador* objetivo);
    void aplicarDanio(Jugador* atacante, Jugador* defensor, bool bloqueando);
    void aplicarFriccionEmpuje();

    void generarOleadaFlechas();
    void actualizarFlechas();
    void comprobarColisionesFlechas();
};

#endif // NIVEL2_H
