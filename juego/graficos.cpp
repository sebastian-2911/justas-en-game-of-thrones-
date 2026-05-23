#include "graficos.h"
#include "jugador.h"
#include "nivel1.h"
#include "nivel2.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

float scrollMundo = 0.0f;
float velocidadMundo = 400.0f;

Graficos::Graficos(QWidget* parent)
    : QWidget(parent),
    nivel(nullptr),
    timer(new QTimer(this)),
    fondoCargado(false),
    menuActivo(true)
{
    setFixedSize(1024, 768);

    cargarFondo();

    connect(timer, &QTimer::timeout, this, &Graficos::loop);
    timer->start(16);

    setFocusPolicy(Qt::StrongFocus);
}

Graficos::~Graficos()
{
    delete nivel;
}

void Graficos::cargarFondo()
{
    QPixmap original;

    if (!original.load("fondo.jpeg"))
    {
        fondoCargado = false;
        return;
    }

    QPixmap escalado = original.scaled(
        1024,
        1200,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    QImage img = escalado.toImage();
    int w = img.width();
    int h = img.height();
    int zona = 150;

    for (int y = 0; y < zona; y++)
    {
        float alpha = (float)y / zona;

        for (int x = 0; x < w; x++)
        {
            QColor colorTop = img.pixelColor(x, y);
            QColor colorBot = img.pixelColor(x, h - zona + y);

            int r = colorBot.red() * (1 - alpha) + colorTop.red() * alpha;
            int g = colorBot.green() * (1 - alpha) + colorTop.green() * alpha;
            int b = colorBot.blue() * (1 - alpha) + colorTop.blue() * alpha;

            img.setPixelColor(x, y, QColor(r, g, b));
        }
    }

    fondo = QPixmap::fromImage(img);
    fondoCargado = true;
}

void Graficos::iniciarNivel1(Nivel1::Dificultad dificultad)
{
    // crear nivel
    delete nivel;

    nivel = new Nivel1(dificultad);
    nivel->iniciar();

    scrollMundo = 0.0f;
    velocidadMundo = nivel->getVelocidadMundo();

    menuActivo = false;
}

void Graficos::iniciarNivel2()
{
    delete nivel;

    nivel = new Nivel2();
    nivel->iniciar();

    scrollMundo = 0.0f;
    velocidadMundo = nivel->getVelocidadMundo();

    menuActivo = false;
}

void Graficos::volverAlMenu()
{
    // reiniciar juego
    delete nivel;

    nivel = nullptr;
    scrollMundo = 0.0f;
    menuActivo = true;
}

void Graficos::loop()
{
    // esperar menu
    if (menuActivo)
    {
        update();
        return;
    }

    float dt = 1.0f / 60.0f;
    scrollMundo += velocidadMundo * dt;

    nivel->configurarMovimiento(dt, scrollMundo);

    nivel->actualizar();

    if (nivel->pidioSiguienteNivel())
    {
        iniciarNivel2();
        return;
    }

    if (nivel->pidioReinicio())
    {
        volverAlMenu();
        return;
    }

    update();
}

void Graficos::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (fondoCargado)
    {
        int h = fondo.height();
        int fondoY = static_cast<int>(std::fmod(scrollMundo, static_cast<float>(h)));

        painter.drawPixmap(0, fondoY - h, fondo);
        painter.drawPixmap(0, fondoY, fondo);
    }

    if (menuActivo)
    {
        dibujarMenu(painter);
        return;
    }

    if (nivel)
        nivel->renderizar(painter);
}

void Graficos::dibujarMenu(QPainter& painter)
{
    // botones
    botonNormal = QRect(362, 310, 300, 60);
    botonDificil = QRect(362, 390, 300, 60);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 34, QFont::Bold));
    painter.drawText(270, 220, "Batalla del Tridente");

    painter.setFont(QFont("Arial", 20, QFont::Bold));
    painter.drawText(365, 275, "Selecciona dificultad");

    painter.setBrush(Qt::darkGray);
    painter.drawRect(botonNormal);
    painter.drawRect(botonDificil);

    painter.setPen(Qt::white);
    painter.drawText(460, 350, "NORMAL");
    painter.drawText(460, 430, "DIFICIL");
}

void Graficos::keyPressEvent(QKeyEvent* event)
{
    if (menuActivo || !nivel)
        return;

    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivel);

    // NIVEL 2
    if (nivel2)
    {
        if (event->key() == Qt::Key_A)
            nivel2->moverJugadorIzquierda();

        if (event->key() == Qt::Key_D)
            nivel2->moverJugadorDerecha();

        return;
    }

    // NIVEL 1
    Jugador* j = nivel->getJugador();

    if (!j)
        return;

    if (event->key() == Qt::Key_A)
        j->procesarInput('a');

    if (event->key() == Qt::Key_D)
        j->procesarInput('d');
}

void Graficos::mousePressEvent(QMouseEvent* event)
{
    int x = event->position().x();
    int y = event->position().y();

    // seleccionar dificultad
    if (menuActivo)
    {
        if (botonNormal.contains(x, y))
            iniciarNivel1(Nivel1::NORMAL);

        if (botonDificil.contains(x, y))
            iniciarNivel1(Nivel1::DIFICIL);

        return;
    }

    if (!nivel)
        return;

    nivel->manejarClick(
        x,
        y
        );
}
