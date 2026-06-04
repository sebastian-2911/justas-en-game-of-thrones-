#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "juego.h"

#include <QAudioOutput>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QPixmap>
#include <QRect>
#include <QSoundEffect>
#include <QTimer>
#include <QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* event)      override;
    void keyPressEvent(QKeyEvent* event)     override;
    void keyReleaseEvent(QKeyEvent* event)   override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void loop();
    void onVideoTerminado(QMediaPlayer::PlaybackState state);
    void onVideoEstadoCambiado(QMediaPlayer::MediaStatus status);
    void onVideoError(QMediaPlayer::Error error, const QString& errorString);
    void onVideoFinalTerminado(QMediaPlayer::PlaybackState state);

private:
    void cargarFondo();
    void iniciarNivel1(Nivel1::Dificultad dificultad);
    void mostrarPantallaCargaN1(Nivel1::Dificultad dificultad);
    void iniciarNivel2();
    void iniciarCutscene();
    void iniciarNivel2Real();
    void reproducirCutscene();
    void finalizarCutscene();
    void reproducirVideoFinal();
    void mostrarPantallaPerdedor();
    void volverAlMenu();
    void dibujarMenu(QPainter& painter);
    void renderizarNivel2(QPainter& painter);
    void dibujarEscenarioNivel2(QPainter& painter, Nivel2* nivel2);
    void dibujarPersonajesNivel2(QPainter& painter, Nivel2* nivel2);
    void dibujarFlechasNivel2(QPainter& painter, Nivel2* nivel2);
    void dibujarResultadoNivel2(QPainter& painter, Nivel2* nivel2);
    void dibujarHudNivel2(QPainter& painter, Nivel2* nivel2);

    Ui::MainWindow* ui;
    Juego           juego;
    QTimer*         timer;

    // fondos
    QPixmap         fondo;
    QPixmap         fondoMenu;
    bool            fondoCargado;

    // estado general
    bool            menuActivo;
    bool            reproduciendo;
    bool            cinematicaActiva;
    bool            nivel1Ganado;

    // pantalla de carga nivel 1
    QPixmap            pantallaCargaN1;
    bool               mostrandoCargaN1;
    float              temporizadorCargaN1;
    Nivel1::Dificultad dificultadPendiente;

    // pantalla entre niveles
    QPixmap         pantallaCargaEntreNiveles;
    bool            mostrandoEntreNiveles;
    float           temporizadorEntreNiveles;

    // pantalla de carga nivel 2
    QPixmap         pantallaCargaN2;
    bool            mostrandoCargaN2;
    float           temporizadorCargaN2;

    // pantalla de derrota
    QPixmap         pantallaPerdedor;
    bool            mostrandoPerdedor;
    float           temporizadorPerdedor;

    // video final (victoria nivel 2)
    bool            mostrandoVideoFinal;

    // botones menú
    QRect           botonNormal;
    QRect           botonDificil;

    // audio
    QSoundEffect    musicaMenu;
    QSoundEffect    musicaNivel1;
    QSoundEffect    musicaNivel2;

    // video cutscene / final
    QMediaPlayer*   mediaPlayer;
    QVideoWidget*   videoWidget;
};

#endif // MAINWINDOW_H
