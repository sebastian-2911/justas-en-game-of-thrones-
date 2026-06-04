#ifndef JUGADOR_H
#define JUGADOR_H
#include "entidad.h"
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QTransform>

enum TipoJugador
{
    JUGADOR_NIVEL1,
    JUGADOR_NIVEL2,
    JUGADOR_NIVEL2_TARGERYAN,
    JUGADOR_NIVEL2_ROBERT
};

enum EstadoAnimacion
{
    ANIM_CAMINAR,
    ANIM_ATAQUE,
    ANIM_BLOQUEO,
    ANIM_GOLPE,
    ANIM_SALTO,    // frame fijo, no pertenece a ninguna fila completa
    ANIM_TOTAL     // centinela (NO usar como índice de framesN2)
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

    // ── Getters ─────────────────────────────────────────
    float  getVida()      const;
    float  getEscudo()    const;
    float  getVelY()      const;
    float  getVelMovX()   const;
    bool   estaEnSuelo()  const;
    int    getCarril()    const;

    float& refVelY();
    bool&  refEnSuelo();

    // ── Setters ─────────────────────────────────────────
    void setVida(float valor);
    void setEscudo(float valor);
    void setVelY(float valor);
    void setEnSuelo(bool valor);
    void usarMovimientoLibre(bool estado);
    void mirarHaciaDerecha(bool derecha);

    // ── Control de animación (Nivel 2) ───────────────────
    void            setEstadoAnimacion(EstadoAnimacion estado);
    EstadoAnimacion getEstadoAnimacion() const;

private:
    // ── Helpers ──────────────────────────────────────────
    void cargarSpriteSheetNivel1();
    void cargarSpriteSheetRhaegar();
    void cargarSpriteSheetRobert();
    void avanzarFrame(float dt);
    void renderizarNivel1(QPainter& painter);
    void renderizarNivel2Sprite(QPainter& painter);
    bool esJugadorNivel2() const;
    bool usaSpriteTargeryan() const;
    bool usaSpriteRobert() const;

    // ── Estado ───────────────────────────────────────────
    TipoJugador     tipoJugador;

    int   carril;
    bool  movimientoLibre;

    float vida;
    float escudo;
    float tiempoInvulnerable;

    float velMovX;
    float velY;
    bool  enSuelo;

    Vector3 hitbox;

    // ── Dirección de cara ────────────────────────────────
    bool mirandoDerecha;   // true = sprite normal, false = espejado

    // ── Animación Nivel 1 ────────────────────────────────
    static const int TOTAL_FRAMES_N1 = 4;
    QPixmap framesN1[TOTAL_FRAMES_N1];

    // ── Animación Nivel 2 (sprite sheet Targeryan) ───────
    // ANIM_TOTAL - 1 porque ANIM_SALTO es frame fijo aparte
    // hoja de sprites nivel 2
    // Rhaegar: 1533x972, 7 cols x 4 filas, celda 219x243
    static constexpr int SPRITE_COLS_RHAEGAR   = 7;
    static constexpr int SPRITE_CELL_W_RHAEGAR = 219;
    static constexpr int SPRITE_CELL_H_RHAEGAR = 243;

    // Robert: 1536x1024, 8 cols x 4 filas, celda 192x256
    static constexpr int SPRITE_COLS_ROBERT    = 8;
    static constexpr int SPRITE_CELL_W_ROBERT  = 192;
    static constexpr int SPRITE_CELL_H_ROBERT  = 256;

    static constexpr int SPRITE_ANIM_ROWS = 4;

    // graficos rhaegar
    QPixmap spriteSheetRhaegar;
    QPixmap framesRhaegar      [SPRITE_ANIM_ROWS][7];   // 7 cols
    QPixmap framesRhaegarMirror[SPRITE_ANIM_ROWS][7];   // espejados

    QPixmap frameSaltoRhaegar;
    QPixmap frameSaltoRhaegarMirror;

    // graficos robert
    QPixmap spriteSheetRobert;

    QPixmap framesRobert         [SPRITE_ANIM_ROWS][8];   // 8 cols
    QPixmap framesRobertMirror   [SPRITE_ANIM_ROWS][8];   // espejados

    QPixmap frameSaltoRobert;
    QPixmap frameSaltoRobertMirror;

    EstadoAnimacion estadoAnim;
    int             frameActual;
    float           tiempoFrame;

    // ── Constantes ───────────────────────────────────────
    static constexpr float DURACION_FRAME   = 0.14f;
    static constexpr float VEL_MOV          = 310.0f;
    static constexpr float FUERZA_SALTO     = -620.0f;
    static constexpr float GRAVEDAD         = 1800.0f;


    const float carriles[3] = { -1.3f, 0.0f, 1.0f };
};

#endif // JUGADOR_H