#include "jugador.h"

// ════════════════════════════════════════════════════════════
//  Constructor
// ════════════════════════════════════════════════════════════
Jugador::Jugador(TipoJugador tipo)
{
    tipoJugador        = tipo;
    carril             = 1;
    movimientoLibre    = false;
    tiempoInvulnerable = 0.0f;
    hitbox             = {0.8f, 1.2f, 0.0f};
    posicion           = {0.0f, 0.0f, 0.0f};
    vida               = 100.0f;
    escudo             = 100.0f;
    velMovX            = 0.0f;
    velY               = 0.0f;
    enSuelo            = true;
    frameActual        = 0;
    tiempoFrame        = 0.0f;
    estadoAnim         = ANIM_CAMINAR;
    mirandoDerecha     = true;   // por defecto mira a la derecha

    if (tipoJugador == JUGADOR_NIVEL1)
        cargarSpriteSheetNivel1();
    else if (usaSpriteTargeryan())
        cargarSpriteSheetRhaegar();
    else if (usaSpriteRobert())
        cargarSpriteSheetRobert();
}

// ════════════════════════════════════════════════════════════
//  Carga de recursos
// ════════════════════════════════════════════════════════════
void Jugador::cargarSpriteSheetNivel1()
{
    QPixmap sheet;
    sheet.load(":/robert nivel1.png");

    if (!sheet.isNull())
    {
        framesN1[0] = sheet.copy(0,   0, 115, 330);
        framesN1[1] = sheet.copy(115, 0, 127, 330);
        framesN1[2] = sheet.copy(242, 0, 129, 330);
        framesN1[3] = sheet.copy(371, 0, 132, 330);
    }
}
void Jugador::actualizar()
{
    if (tipoJugador != JUGADOR_NIVEL1) return;

    if (!movimientoLibre)
        posicion.x = carriles[carril];

    if (tiempoInvulnerable > 0.0f)
        tiempoInvulnerable -= 0.016f;

    avanzarFrame(0.016f);
}

void Jugador::cargarSpriteSheetRhaegar()
{
    spriteSheetRhaegar.load(":/spritesTargeryan.png");

    if (spriteSheetRhaegar.isNull())
        return;

    for (int anim = 0; anim < SPRITE_ANIM_ROWS; anim++)
    {
        for (int col = 0; col < SPRITE_COLS_RHAEGAR; col++)
        {
            int x = col * SPRITE_CELL_W_RHAEGAR;
            int y = anim * SPRITE_CELL_H_RHAEGAR;

            QPixmap original =
                spriteSheetRhaegar.copy(
                    x, y,
                    SPRITE_CELL_W_RHAEGAR,
                    SPRITE_CELL_H_RHAEGAR
                    );

            framesRhaegar[anim][col] = original;
            framesRhaegarMirror[anim][col] =
                original.transformed(QTransform().scale(-1.0, 1.0));
        }
    }

    frameSaltoRhaegar       = framesRhaegar[0][3];
    frameSaltoRhaegarMirror = framesRhaegarMirror[0][3];
}
void Jugador::cargarSpriteSheetRobert()
{
    spriteSheetRobert.load(":/spritesRobert.png");

    if (spriteSheetRobert.isNull())
        return;

    for (int anim = 0; anim < SPRITE_ANIM_ROWS; anim++)
    {
        for (int col = 0; col < SPRITE_COLS_ROBERT; col++)
        {
            int x = col * SPRITE_CELL_W_ROBERT;
            int y = anim * SPRITE_CELL_H_ROBERT;

            QPixmap original =
                spriteSheetRobert.copy(
                    x, y,
                    SPRITE_CELL_W_ROBERT,
                    SPRITE_CELL_H_ROBERT
                    );

            framesRobert[anim][col] = original;
            framesRobertMirror[anim][col] =
                original.transformed(QTransform().scale(-1.0, 1.0));
        }
    }

    frameSaltoRobert       = framesRobert[0][3];
    frameSaltoRobertMirror = framesRobertMirror[0][3];
}
//════════════════════════════════════════════════════════════
//  Actualización
// ════════════════════════════════════════════════════════════

void Jugador::actualizarNivel2(float dt)
{
    if (!esJugadorNivel2()) return;

    // ── Dirección: actualizar según velocidad horizontal ──
    if      (velMovX > 0.0f) mirandoDerecha = true;
    else if (velMovX < 0.0f) mirandoDerecha = false;

    // ── Movimiento horizontal ──────────────────────────────
    posicion.x += velMovX * dt;

    // ── Gravedad / salto ───────────────────────────────────
    velY       += GRAVEDAD * dt;
    posicion.y += velY * dt;

    if (posicion.y >= 0.0f)
    {
        posicion.y = 0.0f;
        velY       = 0.0f;
        enSuelo    = true;
    }
    else
    {
        enSuelo = false;
    }

    // ── Estado de animación automático ────────────────────
    // Prioridad: salto > acciones activas > movimiento > idle
    if (!enSuelo)
    {
        estadoAnim = ANIM_SALTO;   // en el aire: frame fijo de salto
    }
    else if (estadoAnim == ANIM_CAMINAR ||
             estadoAnim == ANIM_SALTO)
    {
        if (velMovX != 0.0f)
            estadoAnim = ANIM_CAMINAR;
    }

    // Al terminar ataque o golpe, volver a caminar (frame 0 = reposo)
    int colsActuales = (tipoJugador == JUGADOR_NIVEL2_TARGERYAN)
                           ? SPRITE_COLS_RHAEGAR : SPRITE_COLS_ROBERT;
    if ((estadoAnim == ANIM_ATAQUE || estadoAnim == ANIM_GOLPE)
        && frameActual == colsActuales - 1)
    {
        estadoAnim  = ANIM_CAMINAR;
        frameActual = 0;
        tiempoFrame = 0.0f;
    }

    avanzarFrame(dt);

}

// ── Avance de frame (compartido) ──────────────────────────────
void Jugador::avanzarFrame(float dt)
{
    // El estado ANIM_SALTO usa frame fijo, no animar
    if (estadoAnim == ANIM_SALTO)
        return;

    if (esJugadorNivel2() &&
        estadoAnim == ANIM_CAMINAR &&
        velMovX == 0.0f)
    {
        return;
    }

    tiempoFrame += dt;
    if (tiempoFrame >= DURACION_FRAME)
    {
        tiempoFrame = 0.0f;
        if (tipoJugador == JUGADOR_NIVEL1)
            frameActual = (frameActual + 1) % TOTAL_FRAMES_N1;
        else
            frameActual = (frameActual + 1) %
                          (tipoJugador == JUGADOR_NIVEL2_TARGERYAN
                               ? SPRITE_COLS_RHAEGAR : SPRITE_COLS_ROBERT);
    }
}

// ════════════════════════════════════════════════════════════
//  Renderizado
// ════════════════════════════════════════════════════════════
void Jugador::renderizar(QPainter& painter)
{
    if (tipoJugador == JUGADOR_NIVEL1)
        renderizarNivel1(painter);
    else
        renderizarNivel2Sprite(painter);
}

void Jugador::renderizarNivel1(QPainter& painter)
{
    int screenX = 512 + static_cast<int>(posicion.x * 180) - 140;
    int screenY = 400;

    setPosicionReal(static_cast<float>(screenX), static_cast<float>(screenY));

    if (!framesN1[frameActual].isNull())
    {
        painter.drawPixmap(screenX, screenY, 150, 250, framesN1[frameActual]);

        if (tiempoInvulnerable > 0.0f)
        {
            painter.setOpacity(0.35);
            painter.fillRect(screenX, screenY, 150, 250, Qt::yellow);
            painter.setOpacity(1.0);
        }
    }
    else
    {
        painter.setBrush(tiempoInvulnerable > 0.0f ? Qt::yellow : Qt::blue);
        painter.drawRect(screenX, screenY, 50, 50);
    }
}

void Jugador::renderizarNivel2Sprite(QPainter& painter)
{
    int screenX = static_cast<int>(posicion.x);
    int screenY = 450 + static_cast<int>(posicion.y);

    setPosicionReal(static_cast<float>(screenX), static_cast<float>(screenY));

    const int renderW = 240;
    const int renderH = 260;

    // ── Seleccionar frame correcto según estado y dirección ──
    QPixmap frame;

    //saltos

    if (estadoAnim == ANIM_SALTO)
    {
        if (tipoJugador == JUGADOR_NIVEL2_TARGERYAN)
        {
            frame = mirandoDerecha
                        ? frameSaltoRhaegar
                        : frameSaltoRhaegarMirror;
        }
        else
        {
            frame = mirandoDerecha
                        ? frameSaltoRobert
                        : frameSaltoRobertMirror;
        }
    }

    // ─────────────────────────────────────────────
    // Caminar / Ataque / Golpe / Bloqueo
    // ─────────────────────────────────────────────
    else
    {
        int animIdx = static_cast<int>(estadoAnim);

        if (animIdx >= 0 &&
            animIdx < SPRITE_ANIM_ROWS)
        {
            if (tipoJugador == JUGADOR_NIVEL2_TARGERYAN)
            {
                frame = mirandoDerecha
                            ? framesRhaegar[animIdx][frameActual]
                            : framesRhaegarMirror[animIdx][frameActual];
            }
            else
            {
                frame = mirandoDerecha
                            ? framesRobert[animIdx][frameActual]
                            : framesRobertMirror[animIdx][frameActual];
            }
        }
    }
    if (!frame.isNull())
    {
        painter.drawPixmap(screenX, screenY, renderW, renderH, frame);

        if (tiempoInvulnerable > 0.0f)
        {
            painter.setOpacity(0.30);
            painter.fillRect(screenX, screenY, renderW, renderH, Qt::yellow);
            painter.setOpacity(1.0);
        }
    }
    else
    {
        painter.setBrush(Qt::red);
        painter.drawRect(screenX, screenY, renderW, renderH);
    }
}

// ════════════════════════════════════════════════════════════
//  Input y movimiento
// ════════════════════════════════════════════════════════════
void Jugador::procesarInput(char tecla)
{
    if      (tecla == 'a' || tecla == 'A') moverIzquierda();
    else if (tecla == 'd' || tecla == 'D') moverDerecha();
}

void Jugador::moverIzquierda()
{
    if (tipoJugador == JUGADOR_NIVEL1) { if (carril > 0) carril--; return; }
    velMovX = -VEL_MOV;
}

void Jugador::moverDerecha()
{
    if (tipoJugador == JUGADOR_NIVEL1) { if (carril < 2) carril++; return; }
    velMovX = VEL_MOV;
}

void Jugador::frenar()
{
    velMovX = 0.0f;
}

void Jugador::saltar()
{
    if (!enSuelo) return;
    velY    = FUERZA_SALTO;
    enSuelo = false;
}

// ════════════════════════════════════════════════════════════
//  Colisión
// ════════════════════════════════════════════════════════════
bool Jugador::colisiona(Jugador* otro)
{
    if (!otro) return false;
    QRect r1(static_cast<int>(posicion.x), static_cast<int>(posicion.y), 120, 220);
    QRect r2(static_cast<int>(otro->posicion.x), static_cast<int>(otro->posicion.y), 120, 220);
    return r1.intersects(r2);
}

// ════════════════════════════════════════════════════════════
//  Getters / Setters
// ════════════════════════════════════════════════════════════
float  Jugador::getVida()     const { return vida; }
float  Jugador::getEscudo()   const { return escudo; }
float  Jugador::getVelY()     const { return velY; }
float  Jugador::getVelMovX()  const { return velMovX; }
bool   Jugador::estaEnSuelo() const { return enSuelo; }
int    Jugador::getCarril()   const { return carril; }
float& Jugador::refVelY()           { return velY; }
bool&  Jugador::refEnSuelo()        { return enSuelo; }

void Jugador::setVida(float valor)
{
    vida = valor;
    if (vida < 0.0f) vida = 0.0f;
}

void Jugador::setEscudo(float valor)
{
    escudo = valor;
    if (escudo > 100.0f) escudo = 100.0f;
    if (escudo < 0.0f)   escudo = 0.0f;
}

void Jugador::setVelY(float valor)   { velY = valor; }
void Jugador::setEnSuelo(bool valor) { enSuelo = valor; }

void Jugador::usarMovimientoLibre(bool estado)
{
    movimientoLibre = estado;
}

void Jugador::mirarHaciaDerecha(bool derecha)
{
    mirandoDerecha = derecha;
}

void Jugador::setEstadoAnimacion(EstadoAnimacion estado)
{
    if (estadoAnim == estado) return;
    estadoAnim  = estado;
    frameActual = 0;
    tiempoFrame = 0.0f;
}

EstadoAnimacion Jugador::getEstadoAnimacion() const
{
    return estadoAnim;
}

bool Jugador::esJugadorNivel2() const
{
    return tipoJugador == JUGADOR_NIVEL2
           || tipoJugador == JUGADOR_NIVEL2_TARGERYAN
           || tipoJugador == JUGADOR_NIVEL2_ROBERT;
}

bool Jugador::usaSpriteTargeryan() const
{
    return tipoJugador == JUGADOR_NIVEL2_TARGERYAN;
}
bool Jugador::usaSpriteRobert() const
{
    return tipoJugador == JUGADOR_NIVEL2_ROBERT;
}