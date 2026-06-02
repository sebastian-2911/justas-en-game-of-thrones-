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
    void  iniciar()                                    override;
    void  actualizar()                                 override;
    void  renderizar(QPainter& painter)                override;
    bool  terminado()                                  override;
    void  configurarMovimiento(float dt, float scroll) override;
    float getVelocidadMundo()                    const override;
    void  manejarClick(int x, int y)                   override;
    bool  pidioSiguienteNivel()                  const override;
    Jugador* getJugador()                              override;
    Jugador* getJugador1();
    Jugador* getJugador2();
    void keyPresado(int key);
    void keySoltado(int key);
    bool    isFondoCargado() const;
    bool    isPisoCargado()  const;
    QPixmap getFondo()       const;
    QPixmap getPiso()        const;
    const std::vector<Obstaculo*>& getFlechas() const { return flechas; }
    void ataqueJugador();
    void bloqueoJugador(bool activo);
    void setIAActiva(bool activo) { iaActiva = activo; }
    bool getIAActiva()      const { return iaActiva; }
private:
    Jugador* jugador1;
    Jugador* jugador2;
    IA ia;
    QPixmap fondo;
    QPixmap piso;
    bool    fondoCargado;
    bool    pisoCargado;
    bool    finNivel;
    bool    iaActiva;
    float   dt;
    float   tiempoEscudo1;
    float   tiempoEscudo2;
    float   limiteIzquierdo;
    float   limiteDerecho;
    float   velEmpuje1;
    float   velEmpuje2;
    bool    bloqueando1;
    float   cooldownAtaque1;
    float   cooldownAtaque2;
    float   tiempoDecisionIA;
    float   timerPostAtaqueRobert;
    bool    robertAcabaDeAtacar;
    std::vector<Obstaculo*> flechas;
    float   tiempoSiguienteOleada;
    bool teclaDPresionada;
    bool teclaAPresionada;
    static constexpr float MASA_ROBERT          =  2.0f;
    static constexpr float MASA_RHAEGAR         =  1.0f;
    static constexpr float ESCUDO_MAX           = 100.0f;
    static constexpr float ESCUDO_RECUPERACION  =  20.0f;
    static constexpr float MAX_VEL_EMPUJE       = 1200.0f;
    static constexpr float FRICCION_EMPUJE      =   12.0f;
    static constexpr float UMBRAL_EMPUJE        =    8.0f;
    static constexpr float FUERZA_EMPUJE        = 2000.0f;
    static constexpr float DANIO_ESCUDO         =   25.0f;
    static constexpr float DANIO_VIDA           =   30.0f;
    static constexpr float DANIO_FLECHA         =   10.0f;
    static constexpr float COOLDOWN_ATAQUE      =   0.22f;
    static constexpr float HITBOX_W             =   56.0f;
    static constexpr float HITBOX_H             =   96.0f;
    static constexpr float HITBOX_X_MARGIN      =    4.0f;
    static constexpr float HITBOX_Y_BASE        =  450.0f;
    static constexpr float HITBOX_Y_MARGIN      =    4.0f;
    static constexpr float RANGO_ATAQUE         =   30.0f;
    static constexpr float TIEMPO_HUIDA         =    0.5f;
    static constexpr float DIST_HUIDA           =  120.0f;
    static constexpr float INTERVALO_DECISION_IA = 0.16f;
    void procesarTeclas();
    void actualizarIA();
    void limitarJugador(Jugador* j);
    void aplicarEmpuje();
    void aplicarDanioDirecto(Jugador* objetivo);
    void aplicarDanio(Jugador* atacante, Jugador* defensor, bool bloqueando);
    void generarOleadaFlechas();
    void actualizarFlechas();
    void comprobarColisionesFlechas();
};
#endif // NIVEL2_H
