#include "ia.h"


// Constructor

IA::IA()
    : robertBloqueando(false)
    , timerEspera(0.0f)
    , timerBloqueo(0.0f)
    , timerRetroceso(0.0f)
    , cooldownInterno(0.0f)
    , accionMover(false)
    , accionMoverDir(true)
    , accionAtacar(false)
    , accionSaltar(false)
    , accionBloqueo(false)
    , accionRetroceder(false)
    , golpesRecibidos(0)
    , golpesDados(0)
    , fallosSegidos(0)
    , pausaExtra(0.0f)
{}


// actualizar – punto de entrada único por frame

void IA::actualizar(Jugador* rhaegar, Jugador* robert,
                    float& velY2, bool& enSuelo2,
                    float& velEmpuje2, float& velEmpuje1,
                    float& cooldownAtaque2, float dt)
{
    if (!rhaegar || !robert) return;

    // ── Resetear flags
    accionMover      = false;
    accionAtacar     = false;
    accionSaltar     = false;
    accionBloqueo    = false;
    accionRetroceder = false;

    // ── Bajar timers
    if (timerEspera    > 0.0f) timerEspera    -= dt;
    if (timerBloqueo   > 0.0f) timerBloqueo   -= dt;
    if (timerRetroceso > 0.0f) timerRetroceso -= dt;
    if (cooldownInterno > 0.0f) cooldownInterno -= dt;
    // sincronizar con el cooldown externo
    if (cooldownAtaque2 < cooldownInterno) cooldownInterno = cooldownAtaque2;

    // Leer estado
    Vector3 pr = rhaegar->getPosicion();
    Vector3 pb = robert->getPosicion();

    float diff    = pb.x - pr.x;
    float absDist = diff < 0.0f ? -diff : diff;
    bool  enSuelo = enSuelo2;

    float vidaRhaegar  = rhaegar->getVida();
    float vidaRobert   = robert->getVida();
    float escudoRobert = robert->getEscudo();

    // Dirección hacia Robert
    accionMoverDir = (diff >= 0.0f);

    //  DECISIÓN
    bool puedeAtacar = !robertBloqueando
                       && timerEspera    <= 0.0f
                       && cooldownInterno <= 0.0f;

    // ── CASO 1: vida baja → defensivo
    if (vidaRhaegar < VIDA_BAJA)
    {
        if (absDist < DIST_CERCA)
        {
            // Bloquear si no está bloqueando ya
            if (timerBloqueo <= 0.0f)
            {
                accionBloqueo  = true;
                timerBloqueo   = 0.5f;
                timerRetroceso = 0.4f;
                timerEspera    = 0.35f + pausaExtra;
            }
            else if (timerRetroceso > 0.0f)
            {
                accionRetroceder = true;
            }
            // Contraataque si Robert deja de bloquear
            else if (puedeAtacar && absDist <= DIST_ATAQUE && !robertBloqueando)
            {
                accionAtacar = true;
            }
        }
        else if (absDist <= DIST_ATAQUE && puedeAtacar)
        {
            accionAtacar = true;
        }
        else
        {
            // Reposicionarse lentamente
            if (timerEspera <= 0.0f)
                accionMover = true;
        }
    }
    // CASO 2: Robert bloqueando → no atacar, acercarse o esperar
    else if (robertBloqueando)
    {
        if (absDist > DIST_LEJOS)
        {
            accionMover = true;
        }
        else if (absDist < DIST_CERCA)
        {
            // Retroceder un poco para salir del rango
            accionRetroceder = true;
        }
        // Si está en zona media: esperar que suelte el bloqueo (no hacer nada)
    }
    // ── CASO 3: Robert sin escudo → full presión
    else if (escudoRobert <= 0.0f && vidaRobert < vidaRhaegar)
    {
        if (absDist > DIST_ATAQUE)
        {
            accionMover = true;
            // Salto para acortar distancia
            if (enSuelo && absDist > 120.0f)
                accionSaltar = true;
        }
        else if (puedeAtacar)
        {
            accionAtacar = true;
        }
    }
    // ── CASO 4: Robert muy lejos → perseguir
    else if (absDist > DIST_LEJOS)
    {
        accionMover = true;
        if (enSuelo && timerEspera <= 0.0f)
            accionSaltar = true;
    }
    // ── CASO 5: Robert en rango de ataque → atacar
    else if (absDist <= DIST_ATAQUE)
    {
        if (puedeAtacar)
        {
            accionAtacar = true;
        }
        else if (timerBloqueo <= 0.0f && absDist < DIST_CERCA * 0.8f)
        {
            // Demasiado cerca y no puede atacar → bloquear brevemente
            accionBloqueo = true;
            timerBloqueo  = 0.3f;
        }
    }
    // ─CASO 6: distancia media → acercarse
    else
    {
        accionMover = true;

        // Salto ofensivo si está lejos y lleva ventaja
        if (enSuelo && absDist > 120.0f && golpesDados > golpesRecibidos)
            accionSaltar = true;
    }

    // Imperfección: 8% de fallo aleatorio
    static unsigned int sem = 77777u;
    sem = sem * 1664525u + 1013904223u;
    float azar = static_cast<float>(sem & 0xFFFFu) / 65535.0f;
    if (azar < 0.08f)
    {
        accionAtacar  = false;
        accionBloqueo = false;
    }

    // EJECUTAR ACCIONES
    Vector3 pos = rhaegar->getPosicion();

    // Movimiento
    if (accionRetroceder)
    {
        float dir = accionMoverDir ? -1.0f : 1.0f;
        pos.x += dir * VEL_NORMAL * dt;
        rhaegar->setPosicion(pos.x, pos.y, pos.z);
    }
    else if (accionMover)
    {
        // Velocidad rápida si lleva ventaja, normal si no
        float vel = (golpesDados > golpesRecibidos + 2) ? VEL_RAPIDA : VEL_NORMAL;
        float dir = accionMoverDir ? 1.0f : -1.0f;
        pos.x += dir * vel * dt;
        rhaegar->setPosicion(pos.x, pos.y, pos.z);
    }

    // Salto
    if (accionSaltar && enSuelo2)
    {
        velY2    = FUERZA_SALTO;
        enSuelo2 = false;
    }

    // Ataque
    if (accionAtacar && cooldownInterno <= 0.0f)
    {
        pr = rhaegar->getPosicion();
        pb = robert->getPosicion();

        float l2 = pr.x,       r2 = pr.x + 60.0f;
        float l1 = pb.x,       r1 = pb.x + 60.0f;
        float t2 = 450 + pr.y, b2 = t2 + 100.0f;
        float t1 = 450 + pb.y, b1 = t1 + 100.0f;
        bool col = (l2 < r1 && r2 > l1 && t2 < b1 && b2 > t1);

        if (col)
        {
            // Cooldown se reduce si lleva ventaja (más agresivo)
            float cd = COOLDOWN_BASE;
            if (golpesDados > golpesRecibidos + 3) cd *= 0.75f;
            if (cd < 0.10f) cd = 0.10f;

            cooldownInterno  = cd;
            cooldownAtaque2  = cd;
            timerEspera      = 0.12f + pausaExtra * 0.4f; // pausa breve post-golpe

            // Daño
            float esc = robert->getEscudo();
            if (esc > 0.0f)
                robert->setEscudo(esc - 25.0f < 0.0f ? 0.0f : esc - 25.0f);
            else
                robert->setVida(robert->getVida() - 30.0f < 0.0f
                                    ? 0.0f : robert->getVida() - 30.0f);

            // Retroceso físico
            float dir       = (pb.x >= pr.x) ? 1.0f : -1.0f;
            float masaTotal = MASA_R1 + MASA_R2;
            velEmpuje1 +=  dir * FUERZA_EMPUJE * (MASA_R2 / masaTotal);
            velEmpuje2 += -dir * FUERZA_EMPUJE * (MASA_R1 / masaTotal);

            golpesDados++;
            fallosSegidos = 0;
            // Reducir pausa acumulada al conectar
            pausaExtra -= 0.04f;
            if (pausaExtra < 0.0f) pausaExtra = 0.0f;
        }
        else
        {
            // Falló: acumular pausa (máx 0.3s)
            fallosSegidos++;
            pausaExtra += 0.07f;
            if (pausaExtra > 0.30f) pausaExtra = 0.30f;
            timerEspera = 0.18f + pausaExtra;
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
    // Si recibe muchos golpes seguidos, bloquear un momento
    if (golpesRecibidos % 3 == 0 && timerBloqueo <= 0.0f)
    {
        timerBloqueo   = 0.35f;
        timerRetroceso = 0.25f;
    }
}

void IA::notificarPatronHitAndRun()
{
    // bloquear al acercarse la próxima vez
    pausaExtra += 0.05f;
    if (pausaExtra > 0.30f) pausaExtra = 0.30f;
}
