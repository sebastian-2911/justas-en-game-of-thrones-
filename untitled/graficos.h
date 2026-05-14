#ifndef GRAFICOS_H
#define GRAFICOS_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>

class Nivel1;

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
    void cargarFondo();

    Nivel1* nivel;
    QTimer* timer;

    QPixmap fondo;
    bool fondoCargado;

    float offsetY;
    float velocidadFondo;
};

#endif
