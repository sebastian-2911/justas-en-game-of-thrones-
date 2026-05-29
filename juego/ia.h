#ifndef IA_H
#define IA_H

#include "jugador.h"

// IA de Rhaegar Targaryen
class IA
{
public:
    IA();

    void actualizar(Jugador* rhaegar, Jugador* robert,
                    float& velY2, bool& enSuelo2,
                    float& velEmpuje2, float& velEmpuje1,
                    float& cooldownAtaque2, float dt);

    void notificarBloqueoRobert(bool bloqueando);
    void notificarGolpeRecibido();
    void notificarPatronHitAndRun();

    bool quiereBloquear() const { return accionBloqueo; }

private:
    // Estado interno
    bool  robertBloqueando;
    float timerEspera;          // pausa antes de volver a atacar
    float timerBloqueo;         // duración del bloqueo activo
    float timerRetroceso;       // tiempo retrocediendo
    float cooldownInterno;      // cooldown propio de ataque

    // Flags de acción (se resetean cada frame)
    bool accionMover;
    bool accionMoverDir;        // true = derecha
    bool accionAtacar;
    bool accionSaltar;
    bool accionBloqueo;
    bool accionRetroceder;

    // Aprendizaje simple
    int   golpesRecibidos;
    int   golpesDados;
    int   fallosSegidos;
    float pausaExtra;           // se acumula con fallos, se reduce con éxitos

    // Constantes
    static constexpr float DIST_ATAQUE   =  65.0f;
    static constexpr float DIST_CERCA    =  80.0f;
    static constexpr float DIST_LEJOS    = 200.0f;
    static constexpr float VIDA_BAJA     =  40.0f;
    static constexpr float VEL_NORMAL    = 440.0f;
    static constexpr float VEL_RAPIDA    = 510.0f;
    static constexpr float FUERZA_SALTO  = -720.0f;
    static constexpr float FUERZA_EMPUJE = 700.0f;
    static constexpr float MASA_R1       =   2.0f;  // Robert
    static constexpr float MASA_R2       =   1.0f;  // Rhaegar
    static constexpr float COOLDOWN_BASE =   0.22f;
};

#endif // IA_H
