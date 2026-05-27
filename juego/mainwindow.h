#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nivel1.h"
#include "nivel2.h"
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
    void paintEvent(QPaintEvent* event)     override;
    void keyPressEvent(QKeyEvent* event)    override;
    void keyReleaseEvent(QKeyEvent* event)  override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void loop();
    void onVideoTerminado(QMediaPlayer::PlaybackState state);

private:
    void cargarFondo();
    void iniciarNivel1(Nivel1::Dificultad dificultad);
    void iniciarNivel2();
    void iniciarNivel2Real();
    void reproducirCutscene();
    void volverAlMenu();
    void dibujarMenu(QPainter& painter);
    void renderizarNivel2(QPainter& painter);

    Ui::MainWindow* ui;
    Nivel*        nivel;
    QTimer*       timer;
    QPixmap       fondo;
    QPixmap       fondoMenu;
    bool          fondoCargado;
    bool          menuActivo;
    bool          reproduciendo;
    QRect         botonNormal;
    QRect         botonDificil;
    QSoundEffect  musicaMenu;
    QMediaPlayer* mediaPlayer;
    QVideoWidget* videoWidget;
};

#endif // MAINWINDOW_H
