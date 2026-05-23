#ifndef GRAFICOS_H
#define GRAFICOS_H

#include "nivel1.h"
#include "nivel2.h"
#include <QPixmap>
#include <QRect>
#include <QTimer>
#include <QWidget>

class Graficos : public QWidget
{
    Q_OBJECT

public:
    explicit Graficos(QWidget* parent = nullptr);
    ~Graficos();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void loop();

private:
    // fondo
    void cargarFondo();

    // menu
    void iniciarNivel1(Nivel1::Dificultad dificultad);
    void iniciarNivel2();
    void volverAlMenu();
    void dibujarMenu(QPainter& painter);

    Nivel* nivel;
    QTimer* timer;

    QPixmap fondo;
    bool fondoCargado;

    bool menuActivo;
    QRect botonNormal;
    QRect botonDificil;
};

#endif
