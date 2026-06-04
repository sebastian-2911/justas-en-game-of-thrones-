#include "nivel2.h"
#include <QKeyEvent>
#include <cmath>

static const float VIDA_ROBERT  = 130.0f;
static const float VIDA_RHAEGAR =  85.0f;
static const float INTERVALO_OLEADA_MIN = 3.0f;
static const int   INTERVALO_OLEADA_VARIACION = 2;
static const float VELOCIDAD_FLECHA_X_MAX = 160.0f;
static const float VELOCIDAD_FLECHA_Y_MIN = 280.0f;
static const int   VELOCIDAD_FLECHA_Y_VARIACION = 221;
static const float INTERVALO_ESCUDO_BASE = 15.0f;
static const float INTERVALO_ESCUDO_EXTRA_VIDA_BAJA = 12.0f;

struct HitRect {
    float l, r, t, b;
    bool intersects(const HitRect& o) const {
        return l < o.r && r > o.l && t < o.b && b > o.t;
    }
};

static HitRect hitboxDe(const Vector3& p, float w, float h, float mx, float ybase, float my)
{
    HitRect h2;
    h2.l = p.x + mx;
    h2.r = p.x + mx + w;
    h2.t = ybase + p.y + my;
    h2.b = h2.t + h;
    return h2;
}

static HitRect hitboxAtaqueDe(const Vector3& atacante, const Vector3& defensor,
                              float w, float h, float mx, float ybase, float my, float rango)
{
    HitRect hx = hitboxDe(atacante, w, h, mx, ybase, my);
    if (defensor.x >= atacante.x) hx.r += rango;
    else                          hx.l -= rango;
    return hx;
}

static float intervaloRegeneracionEscudo(float vidaActual, float vidaMaxima)
{
    if (vidaMaxima <= 0.0f)
        return INTERVALO_ESCUDO_BASE + INTERVALO_ESCUDO_EXTRA_VIDA_BAJA;

    float proporcionVida = vidaActual / vidaMaxima;
    if (proporcionVida < 0.0f) proporcionVida = 0.0f;
    if (proporcionVida > 1.0f) proporcionVida = 1.0f;

    return INTERVALO_ESCUDO_BASE
           + (1.0f - proporcionVida) * INTERVALO_ESCUDO_EXTRA_VIDA_BAJA;
}

Nivel2::Nivel2()
{
    jugador1              = nullptr;
    jugador2              = nullptr;
    fondoCargado          = false;
    pisoCargado           = false;
    finNivel              = false;
    iaActiva              = true;   // IA encendida por defecto
    dt                    = 1.0f / 60.0f;
    tiempoEscudo1         = 0.0f;
    tiempoEscudo2         = 0.0f;
    limiteIzquierdo       = 50.0f;
    limiteDerecho         = 900.0f;
    velEmpuje1            = 0.0f;
    velEmpuje2            = 0.0f;
    bloqueando1           = false;
    cooldownAtaque1       = 0.0f;
    cooldownAtaque2       = 0.0f;
    tiempoDecisionIA      = 0.0f;
    timerPostAtaqueRobert = 0.0f; //transicion
    robertAcabaDeAtacar   = false;
    teclaDPresionada      = false;
    teclaAPresionada      = false;
    tiempoSiguienteOleada = INTERVALO_OLEADA_MIN + static_cast<float>(
                                QRandomGenerator::global()->bounded(INTERVALO_OLEADA_VARIACION));// como funcionan las oleadas?
}

Nivel2::~Nivel2()
{
    delete jugador1;
    delete jugador2;
    for (auto* f : flechas) delete f;
    flechas.clear();
}

void Nivel2::iniciar()
{
    delete jugador1;
    delete jugador2;

    jugador1 = new Jugador(JUGADOR_NIVEL2_ROBERT);
    jugador2 = new Jugador(JUGADOR_NIVEL2_TARGERYAN);

    jugador1->setVida(VIDA_ROBERT);//jugador 1
    jugador2->setVida(VIDA_RHAEGAR); //jugador 2
    jugador1->setPosicion(200.0f, 0.0f, 0.0f);
    jugador2->setPosicion(700.0f, 0.0f, 0.0f);
    jugador1->mirarHaciaDerecha(true);
    jugador2->mirarHaciaDerecha(false);

    fondoCargado = fondo.load(":/fondo-nevel2.png");
    pisoCargado  = piso.load(":/piso.png");

    finNivel              = false;
    iaActiva              = true;   // siempre arranca encendida
    tiempoEscudo1         = 0.0f;
    tiempoEscudo2         = 0.0f;
    velEmpuje1            = 0.0f;
    velEmpuje2            = 0.0f;
    bloqueando1           = false;
    cooldownAtaque1       = 0.0f;
    cooldownAtaque2       = 0.0f;
    tiempoDecisionIA      = 0.0f;
    timerPostAtaqueRobert = 0.0f;
    robertAcabaDeAtacar   = false;
    teclaDPresionada      = false;
    teclaAPresionada      = false;

    for (auto* f : flechas) delete f;
    flechas.clear();

    tiempoSiguienteOleada = INTERVALO_OLEADA_MIN + static_cast<float>(
                                QRandomGenerator::global()->bounded(INTERVALO_OLEADA_VARIACION));
}

void Nivel2::keyPresado(int key)
{
    if (key == Qt::Key_D) teclaDPresionada = true;
    if (key == Qt::Key_A) teclaAPresionada = true;
    if (key == Qt::Key_W) jugador1->saltar();
    if (key == Qt::Key_J) ataqueJugador();
    if (key == Qt::Key_K) bloqueoJugador(true);
}

void Nivel2::keySoltado(int key)
{
    if (key == Qt::Key_D) teclaDPresionada = false;
    if (key == Qt::Key_A) teclaAPresionada = false;
    if (key == Qt::Key_K) bloqueoJugador(false);
}

void Nivel2::procesarTeclas()
{
    if (teclaDPresionada)      jugador1->moverDerecha();
    else if (teclaAPresionada) jugador1->moverIzquierda();
    else                       jugador1->frenar();
}

void Nivel2::actualizar()
{
    if (!jugador1 || !jugador2) return;

    procesarTeclas();

    if (cooldownAtaque1 > 0.0f) cooldownAtaque1 -= dt;
    if (cooldownAtaque2 > 0.0f) cooldownAtaque2 -= dt;

    jugador1->actualizarNivel2(dt);
    jugador2->actualizarNivel2(dt);

    aplicarEmpuje();

    if (timerPostAtaqueRobert > 0.0f)
    {
        timerPostAtaqueRobert -= dt;
        if (robertAcabaDeAtacar)
        {
            Vector3 p1 = jugador1->getPosicion();
            Vector3 p2 = jugador2->getPosicion();
            float dist = p1.x - p2.x;
            if (dist < 0.0f) dist = -dist;
            if (dist > DIST_HUIDA)
            {
                ia.notificarPatronHitAndRun();
                robertAcabaDeAtacar = false;
            }
        }
    }
    else
    {
        robertAcabaDeAtacar = false;
    }

    // ---- IA: solo actualiza si esta activa ----
    if (iaActiva)
    {
        tiempoDecisionIA += dt;
        if (tiempoDecisionIA >= INTERVALO_DECISION_IA)
        {
            ia.notificarBloqueoRobert(bloqueando1);
            actualizarIA();
        }
    }

    limitarJugador(jugador1);
    limitarJugador(jugador2);

    if (jugador1->getEscudo() < ESCUDO_MAX)
    {
        tiempoEscudo1 += dt;
        if (tiempoEscudo1 >= intervaloRegeneracionEscudo(jugador1->getVida(), VIDA_ROBERT))
        {
            tiempoEscudo1 = 0.0f;
            jugador1->setEscudo(jugador1->getEscudo() + ESCUDO_RECUPERACION);
        }
    }
    else
    {
        tiempoEscudo1 = 0.0f;
    }

    if (jugador2->getEscudo() < ESCUDO_MAX)
    {
        tiempoEscudo2 += dt;
        if (tiempoEscudo2 >= intervaloRegeneracionEscudo(jugador2->getVida(), VIDA_RHAEGAR))
        {
            tiempoEscudo2 = 0.0f;
            jugador2->setEscudo(jugador2->getEscudo() + ESCUDO_RECUPERACION);
        }
    }
    else
    {
        tiempoEscudo2 = 0.0f;
    }

    tiempoSiguienteOleada -= dt;
    if (tiempoSiguienteOleada <= 0.0f)
    {
        generarOleadaFlechas();
        tiempoSiguienteOleada = INTERVALO_OLEADA_MIN + static_cast<float>(
                                    QRandomGenerator::global()->bounded(INTERVALO_OLEADA_VARIACION));
    }

    actualizarFlechas();
    comprobarColisionesFlechas();

    if (jugador1->getVida() <= 0.0f || jugador2->getVida() <= 0.0f)
        finNivel = true;
}

void Nivel2::ataqueJugador()
{
    if (!jugador1 || !jugador2) return;
    if (cooldownAtaque1 > 0.0f) return;

    // Activar animación de ataque en jugador1
    jugador1->setEstadoAnimacion(ANIM_ATAQUE);

    Vector3 p1 = jugador1->getPosicion();
    Vector3 p2 = jugador2->getPosicion();

    float diff    = p2.x - p1.x;
    float absDist = diff < 0.0f ? -diff : diff;

    if (absDist > 200.0f) return;  // rango ajustado al ancho visual del sprite (240px)

    cooldownAtaque1 = COOLDOWN_ATAQUE;

    float escudo       = jugador2->getEscudo();
    bool  bloqueandoIA = iaActiva && ia.quiereBloquear();

    if (bloqueandoIA && escudo > 0.0f)
        jugador2->setEscudo(escudo - DANIO_ESCUDO * 0.5f);
    else if (escudo > 0.0f)
        jugador2->setEscudo(escudo - DANIO_ESCUDO);
    else
        jugador2->setVida(jugador2->getVida() - DANIO_VIDA);

    ia.notificarGolpeRecibido();
    jugador2->setEstadoAnimacion(ANIM_GOLPE);   // animación de golpe recibido
    robertAcabaDeAtacar   = true;
    timerPostAtaqueRobert = TIEMPO_HUIDA;

    float dir          = (diff >= 0.0f) ? 1.0f : -1.0f;
    float masaTotal    = MASA_ROBERT + MASA_RHAEGAR;
    float factorEmpuje = bloqueandoIA ? 0.5f : 1.0f;

    velEmpuje2 += dir * FUERZA_EMPUJE * (MASA_ROBERT  / masaTotal) * factorEmpuje;
    velEmpuje1 -= dir * FUERZA_EMPUJE * (MASA_RHAEGAR / masaTotal) * factorEmpuje;

    if (velEmpuje1 >  MAX_VEL_EMPUJE) velEmpuje1 =  MAX_VEL_EMPUJE;
    if (velEmpuje1 < -MAX_VEL_EMPUJE) velEmpuje1 = -MAX_VEL_EMPUJE;
    if (velEmpuje2 >  MAX_VEL_EMPUJE) velEmpuje2 =  MAX_VEL_EMPUJE;
    if (velEmpuje2 < -MAX_VEL_EMPUJE) velEmpuje2 = -MAX_VEL_EMPUJE;
}

void Nivel2::bloqueoJugador(bool activo)
{
    bloqueando1 = activo;
    if (activo)
        jugador1->setEstadoAnimacion(ANIM_BLOQUEO);
    else
        jugador1->setEstadoAnimacion(ANIM_CAMINAR);
    if (!activo) jugador1->frenar();
}

void Nivel2::aplicarDanioDirecto(Jugador* objetivo)
{
    float escudo = objetivo->getEscudo();
    if (escudo > 0.0f)
        objetivo->setEscudo(escudo - DANIO_ESCUDO);
    else
        objetivo->setVida(objetivo->getVida() - DANIO_VIDA);
}

void Nivel2::aplicarEmpuje()
{
    float factor = 1.0f - FRICCION_EMPUJE * dt;
    if (factor < 0.0f) factor = 0.0f;

    velEmpuje1 *= factor;
    velEmpuje2 *= factor;

    if (std::abs(velEmpuje1) < UMBRAL_EMPUJE) velEmpuje1 = 0.0f;
    if (std::abs(velEmpuje2) < UMBRAL_EMPUJE) velEmpuje2 = 0.0f;

    if (jugador1)
    {
        Vector3 p = jugador1->getPosicion();
        p.x += velEmpuje1 * dt;
        jugador1->setPosicion(p.x, p.y, p.z);
    }
    if (jugador2)
    {
        Vector3 p = jugador2->getPosicion();
        p.x += velEmpuje2 * dt;
        jugador2->setPosicion(p.x, p.y, p.z);
    }
}

void Nivel2::actualizarIA()
{
    if (!jugador1 || !jugador2) return;

    float dtDecision = tiempoDecisionIA + dt;
    tiempoDecisionIA = 0.0f;

    float xAntesIA = jugador2->getPosicion().x;

    ia.actualizar(jugador2, jugador1,
                  jugador2->refVelY(),
                  jugador2->refEnSuelo(),
                  velEmpuje2, velEmpuje1,
                  cooldownAtaque2, dtDecision);

    bool iaSeMovio = std::abs(jugador2->getPosicion().x - xAntesIA) > 0.1f;

    if (velEmpuje1 >  MAX_VEL_EMPUJE) velEmpuje1 =  MAX_VEL_EMPUJE;
    if (velEmpuje1 < -MAX_VEL_EMPUJE) velEmpuje1 = -MAX_VEL_EMPUJE;
    if (velEmpuje2 >  MAX_VEL_EMPUJE) velEmpuje2 =  MAX_VEL_EMPUJE;
    if (velEmpuje2 < -MAX_VEL_EMPUJE) velEmpuje2 = -MAX_VEL_EMPUJE;

    // ── Sincronizar animación de la IA con su estado ────────
    // Solo cambiar si no está en medio de un golpe o ataque
    EstadoAnimacion animActual = jugador2->getEstadoAnimacion();
    bool animacionActiva = (animActual == ANIM_ATAQUE || animActual == ANIM_GOLPE);

    if (!animacionActiva)
    {
        if (ia.quiereBloquear())
            jugador2->setEstadoAnimacion(ANIM_BLOQUEO);
        else if (iaSeMovio)
            jugador2->setEstadoAnimacion(ANIM_CAMINAR);
    }

    (void)ia.quiereBloquear();
}

void Nivel2::limitarJugador(Jugador* j)
{
    Vector3 pos = j->getPosicion();
    if (pos.x < limiteIzquierdo) pos.x = limiteIzquierdo;
    if (pos.x > limiteDerecho)   pos.x = limiteDerecho;
    j->setPosicion(pos.x, pos.y, pos.z);
}

void Nivel2::generarOleadaFlechas()
{
    int cantidad = 1 + static_cast<int>(QRandomGenerator::global()->bounded(3));

    for (int i = 0; i < cantidad; i++)
    {
        float x  = limiteIzquierdo + static_cast<float>(
                      QRandomGenerator::global()->bounded(
                          static_cast<int>(limiteDerecho - limiteIzquierdo)));
        float y  = -30.0f;
        float vx = static_cast<float>(
                       QRandomGenerator::global()->bounded(
                           static_cast<int>(VELOCIDAD_FLECHA_X_MAX * 2.0f)))
                   - VELOCIDAD_FLECHA_X_MAX;
        float vy = VELOCIDAD_FLECHA_Y_MIN + static_cast<float>(
                       QRandomGenerator::global()->bounded(VELOCIDAD_FLECHA_Y_VARIACION));

        flechas.push_back(new Obstaculo(x, y, vx, vy, true));
    }
}

void Nivel2::actualizarFlechas()
{
    for (auto* f : flechas)
        f->actualizar(dt);

    auto it = flechas.begin();
    while (it != flechas.end())
    {
        if ((*it)->debeEliminarse()) { delete *it; it = flechas.erase(it); }
        else { ++it; }
    }
}

void Nivel2::comprobarColisionesFlechas()
{
    for (auto* f : flechas)
    {
        if (f->debeEliminarse()) continue;

        QRect rf = f->getRect();

        auto chequear = [&](Jugador* j)
        {
            if (!j) return;
            Vector3 p = j->getPosicion();
            QRect rj(static_cast<int>(p.x + HITBOX_X_MARGIN),
                     static_cast<int>(HITBOX_Y_BASE + p.y + HITBOX_Y_MARGIN),
                     static_cast<int>(HITBOX_W),
                     static_cast<int>(HITBOX_H));
            if (rf.intersects(rj))
            {
                float esc = j->getEscudo();
                if (esc > 0.0f) j->setEscudo(esc - DANIO_FLECHA);
                else            j->setVida(j->getVida() - DANIO_FLECHA);
                j->setEstadoAnimacion(ANIM_GOLPE);   // feedback visual de impacto
                f->desactivar();
            }
        };

        chequear(jugador1);
        if (!f->debeEliminarse()) chequear(jugador2);
    }
}

void Nivel2::renderizar(QPainter& painter) { (void)painter; }

bool     Nivel2::terminado()                  { return finNivel; }
float    Nivel2::getVelocidadMundo() const    { return 0.0f; }
void     Nivel2::manejarClick(int, int)       {}
bool     Nivel2::pidioSiguienteNivel() const  { return false; }
Jugador* Nivel2::getJugador()                 { return jugador1; }
Jugador* Nivel2::getJugador1()                { return jugador1; }
Jugador* Nivel2::getJugador2()                { return jugador2; }
bool     Nivel2::isFondoCargado() const       { return fondoCargado; }
bool     Nivel2::isPisoCargado()  const       { return pisoCargado; }
QPixmap  Nivel2::getFondo()       const       { return fondo; }
QPixmap  Nivel2::getPiso()        const       { return piso; }

void Nivel2::configurarMovimiento(float nuevoDt, float) { dt = nuevoDt; }
void Nivel2::aplicarDanio(Jugador*, Jugador*, bool) {}
