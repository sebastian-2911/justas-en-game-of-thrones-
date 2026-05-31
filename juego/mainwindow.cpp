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
    mostrandoCargaN1(false),
    temporizadorCargaN1(0.0f),
    dificultadPendiente(Nivel1::NORMAL),
    mostrandoEntreNiveles(false),
    temporizadorEntreNiveles(0.0f),
    mostrandoCargaN2(false),
    temporizadorCargaN2(0.0f),
    mostrandoPerdedor(false),
    temporizadorPerdedor(0.0f),
    mostrandoVideoFinal(false),
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

    musicaNivel1.setSource(QUrl("qrc:/cancion nivel1.wav"));
    musicaNivel1.setLoopCount(QSoundEffect::Infinite);
    musicaNivel1.setVolume(0.5f);

    musicaNivel2.setSource(QUrl("qrc:/cancion n2.wav"));
    musicaNivel2.setLoopCount(QSoundEffect::Infinite);
    musicaNivel2.setVolume(0.5f);

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

    fondoMenu = QPixmap(":/menu-1.png").scaled(
        1024, 668,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    pantallaCargaN1 = QPixmap(":/pantalla de carga n1.png").scaled(
        1024, 668,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    pantallaCargaEntreNiveles = QPixmap(":/carga entreniveles.png").scaled(
        1024, 668,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    pantallaCargaN2 = QPixmap(":/pantalla de carga n2.png").scaled(
        1024, 668,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    pantallaPerdedor = QPixmap(":/perdedor.png").scaled(
        1024, 668,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );
}

// ─── Helpers para detectar derrota/victoria en cada nivel

static bool jugadorPerdioNivel1(Nivel* nivel)
{
    Nivel1* n1 = dynamic_cast<Nivel1*>(nivel);
    if (!n1) return false;
    return n1->pidioReinicio() && !n1->pidioSiguienteNivel();
}

static bool jugadorPerdioNivel2(Nivel* nivel)
{
    Nivel2* n2 = dynamic_cast<Nivel2*>(nivel);
    if (!n2 || !n2->terminado()) return false;
    Jugador* j1 = n2->getJugador1();
    return j1 && j1->getVida() <= 0;
}

static bool jugadorGanoNivel2(Nivel* nivel)
{
    Nivel2* n2 = dynamic_cast<Nivel2*>(nivel);
    if (!n2 || !n2->terminado()) return false;
    Jugador* j1 = n2->getJugador1();
    Jugador* j2 = n2->getJugador2();
    // El jugador gana si j1 sigue vivo y j2 fue derrotado
    return j1 && j1->getVida() > 0 && j2 && j2->getVida() <= 0;
}

// ─── Flujo nivel 1

void MainWindow::iniciarNivel1(Nivel1::Dificultad dificultad)
{
    dificultadPendiente = dificultad;
    mostrandoCargaN1    = true;
    temporizadorCargaN1 = 0.0f;
    menuActivo          = false;
}

void MainWindow::mostrarPantallaCargaN1(Nivel1::Dificultad dificultad)
{
    musicaMenu.stop();
    musicaNivel1.play();

    delete nivel;

    nivel = new Nivel1(dificultad);
    nivel->iniciar();

    scrollMundo    = 0.0f;
    velocidadMundo = nivel->getVelocidadMundo();

    mostrandoCargaN1 = false;
}

// ─── Flujo nivel 2

void MainWindow::iniciarNivel2()
{
    mostrandoEntreNiveles    = true;
    temporizadorEntreNiveles = 0.0f;
}

void MainWindow::iniciarCutscene()
{
    musicaNivel1.stop();
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

    setStyleSheet("");
    reproduciendo       = false;
    mostrandoCargaN2    = true;
    temporizadorCargaN2 = 0.0f;

    musicaNivel2.play();

    timer->start(16);
}

void MainWindow::iniciarNivel2Real()
{
    delete nivel;

    nivel = new Nivel2();
    nivel->iniciar();

    scrollMundo    = 0.0f;
    velocidadMundo = nivel->getVelocidadMundo();

    menuActivo       = false;
    mostrandoCargaN2 = false;
}

// ─── Video final (victoria nivel 2)

void MainWindow::reproducirVideoFinal()
{
    musicaNivel1.stop();
    musicaNivel2.stop();

    delete nivel;
    nivel = nullptr;

    reproduciendo       = true;
    mostrandoVideoFinal = true;
    timer->stop();

    setStyleSheet("background-color: black;");

    videoWidget = new QVideoWidget(this);
    videoWidget->setGeometry(0, 0, 1024, 768);
    videoWidget->show();
    videoWidget->raise();

    mediaPlayer = new QMediaPlayer(this);

    QAudioOutput* audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0.8f);
    mediaPlayer->setAudioOutput(audioOutput);

    mediaPlayer->setVideoOutput(videoWidget);
    mediaPlayer->setSource(QUrl("qrc:/final.mp4"));

    connect(
        mediaPlayer,
        &QMediaPlayer::playbackStateChanged,
        this,
        &MainWindow::onVideoFinalTerminado
        );

    mediaPlayer->play();
}

void MainWindow::onVideoFinalTerminado(QMediaPlayer::PlaybackState state)
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

    setStyleSheet("");
    reproduciendo       = false;
    mostrandoVideoFinal = false;

    // Al terminar el video final volvemos al menú
    volverAlMenu();

    timer->start(16);
}

// ─── Pantalla de derrota

void MainWindow::mostrarPantallaPerdedor()
{
    musicaNivel1.stop();
    musicaNivel2.stop();

    delete nivel;
    nivel = nullptr;

    scrollMundo           = 0.0f;
    mostrandoPerdedor     = true;
    temporizadorPerdedor  = 0.0f;
}

// ─── Volver al menú ─

void MainWindow::volverAlMenu()
{
    musicaNivel1.stop();
    musicaNivel2.stop();

    delete nivel;

    nivel       = nullptr;
    scrollMundo = 0.0f;
    menuActivo  = true;

    mostrandoPerdedor   = false;
    mostrandoVideoFinal = false;

    musicaMenu.play();
}

//  Loop principal

void MainWindow::loop()
{
    // Pantalla de derrota: espera 3 segundos y vuelve al menú
    if (mostrandoPerdedor)
    {
        temporizadorPerdedor += 1.0f / 60.0f;

        if (temporizadorPerdedor >= 3.0f)
            volverAlMenu();

        update();
        return;
    }

    if (mostrandoCargaN1)
    {
        temporizadorCargaN1 += 1.0f / 60.0f;

        if (temporizadorCargaN1 >= 5.0f)
            mostrarPantallaCargaN1(dificultadPendiente);

        update();
        return;
    }

    if (mostrandoEntreNiveles)
    {
        temporizadorEntreNiveles += 1.0f / 60.0f;

        if (temporizadorEntreNiveles >= 5.0f)
        {
            mostrandoEntreNiveles = false;
            iniciarCutscene();
        }

        update();
        return;
    }

    if (mostrandoCargaN2)
    {
        temporizadorCargaN2 += 1.0f / 60.0f;

        if (temporizadorCargaN2 >= 5.0f)
            iniciarNivel2Real();

        update();
        return;
    }

    if (menuActivo || reproduciendo)
    {
        update();
        return;
    }

    float dt = 1.0f / 60.0f;
    scrollMundo += velocidadMundo * dt;

    nivel->configurarMovimiento(dt, scrollMundo);
    nivel->actualizar();

    // ── Detectar victoria nivel 2
    if (jugadorGanoNivel2(nivel))
    {
        reproducirVideoFinal();
        update();
        return;
    }

    // ── Detectar derrota (nivel 1 o nivel 2)
    if (jugadorPerdioNivel1(nivel) || jugadorPerdioNivel2(nivel))
    {
        mostrarPantallaPerdedor();
        update();
        return;
    }

    // Avanzar al siguiente nivel
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

// ─── Render nivel 2

void MainWindow::renderizarNivel2(QPainter& painter)
{
    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivel);
    if (!nivel2) return;

    if (nivel2->isFondoCargado())
        painter.drawPixmap(0, 0, 1024, 768, nivel2->getFondo());

    if (nivel2->isPisoCargado())
        painter.drawPixmap(0, 550, 1024, 220, nivel2->getPiso());

    Jugador* j1 = nivel2->getJugador1();
    Jugador* j2 = nivel2->getJugador2();

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

    painter.setPen(Qt::NoPen);
    for (auto* f : nivel2->getFlechas())
        f->renderizar(painter);

    if (nivel2->terminado())
    {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 40, QFont::Bold));

        if (j1 && j1->getVida() <= 0)
            painter.drawText(330, 200, "GANA IA");
        else
            painter.drawText(300, 200, "GANASTE");
    }

    const int BARRA_W  = 380;
    const int BARRA_H  = 16;
    const int BARRA_Y1 = 20;
    const int BARRA_Y2 = 42;
    const int MARGEN   = 20;

    painter.setPen(Qt::NoPen);

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

// ─── paintEvent

void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // Pantalla de derrota
    if (mostrandoPerdedor)
    {
        if (!pantallaPerdedor.isNull())
            painter.drawPixmap(0, 0, 1024, 768, pantallaPerdedor);
        else
        {
            painter.fillRect(0, 0, 1024, 768, Qt::black);
            painter.setPen(Qt::red);
            painter.setFont(QFont("Arial", 48, QFont::Bold));
            painter.drawText(QRect(0, 0, 1024, 768), Qt::AlignCenter, "PERDISTE");
        }
        return;
    }

    if (mostrandoCargaN1)
    {
        if (!pantallaCargaN1.isNull())
            painter.drawPixmap(0, 0, 1024, 768, pantallaCargaN1);
        else
        {
            painter.fillRect(0, 0, 1024, 768, Qt::black);
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 30, QFont::Bold));
            painter.drawText(QRect(0, 0, 1024, 768), Qt::AlignCenter, "Cargando...");
        }
        return;
    }

    if (mostrandoEntreNiveles)
    {
        if (!pantallaCargaEntreNiveles.isNull())
            painter.drawPixmap(0, 0, 1024, 768, pantallaCargaEntreNiveles);
        else
        {
            painter.fillRect(0, 0, 1024, 768, Qt::black);
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 30, QFont::Bold));
            painter.drawText(QRect(0, 0, 1024, 768), Qt::AlignCenter, "Cargando...");
        }
        return;
    }

    if (mostrandoCargaN2)
    {
        if (!pantallaCargaN2.isNull())
            painter.drawPixmap(0, 0, 1024, 768, pantallaCargaN2);
        else
        {
            painter.fillRect(0, 0, 1024, 768, Qt::black);
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 30, QFont::Bold));
            painter.drawText(QRect(0, 0, 1024, 768), Qt::AlignCenter, "Cargando...");
        }
        return;
    }

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

// ─── Menú
void MainWindow::dibujarMenu(QPainter& painter)
{
    painter.drawPixmap(0, 0, fondoMenu);

    botonNormal  = QRect(162, 420, 210, 200);
    botonDificil = QRect(660, 420, 210, 200);

    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::NoPen);
    painter.drawRect(botonNormal);
    painter.drawRect(botonDificil);
}

// ─── Inputs

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (reproduciendo)
    {
        if (event->key() == Qt::Key_Escape && mediaPlayer)
            mediaPlayer->stop();
        return;
    }

    if (mostrandoCargaN1 || mostrandoEntreNiveles || mostrandoCargaN2 || mostrandoPerdedor)
        return;

    if (menuActivo || !nivel)
        return;

    Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivel);

    if (nivel2)
    {
        if (event->key() == Qt::Key_A)
            nivel2->keyPresado(Qt::Key_A);

        if (event->key() == Qt::Key_D)
            nivel2->keyPresado(Qt::Key_D);

        if (event->key() == Qt::Key_W)
            nivel2->keyPresado(Qt::Key_W);

        if (event->key() == Qt::Key_Z)
            nivel2->ataqueJugador();

        if (event->key() == Qt::Key_X)
            nivel2->bloqueoJugador(true);

        if (event->key() == Qt::Key_I)
            nivel2->setIAActiva(!nivel2->getIAActiva());

        return;
    }

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

    if (event->key() == Qt::Key_A)
        nivel2->keySoltado(Qt::Key_A);

    if (event->key() == Qt::Key_D)
        nivel2->keySoltado(Qt::Key_D);

    if (event->key() == Qt::Key_X)
        nivel2->bloqueoJugador(false);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    int x = event->position().x();
    int y = event->position().y();

    if (mostrandoCargaN1 || mostrandoEntreNiveles || mostrandoCargaN2 || mostrandoPerdedor)
        return;

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

