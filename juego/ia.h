#ifndef IA_H
#define IA_H
#include "jugador.h"

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
    bool  robertBloqueando;
    float timerBloqueo;
    float cooldownInterno;
    float timerSinAtaque;
    float timerPresion;

    bool accionMover;
    bool accionMoverDir;
    bool accionAtacar;
    bool accionSaltar;
    bool accionBloqueo;
    bool accionRetroceder;

    int   golpesRecibidos;
    int   golpesDados;
    int   fallosSegidos;
    int   golpesConsecutivos;
    int   golpesRecibidosSegidos;
    float pausaExtra;
    float agresividad;

    static constexpr float DIST_ATAQUE     =  68.0f;
    static constexpr float DIST_CERCA      =  85.0f;
    static constexpr float DIST_MEDIA      = 140.0f;
    static constexpr float DIST_LEJOS      = 190.0f;
    static constexpr float VIDA_BAJA       =  40.0f;
    static constexpr float VIDA_CRITICA    =  20.0f;
    static constexpr float VEL_NORMAL      = 320.0f;
    static constexpr float VEL_RAPIDA      = 370.0f;
    static constexpr float VEL_PRESION     = 400.0f;
    static constexpr float VEL_DESESPERADA = 420.0f;
    static constexpr float FUERZA_SALTO    = -720.0f;
    static constexpr float FUERZA_EMPUJE   = 700.0f;
    static constexpr float MASA_R1         =   2.0f;
    static constexpr float MASA_R2         =   1.0f;
    static constexpr float COOLDOWN_BASE   =   0.24f;
    static constexpr float PROB_ERROR      =   0.15f;
    static constexpr float TIEMPO_OFENSIVA =   3.0f;
};
#endif // IA_H
