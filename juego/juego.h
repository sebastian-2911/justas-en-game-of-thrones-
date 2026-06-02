#ifndef JUEGO_H
#define JUEGO_H

#include "nivel.h"
#include "nivel1.h"
#include "nivel2.h"

class Juego
{
public:
    enum ResultadoActualizacion
    {
        SinCambios,
        VictoriaNivel2,
        Derrota,
        SiguienteNivel
    };

    Juego();
    ~Juego();

    void iniciarNivel1(Nivel1::Dificultad dificultad);
    void iniciarNivel2();
    void limpiarNivel();

    ResultadoActualizacion actualizar(float dt, float scrollMundo);
    void manejarTeclaPresionada(int key);
    void manejarTeclaSoltada(int key);
    void manejarClick(int x, int y);

    bool estaEjecutando() const;
    bool tieneNivel() const;
    float getVelocidadMundo() const;
    Nivel* getNivel();
    Nivel2* getNivel2();

private:
    void cambiarNivel(Nivel* nuevoNivel);
    bool jugadorPerdioNivel1() const;
    bool jugadorPerdioNivel2() const;
    bool jugadorGanoNivel2() const;

    Nivel* nivelActual;
    bool ejecutando;
};

#endif
