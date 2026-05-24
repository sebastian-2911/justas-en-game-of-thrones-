#include "graficos.h"
#include "jugador.h"
#include "nivel1.h"
#include "nivel2.h"

#include <QAudioOutput>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

float scrollMundo    = 0.0f;
float velocidadMundo = 400.0f;

Graficos::Graficos(QWidget* parent)
    : QWidget(parent),
    nivel(nullptr),
    timer(new QTimer(this)),
    fondoCargado(false),
    menuActivo(true),
    reproduciendo(false),
    mediaPlayer(nullptr),
    videoWidget(nullptr)
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

    if (mediaPlayer)
    {
        mediaPlayer->stop();
        delete mediaPlayer;
    }

    if (videoWidget)
        delete videoWidget;
}

// ── fondo ─────────────────────────────────────────────────────────────────────

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
    int w    = img.width();
    int h    = img.height();
    int zona = 150;

    for (int y = 0; y < zona; y++)
    {
        float alpha = (float)y / zona;

        for (int x = 0; x < w; x++)
        {
            QColor colorTop = img.pixelColor(x, y);
            QColor colorBot = img.pixelColor(x, h - zona + y);

            int r = colorBot.red()   * (1 - alpha) + colorTop.red()   * alpha;
            int g = colorBot.green() * (1 - alpha) + colorTop.green() * alpha;
            int b = colorBot.blue()  * (1 - alpha) + colorTop.blue()  * alpha;

            img.setPixelColor(x, y, QColor(r, g, b));
        }
    }

    fondo        = QPixmap::fromImage(img);
    fondoCargado = true;

    fondoMenu = QPixmap(":/menu-1.jpeg").scaled(
        1024,
        668,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );
}

// ── nivel 1 ───────────────────────────────────────────────────────────────────

void Graficos::iniciarNivel1(Nivel1::Dificultad dificultad)
{
    musicaMenu.stop();

    delete nivel;

    nivel = new Nivel1(dificultad);
    nivel->iniciar();

    scrollMundo    = 0.0f;
    velocidadMundo = nivel->getVelocidadMundo();

    menuActivo = false;
}

// ── cutscene + nivel 2 ────────────────────────────────────────────────────────

void Graficos::iniciarNivel2()
{
    reproducirCutscene();
}

void Graficos::reproducirCutscene()
{
    reproduciendo = true;
    timer->stop();

    setStyleSheet("background-color: black;");

    videoWidget = new QVideoWidget(this);
    videoWidget->setGeometry(0, 0, 1024, 668);
    videoWidget->show();
    videoWidget->raise();

    mediaPlayer = new QMediaPlayer(this);

    QAudioOutput* audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(1.0f);
    mediaPlayer->setAudioOutput(audioOutput);

    mediaPlayer->setVideoOutput(videoWidget);
    mediaPlayer->setSource(QUrl("qrc:/video nivel1.mp4"));

    connect(
        mediaPlayer,
        &QMediaPlayer::playbackStateChanged,
        this,
        &Graficos::onVideoTerminado
        );

    mediaPlayer->play();
}

void Graficos::onVideoTerminado(QMediaPlayer::PlaybackState state)
{
    if (state != QMediaPlayer::StoppedState)
        return;

    if (videoWidget)
    {
        videoWidget->hide();
        videoWidget->deleteLater();
        videoWidget = nullptr;
    }

    if (mediaPlayer)
    {
        mediaPlayer->deleteLater();
        mediaPlayer = nullptr;
    }

    setStyleSheet("background-color: black;");


    QTimer::singleShot(3000, this, [this]()
                       {
                           setStyleSheet("");
                           reproduciendo = false;
                           iniciarNivel2Real();
                       });
}
void Graficos::iniciarNivel2Real()
{
    delete nivel;

    nivel = new Nivel2();
    nivel->iniciar();

    scrollMundo    = 0.0f;
    velocidadMundo = nivel->getVelocidadMundo();

    menuActivo = false;

    timer->start(16);
}

// ── menú ──────────────────────────────────────────────────────────────────────

void Graficos::volverAlMenu()
{
    delete nivel;

    nivel        = nullptr;
    scrollMundo  = 0.0f;
    menuActivo   = true;

    musicaMenu.play();
}

// ── game loop ─────────────────────────────────────────────────────────────────

void Graficos::loop()
{
    if (menuActivo || reproduciendo)
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

// ── pintado ───────────────────────────────────────────────────────────────────

void Graficos::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (menuActivo)
    {
        dibujarMenu(painter);
        return;
    }

    if (reproduciendo)
        return; // QVideoWidget pinta el video

    if (fondoCargado)
    {
        int h      = fondo.height();
        int fondoY = static_cast<int>(std::fmod(scrollMundo, static_cast<float>(h)));

        painter.drawPixmap(0, fondoY - h, fondo);
        painter.drawPixmap(0, fondoY,     fondo);
    }

    if (nivel)
        nivel->renderizar(painter);
}

void Graficos::dibujarMenu(QPainter& painter)
{
    painter.drawPixmap(0, 0, fondoMenu);

    botonNormal  = QRect(362, 310, 300, 70);
    botonDificil = QRect(362, 420, 300, 70);

    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::NoPen);
    painter.drawRect(botonNormal);
    painter.drawRect(botonDificil);
}

// ── input ─────────────────────────────────────────────────────────────────────

void Graficos::keyPressEvent(QKeyEvent* event)
{
    if (reproduciendo)
    {
        if (event->key() == Qt::Key_Escape && mediaPlayer)
            mediaPlayer->stop();

        return;
    }

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
