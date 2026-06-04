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
    timer(new QTimer(this)),
    fondoCargado(false),
    menuActivo(true),
    reproduciendo(false),
    cinematicaActiva(false),
    nivel1Ganado(false),
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

// ─── Flujo nivel 1

void MainWindow::iniciarNivel1(Nivel1::Dificultad dificultad)
{
    dificultadPendiente = dificultad;
    mostrandoCargaN1    = true;
    temporizadorCargaN1 = 0.0f;
    menuActivo          = false;
    nivel1Ganado        = false;
}

void MainWindow::mostrarPantallaCargaN1(Nivel1::Dificultad dificultad)
{
    musicaMenu.stop();
    musicaNivel1.play();

    juego.iniciarNivel1(dificultad);

    scrollMundo    = 0.0f;
    velocidadMundo = juego.getVelocidadMundo();

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
    reproduciendo    = true;
    cinematicaActiva = true;
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

    connect(
        mediaPlayer,
        &QMediaPlayer::mediaStatusChanged,
        this,
        &MainWindow::onVideoEstadoCambiado
        );

    connect(
        mediaPlayer,
        &QMediaPlayer::errorOccurred,
        this,
        &MainWindow::onVideoError
        );

    mediaPlayer->play();
}

void MainWindow::onVideoTerminado(QMediaPlayer::PlaybackState state)
{
    if (state != QMediaPlayer::StoppedState)
        return;

    finalizarCutscene();
}

void MainWindow::onVideoEstadoCambiado(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia)
        finalizarCutscene();
}

void MainWindow::onVideoError(QMediaPlayer::Error error, const QString& errorString)
{
    (void)errorString;

    if (error != QMediaPlayer::NoError)
        finalizarCutscene();
}

void MainWindow::finalizarCutscene()
{
    if (!cinematicaActiva || mostrandoVideoFinal)
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
    cinematicaActiva    = false;
    mostrandoCargaN2    = true;
    temporizadorCargaN2 = 0.0f;

    musicaNivel2.play();

    timer->start(16);
}

void MainWindow::iniciarNivel2Real()
{
    juego.iniciarNivel2();

    Nivel2* nivel2 = juego.getNivel2();
    if (nivel1Ganado && nivel2 && nivel2->getJugador2())
        nivel2->getJugador2()->setVida(65.0f);

    scrollMundo    = 0.0f;
    velocidadMundo = juego.getVelocidadMundo();

    menuActivo       = false;
    mostrandoCargaN2 = false;
}

// ─── Video final (victoria nivel 2)

void MainWindow::reproducirVideoFinal()
{
    musicaNivel1.stop();
    musicaNivel2.stop();

    juego.limpiarNivel();

    reproduciendo       = true;
    cinematicaActiva    = true;
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
    cinematicaActiva    = false;
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

    juego.limpiarNivel();

    scrollMundo           = 0.0f;
    mostrandoPerdedor     = true;
    temporizadorPerdedor  = 0.0f;
}

// ─── Volver al menú ─

void MainWindow::volverAlMenu()
{
    musicaNivel1.stop();
    musicaNivel2.stop();

    juego.limpiarNivel();

    scrollMundo = 0.0f;
    menuActivo  = true;

    mostrandoPerdedor   = false;
    mostrandoVideoFinal = false;
    cinematicaActiva    = false;
    nivel1Ganado        = false;

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

    Juego::ResultadoActualizacion resultado = juego.actualizar(dt, scrollMundo);

    if (resultado == Juego::VictoriaNivel2)
    {
        reproducirVideoFinal();
        update();
        return;
    }

    if (resultado == Juego::Derrota)
    {
        mostrarPantallaPerdedor();
        update();
        return;
    }

    if (resultado == Juego::SiguienteNivel)
    {
        Nivel1* nivel1 = dynamic_cast<Nivel1*>(juego.getNivel());
        nivel1Ganado = nivel1 && nivel1->aplicaDanioInicialNivel2();
        iniciarNivel2();
        return;
    }

    update();
}

// ─── Render nivel 2

void MainWindow::renderizarNivel2(QPainter& painter)
{
    Nivel2* nivel2 = juego.getNivel2();
    if (!nivel2)
        return;

    dibujarEscenarioNivel2(painter, nivel2);
    dibujarPersonajesNivel2(painter, nivel2);
    dibujarFlechasNivel2(painter, nivel2);
    dibujarResultadoNivel2(painter, nivel2);
    dibujarHudNivel2(painter, nivel2);
}

void MainWindow::dibujarEscenarioNivel2(QPainter& painter, Nivel2* nivel2)
{
    if (nivel2->isFondoCargado())
        painter.drawPixmap(0, 0, 1024, 768, nivel2->getFondo());
    else
        painter.fillRect(0, 0, 1024, 768, QColor(26, 29, 35));

}

void MainWindow::dibujarPersonajesNivel2(QPainter& painter, Nivel2* nivel2)
{
    Jugador* j1 = nivel2->getJugador1();
    Jugador* j2 = nivel2->getJugador2();

    if (j1)
        j1->renderizar(painter);

    if (j2)
        j2->renderizar(painter);
}

void MainWindow::dibujarFlechasNivel2(QPainter& painter, Nivel2* nivel2)
{
    painter.setPen(Qt::NoPen);
    for (auto* f : nivel2->getFlechas())
    {
        if (f)
            f->renderizar(painter);
    }
}

void MainWindow::dibujarResultadoNivel2(QPainter& painter, Nivel2* nivel2)
{
    if (!nivel2->terminado())
        return;

    Jugador* j1 = nivel2->getJugador1();

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 40, QFont::Bold));

    if (j1 && j1->getVida() <= 0)
        painter.drawText(330, 200, "GANA IA");
    else
        painter.drawText(300, 200, "GANASTE");
}

void MainWindow::dibujarHudNivel2(QPainter& painter, Nivel2* nivel2)
{
    Jugador* j1 = nivel2->getJugador1();
    Jugador* j2 = nivel2->getJugador2();

    const int barraW  = 380;
    const int barraH  = 16;
    const int barraY1 = 20;
    const int barraY2 = 42;
    const int margen  = 20;

    painter.setPen(Qt::NoPen);

    if (j1)
    {
        painter.setBrush(QColor(60, 60, 60));
        painter.drawRect(margen, barraY1, barraW, barraH);
        painter.drawRect(margen, barraY2, barraW, barraH);

        int vidaW1 = static_cast<int>(j1->getVida() / 100.0f * barraW);
        painter.setBrush(QColor(200, 30, 30));
        painter.drawRect(margen, barraY1, vidaW1, barraH);

        int escudoW1 = static_cast<int>(j1->getEscudo() / 100.0f * barraW);
        painter.setBrush(QColor(30, 100, 220));
        painter.drawRect(margen, barraY2, escudoW1, barraH);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(margen, barraY1 - 4, "JUGADOR");
        painter.setPen(Qt::NoPen);
    }

    if (j2)
    {
        int xDerecha = 1024 - margen - barraW;

        painter.setBrush(QColor(60, 60, 60));
        painter.drawRect(xDerecha, barraY1, barraW, barraH);
        painter.drawRect(xDerecha, barraY2, barraW, barraH);

        int vidaW2   = static_cast<int>(j2->getVida()   / 100.0f * barraW);
        int escudoW2 = static_cast<int>(j2->getEscudo() / 100.0f * barraW);

        painter.setBrush(QColor(200, 30, 30));
        painter.drawRect(1024 - margen - vidaW2, barraY1, vidaW2, barraH);

        painter.setBrush(QColor(30, 100, 220));
        painter.drawRect(1024 - margen - escudoW2, barraY2, escudoW2, barraH);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        QFontMetrics fm(painter.font());
        int textoW = fm.horizontalAdvance("IA");
        painter.drawText(1024 - margen - textoW, barraY1 - 4, "IA");
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

    Nivel* nivel = juego.getNivel();
    Nivel2* nivel2 = juego.getNivel2();

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
        if (cinematicaActiva && event->key() == Qt::Key_S && mediaPlayer)
            mediaPlayer->stop();

        if (event->key() == Qt::Key_Escape && mediaPlayer)
            mediaPlayer->stop();
        return;
    }

    if (mostrandoCargaN1 || mostrandoEntreNiveles || mostrandoCargaN2 || mostrandoPerdedor)
        return;

    if (menuActivo || !juego.tieneNivel())
        return;

    juego.manejarTeclaPresionada(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (menuActivo || !juego.tieneNivel()) return;

    juego.manejarTeclaSoltada(event->key());
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

    if (!juego.tieneNivel())
        return;

    juego.manejarClick(x, y);
}

