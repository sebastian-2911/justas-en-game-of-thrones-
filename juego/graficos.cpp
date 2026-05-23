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

    musicaMenu.setSource(QUrl("qrc:/musica-menu.wav"));
    musicaMenu.setLoopCount(QSoundEffect::Infinite);
    musicaMenu.setVolume(0.5f);
    musicaMenu.play();

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

    if (!original.load(":/fondo.jpeg"))
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

    fondoMenu = QPixmap(":/menu-1.jpeg").scaled(
        1024,//x
        668,//y
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );
}

void Graficos::iniciarNivel1(Nivel1::Dificultad dificultad)
{
    musicaMenu.stop();

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
    delete nivel;

    nivel = nullptr;
    scrollMundo = 0.0f;
    menuActivo = true;

    musicaMenu.play();
}

void Graficos::loop()
{
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

    if (menuActivo)
    {
        dibujarMenu(painter);
        return;
    }

    if (fondoCargado)
    {
        int h = fondo.height();
        int fondoY = static_cast<int>(std::fmod(scrollMundo, static_cast<float>(h)));

        painter.drawPixmap(0, fondoY - h, fondo);
        painter.drawPixmap(0, fondoY, fondo);
    }

    if (nivel)
        nivel->renderizar(painter);
}

void Graficos::dibujarMenu(QPainter& painter)
{
    painter.drawPixmap(0, 0, fondoMenu);

    botonNormal = QRect(362, 310, 300, 70);
    botonDificil = QRect(362, 420, 300, 70);

    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::NoPen);
    painter.drawRect(botonNormal);
    painter.drawRect(botonDificil);
}
void Graficos::keyPressEvent(QKeyEvent* event)
{
    if (menuActivo || !nivel)
        return;

    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivel);

    if (nivel2)
    {
        if (event->key() == Qt::Key_A)
            nivel2->moverJugadorIzquierda();

        if (event->key() == Qt::Key_D)
            nivel2->moverJugadorDerecha();

        return;
    }

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

    nivel->manejarClick(x, y);
}
