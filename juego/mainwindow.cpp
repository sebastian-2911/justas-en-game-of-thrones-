#include "mainwindow.h"
#include "jugador.h"
#include "nivel1.h"
#include "nivel2.h"
#include "./ui_mainwindow.h"

#include <QAudioOutput>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

float scrollMundo    = 0.0f;
float velocidadMundo = 400.0f;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    nivel(nullptr),
    timer(new QTimer(this)),
    fondoCargado(false),
    menuActivo(true),
    reproduciendo(false),
    mediaPlayer(nullptr),
    videoWidget(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Batalla del Tridente - copia");
    setFixedSize(1024, 768);

    cargarFondo();

    musicaMenu.setSource(QUrl("qrc:/musica-menu.wav"));
    musicaMenu.setLoopCount(QSoundEffect::Infinite);
    musicaMenu.setVolume(0.5f);
    musicaMenu.play();

    connect(timer, &QTimer::timeout, this, &MainWindow::loop);
    timer->start(16);

    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::cargarFondo()
{
    QPixmap original;

    if (!original.load(":/fondo.jpeg"))
    {
        fondoCargado = false;
        return;
    }

    QPixmap escalado = original.scaled(
        1024, 1200,
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
        1024, 668,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );
}

// ── nivel 1 ───────────────────────────────────────────────────────────────────

void MainWindow::iniciarNivel1(Nivel1::Dificultad dificultad)
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

void MainWindow::iniciarNivel2()
{
    reproducirCutscene();
}

void MainWindow::reproducirCutscene()
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
    audioOutput->setVolume(0.8f);
    mediaPlayer->setAudioOutput(audioOutput);

    mediaPlayer->setVideoOutput(videoWidget);
    mediaPlayer->setSource(QUrl("qrc:/video nivel1.mp4"));

    connect(
        mediaPlayer,
        &QMediaPlayer::playbackStateChanged,
        this,
        &MainWindow::onVideoTerminado
        );

    mediaPlayer->play();
}

void MainWindow::onVideoTerminado(QMediaPlayer::PlaybackState state)
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

void MainWindow::iniciarNivel2Real()
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

void MainWindow::volverAlMenu()
{
    delete nivel;

    nivel       = nullptr;
    scrollMundo = 0.0f;
    menuActivo  = true;

    musicaMenu.play();
}

// ── game loop ─────────────────────────────────────────────────────────────────

void MainWindow::loop()
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

// ── renderizar nivel 2 ────────────────────────────────────────────────────────

void MainWindow::renderizarNivel2(QPainter& painter)
{
    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivel);
    if (!nivel2) return;

    if (nivel2->isFondoCargado())
        painter.drawPixmap(0, 0, 1024, 768, nivel2->getFondo());

    if (nivel2->isPisoCargado())
        painter.drawPixmap(0, 550, 1024, 220, nivel2->getPiso());

    // ── declaración única de j1 y j2 ─────────────────────────────────────────
    Jugador* j1 = nivel2->getJugador1();
    Jugador* j2 = nivel2->getJugador2();

    // ── dibujar jugadores ─────────────────────────────────────────────────────
    painter.setPen(Qt::NoPen);

    if (j1)
    {
        Vector3 pos = j1->getPosicion();
        int screenX = static_cast<int>(pos.x);
        int screenY = 450 + static_cast<int>(pos.y);

        painter.setBrush(Qt::blue);
        painter.drawRect(screenX, screenY, 60, 100);
    }

    if (j2)
    {
        Vector3 pos = j2->getPosicion();
        int screenX = static_cast<int>(pos.x);
        int screenY = 450 + static_cast<int>(pos.y);

        painter.setBrush(Qt::red);
        painter.drawRect(screenX, screenY, 60, 100);
    }

    // ── mensaje fin de nivel ──────────────────────────────────────────────────
    if (nivel2->terminado())
    {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 40, QFont::Bold));

        if (j1 && j1->getVida() <= 0)
            painter.drawText(330, 200, "GANA IA");
        else
            painter.drawText(300, 200, "GANASTE");
    }

    // ── barras HUD superiores ─────────────────────────────────────────────────
    const int BARRA_W  = 380;
    const int BARRA_H  = 16;
    const int BARRA_Y1 = 20;
    const int BARRA_Y2 = 42;
    const int MARGEN   = 20;

    painter.setPen(Qt::NoPen);

    // --- jugador 1 (izquierda) ---
    if (j1)
    {
        painter.setBrush(QColor(60, 60, 60));
        painter.drawRect(MARGEN, BARRA_Y1, BARRA_W, BARRA_H);
        painter.drawRect(MARGEN, BARRA_Y2, BARRA_W, BARRA_H);

        int vidaW1 = static_cast<int>(j1->getVida() / 100.0f * BARRA_W);
        painter.setBrush(QColor(200, 30, 30));
        painter.drawRect(MARGEN, BARRA_Y1, vidaW1, BARRA_H);

        int escudoW1 = static_cast<int>(j1->getEscudo() / 100.0f * BARRA_W);
        painter.setBrush(QColor(30, 100, 220));
        painter.drawRect(MARGEN, BARRA_Y2, escudoW1, BARRA_H);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(MARGEN, BARRA_Y1 - 4, "JUGADOR");
        painter.setPen(Qt::NoPen);
    }

    // --- jugador 2 / IA (derecha) ---
    if (j2)
    {
        int xDerecha = 1024 - MARGEN - BARRA_W;

        painter.setBrush(QColor(60, 60, 60));
        painter.drawRect(xDerecha, BARRA_Y1, BARRA_W, BARRA_H);
        painter.drawRect(xDerecha, BARRA_Y2, BARRA_W, BARRA_H);

        int vidaW2   = static_cast<int>(j2->getVida()   / 100.0f * BARRA_W);
        int escudoW2 = static_cast<int>(j2->getEscudo() / 100.0f * BARRA_W);

        painter.setBrush(QColor(200, 30, 30));
        painter.drawRect(1024 - MARGEN - vidaW2, BARRA_Y1, vidaW2, BARRA_H);

        painter.setBrush(QColor(30, 100, 220));
        painter.drawRect(1024 - MARGEN - escudoW2, BARRA_Y2, escudoW2, BARRA_H);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        QFontMetrics fm(painter.font());
        int textoW = fm.horizontalAdvance("IA");
        painter.drawText(1024 - MARGEN - textoW, BARRA_Y1 - 4, "IA");
        painter.setPen(Qt::NoPen);
    }
}

// ── pintado ───────────────────────────────────────────────────────────────────

void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (menuActivo)
    {
        dibujarMenu(painter);
        return;
    }

    if (reproduciendo)
        return;

    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivel);

    if (!nivel2 && fondoCargado)
    {
        int h      = fondo.height();
        int fondoY = static_cast<int>(std::fmod(scrollMundo, static_cast<float>(h)));

        painter.drawPixmap(0, fondoY - h, fondo);
        painter.drawPixmap(0, fondoY,     fondo);
    }

    if (nivel)
    {
        if (nivel2)
            renderizarNivel2(painter);
        else
            nivel->renderizar(painter);
    }
}

void MainWindow::dibujarMenu(QPainter& painter)
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

void MainWindow::keyPressEvent(QKeyEvent* event)
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
        // ── movimiento ────────────────────────────────────────────────────────
        if (event->key() == Qt::Key_A)
            nivel2->moverJugadorIzquierda();

        if (event->key() == Qt::Key_D)
            nivel2->moverJugadorDerecha();

        // ── salto con W ───────────────────────────────────────────────────────
        if (event->key() == Qt::Key_W)
            nivel2->saltoJugador();

        // ── ataque con Z ──────────────────────────────────────────────────────
        if (event->key() == Qt::Key_Z)
            nivel2->ataqueJugador();

        // ── bloqueo con X (mantener presionado) ───────────────────────────────
        if (event->key() == Qt::Key_X)
            nivel2->bloqueoJugador(true);

        return;
    }

    // nivel 1
    Jugador* j = nivel->getJugador();
    if (!j) return;

    if (event->key() == Qt::Key_A)
        j->procesarInput('a');

    if (event->key() == Qt::Key_D)
        j->procesarInput('d');
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (menuActivo || !nivel) return;

    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivel);
    if (!nivel2) return;

    // Soltar X desactiva el bloqueo
    if (event->key() == Qt::Key_X)
        nivel2->bloqueoJugador(false);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
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
