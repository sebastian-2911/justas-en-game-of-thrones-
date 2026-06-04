#include "ia.h"

IA::IA()
    : robertBloqueando(false)
    , timerBloqueo(0.0f)
    , cooldownInterno(0.0f)
    , timerSinAtaque(0.0f)
    , timerPresion(0.0f)
    , accionMover(false)
    , accionMoverDir(true)
    , accionAtacar(false)
    , accionSaltar(false)
    , accionBloqueo(false)
    , accionRetroceder(false)
    , golpesRecibidos(0)
    , golpesDados(0)
    , fallosSegidos(0)
    , golpesConsecutivos(0)
    , golpesRecibidosSegidos(0)
    , pausaExtra(0.0f)
    , agresividad(0.40f)
{}

void IA::actualizar(Jugador* rhaegar, Jugador* robert,
                    float& velY2, bool& enSuelo2,
                    float& velEmpuje2, float& velEmpuje1,
                    float& cooldownAtaque2, float dt)
{
    if (!rhaegar || !robert) return;

    accionMover      = false;
    accionAtacar     = false;
    accionSaltar     = false;
    accionBloqueo    = false;
    accionRetroceder = false;

    if (timerBloqueo    > 0.0f) timerBloqueo    -= dt;
    if (cooldownInterno > 0.0f) cooldownInterno -= dt;
    if (timerPresion    > 0.0f) timerPresion    -= dt;
    if (cooldownAtaque2 < cooldownInterno) cooldownInterno = cooldownAtaque2;

    timerSinAtaque += dt;

    Vector3 pr = rhaegar->getPosicion();
    Vector3 pb = robert->getPosicion();

    float diff        = pb.x - pr.x;
    float absDist     = diff < 0.0f ? -diff : diff;
    float vidaRhaegar = rhaegar->getVida();
    float vidaRobert  = robert->getVida();
    float escRobert   = robert->getEscudo();
    float escRhaegar  = rhaegar->getEscudo();
    bool  enSuelo     = enSuelo2;
    bool  ganando     = vidaRobert < vidaRhaegar - 15.0f;
    bool  escudoAlto  = escRhaegar > 55.0f;

    accionMoverDir = (diff >= 0.0f);

    bool puedeAtacar = !robertBloqueando
                       && cooldownInterno <= 0.0f;

    if (timerSinAtaque >= TIEMPO_OFENSIVA && vidaRhaegar > VIDA_BAJA)
    {
        timerSinAtaque = 0.0f;
        timerPresion   = 2.0f;
        pausaExtra     = 0.0f;
    }

    bool modoPresion = (timerPresion > 0.0f && !robertBloqueando);

    if (vidaRhaegar < VIDA_CRITICA)
    {
        if (timerSinAtaque >= 1.5f)
        {
            timerSinAtaque = 0.0f;
            timerPresion   = 3.0f;
            pausaExtra     = 0.0f;
        }

        modoPresion = true;

        if (absDist > DIST_ATAQUE)
        {
            accionMover = true;
            if (enSuelo && absDist > DIST_MEDIA * 0.6f)
                accionSaltar = true;
        }
        else if (puedeAtacar)
        {
            accionAtacar = true;
        }
        else if (absDist < DIST_CERCA * 0.6f && timerBloqueo <= 0.0f)
        {
            accionBloqueo = true;
            timerBloqueo  = 0.15f;
        }
    }
    else if (vidaRhaegar < VIDA_BAJA)
    {
        if (timerSinAtaque >= 2.0f)
        {
            timerSinAtaque = 0.0f;
            timerPresion   = 2.5f;
            pausaExtra     = 0.0f;
        }

        modoPresion = true;

        if (absDist > DIST_ATAQUE)
        {
            accionMover = true;
            if (enSuelo && absDist > DIST_MEDIA * 0.7f)
                accionSaltar = true;
        }
        else if (puedeAtacar)
        {
            accionAtacar = true;
        }
        else if (absDist < DIST_CERCA * 0.75f && timerBloqueo <= 0.0f)
        {
            accionBloqueo = true;
            timerBloqueo  = 0.20f;
        }
    }
    else if (modoPresion)
    {
        if (absDist > DIST_ATAQUE)
        {
            accionMover = true;
            if (enSuelo && absDist > DIST_MEDIA * 0.7f)
                accionSaltar = true;
        }
        else if (puedeAtacar)
        {
            accionAtacar = true;
        }
        else if (absDist < DIST_CERCA * 0.75f && timerBloqueo <= 0.0f)
        {
            accionBloqueo = true;
            timerBloqueo  = 0.20f;
        }
    }
    else if (robertBloqueando)
    {
        if (absDist > DIST_LEJOS)
            accionMover = true;
        else if (absDist < DIST_CERCA)
            accionRetroceder = true;
    }
    else if (escRobert <= 0.0f)
    {
        if (absDist > DIST_ATAQUE)
        {
            accionMover = true;
            if (enSuelo && absDist > 110.0f)
                accionSaltar = true;
        }
        else if (puedeAtacar)
        {
            accionAtacar = true;
        }
    }
    else if (ganando && escudoAlto)
    {
        if (absDist > DIST_ATAQUE)
        {
            accionMover = true;
            if (enSuelo && absDist > DIST_MEDIA)
                accionSaltar = true;
        }
        else if (puedeAtacar)
        {
            accionAtacar = true;
        }
    }
    else if (absDist > DIST_LEJOS)
    {
        accionMover = true;
        if (enSuelo)
            accionSaltar = true;
    }
    else if (absDist <= DIST_ATAQUE)
    {
        if (puedeAtacar)
        {
            accionAtacar = true;
        }
        else if (timerBloqueo <= 0.0f && absDist < DIST_CERCA * 0.75f)
        {
            accionBloqueo = true;
            timerBloqueo  = 0.25f;
        }
    }
    else if (absDist <= DIST_MEDIA)
    {
        accionMover = true;
        if (enSuelo && absDist > 100.0f && agresividad > 0.35f)
            accionSaltar = true;
    }
    else
    {
        accionMover = true;
    }

    static unsigned int sem = 77777u;
    sem = sem * 1664525u + 1013904223u;
    float azar = static_cast<float>(sem & 0xFFFFu) / 65535.0f;
    if (azar < PROB_ERROR && vidaRhaegar >= VIDA_BAJA)
    {
        accionAtacar  = false;
        accionBloqueo = false;
    }

    Vector3 pos = rhaegar->getPosicion();

    float velActual = VEL_NORMAL;
    if (vidaRhaegar < VIDA_CRITICA)     velActual = VEL_DESESPERADA;
    else if (vidaRhaegar < VIDA_BAJA)   velActual = VEL_PRESION;
    else if (modoPresion)               velActual = VEL_PRESION;
    else if (golpesDados > golpesRecibidos + 2) velActual = VEL_RAPIDA;

    if (accionRetroceder)
    {
        float dir = accionMoverDir ? -1.0f : 1.0f;
        pos.x += dir * velActual * dt;
        rhaegar->setPosicion(pos.x, pos.y, pos.z);
        rhaegar->mirarHaciaDerecha(dir > 0.0f);
        rhaegar->setEstadoAnimacion(ANIM_CAMINAR);
    }
    else if (accionMover)
    {
        float dir = accionMoverDir ? 1.0f : -1.0f;
        pos.x += dir * velActual * dt;
        rhaegar->setPosicion(pos.x, pos.y, pos.z);
        rhaegar->mirarHaciaDerecha(dir > 0.0f);
        rhaegar->setEstadoAnimacion(ANIM_CAMINAR);
    }

    if (accionSaltar && enSuelo2)
    {
        velY2    = FUERZA_SALTO;
        enSuelo2 = false;
    }

    if (accionAtacar && cooldownInterno <= 0.0f)
    {
        pr = rhaegar->getPosicion();
        pb = robert->getPosicion();
        rhaegar->mirarHaciaDerecha(pb.x >= pr.x);
        rhaegar->setEstadoAnimacion(ANIM_ATAQUE);

        float l2 = pr.x,       r2 = pr.x + 60.0f;
        float l1 = pb.x,       r1 = pb.x + 60.0f;
        float t2 = 450 + pr.y, b2 = t2 + 100.0f;
        float t1 = 450 + pb.y, b1 = t1 + 100.0f;
        bool col = (l2 < r1 && r2 > l1 && t2 < b1 && b2 > t1);

        if (col)
        {
            float cdMult = (vidaRhaegar < VIDA_CRITICA) ? 0.65f
                           : (vidaRhaegar < VIDA_BAJA)    ? 0.75f
                                                        : 1.0f;
            float cd = COOLDOWN_BASE * cdMult * (1.0f - agresividad * 0.45f);
            if (cd < 0.09f) cd = 0.09f;

            cooldownInterno = cd;
            cooldownAtaque2 = cd;

            float esc = robert->getEscudo();
            if (esc > 0.0f)
                robert->setEscudo(esc - 25.0f < 0.0f ? 0.0f : esc - 25.0f);
            else
                robert->setVida(robert->getVida() - 30.0f < 0.0f
                                    ? 0.0f : robert->getVida() - 30.0f);
            robert->setEstadoAnimacion(ANIM_GOLPE);

            float dir       = (pb.x >= pr.x) ? 1.0f : -1.0f;
            float masaTotal = MASA_R1 + MASA_R2;
            velEmpuje1 +=  dir * FUERZA_EMPUJE * (MASA_R2 / masaTotal);
            velEmpuje2 += -dir * FUERZA_EMPUJE * (MASA_R1 / masaTotal);

            golpesDados++;
            golpesConsecutivos++;
            golpesRecibidosSegidos = 0;
            fallosSegidos = 0;

            if (pausaExtra > 0.0f)
            {
                pausaExtra -= 0.05f;
                if (pausaExtra < 0.0f) pausaExtra = 0.0f;
            }

            if (golpesConsecutivos >= 2)
            {
                agresividad += 0.04f;
                if (agresividad > 1.0f) agresividad = 1.0f;
            }

            timerSinAtaque = 0.0f;
        }
        else
        {
            fallosSegidos++;
            golpesConsecutivos = 0;
            pausaExtra += 0.06f;
            if (pausaExtra > 0.28f) pausaExtra = 0.28f;
            if (vidaRhaegar >= VIDA_BAJA)
            {
                agresividad -= 0.02f;
                if (agresividad < 0.20f) agresividad = 0.20f;
            }
        }
    }
}

void IA::notificarBloqueoRobert(bool bloqueando)
{
    robertBloqueando = bloqueando;
}

void IA::notificarGolpeRecibido()
{
    golpesRecibidos++;
    golpesRecibidosSegidos++;
    golpesConsecutivos = 0;
    timerSinAtaque     = 0.0f;

    if (golpesRecibidosSegidos >= 2 && timerBloqueo <= 0.0f)
    {
        timerBloqueo   = 0.20f;
        golpesRecibidosSegidos = 0;
    }

    agresividad += 0.05f;
    if (agresividad > 1.0f) agresividad = 1.0f;
}

void IA::notificarPatronHitAndRun()
{
    pausaExtra += 0.04f;
    if (pausaExtra > 0.28f) pausaExtra = 0.28f;
}
