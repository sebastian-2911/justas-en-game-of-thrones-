#include "graficos.h"
#include "Nivel1.h"
#include "Jugador.h"

#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>

// ========================== CONSTRUCTOR ==========================
Graficos::Graficos(QWidget* parent)
    : QWidget(parent),
    nivel(new Nivel1(Nivel1::NORMAL)),
    timer(new QTimer(this)),
    fondoCargado(false),
    offsetY(0.0f),
    velocidadFondo(2.0f)
{
    setFixedSize(1024, 768);

    nivel->iniciar();
    cargarFondo();

    connect(timer, &QTimer::timeout, this, &Graficos::loop);
    timer->start(16); // ~60 FPS

    setFocusPolicy(Qt::StrongFocus);
}

// ========================== DESTRUCTOR ==========================
Graficos::~Graficos()
{
    delete nivel;
}

// ========================== CARGAR FONDO ==========================
void Graficos::cargarFondo()
{
    QPixmap original;
    if (!original.load("fondo.jpeg")) {
        fondoCargado = false;
        return;
    }

    QPixmap escalado = original.scaled(1024, 1200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Hacer tileable: mezclar el borde inferior con el superior en una zona de transición
    QImage img = escalado.toImage();
    int w = img.width();
    int h = img.height();
    int zona = 150; // px de transición

    for (int y = 0; y < zona; y++)
    {
        float alpha = (float)y / zona; // 0.0 arriba → 1.0 abajo
        for (int x = 0; x < w; x++)
        {
            QColor colorTop = img.pixelColor(x, y);           // borde superior
            QColor colorBot = img.pixelColor(x, h - zona + y); // borde inferior
            // mezclar: el borde inferior se convierte en el superior gradualmente
            int r = colorBot.red()   * (1-alpha) + colorTop.red()   * alpha;
            int g = colorBot.green() * (1-alpha) + colorTop.green() * alpha;
            int b = colorBot.blue()  * (1-alpha) + colorTop.blue()  * alpha;
            img.setPixelColor(x, y, QColor(r,g,b));
        }
    }

    fondo = QPixmap::fromImage(img);
    fondoCargado = true;
}
// ========================== LOOP ==========================
void Graficos::loop()
{
    float dt = 1.0f / 60.0f;
    offsetY += velocidadFondo;  // fondo baja = jugador avanza

    int h = fondo.height();
    // Cuando el tile principal salió completamente por arriba, lo reposicionamos abajo
    if (offsetY >= (float)h)
        offsetY -= (float)h;

    nivel->actualizar(dt, velocidadFondo);
    update();
}
// ========================== RENDER ==========================
void Graficos::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    if (fondoCargado)
    {
        int h = fondo.height();
        int y = static_cast<int>(offsetY);

        // Tile 1: empieza en y (baja)
        // Tile 2: siempre ARRIBA del tile 1, fuera de pantalla hasta que se necesite
        painter.drawPixmap(0, y - h, fondo);  // arriba (entrando desde arriba)
        painter.drawPixmap(0, y,     fondo);  // principal
    }
    if (nivel)
        nivel->renderizar(painter);
}
// ========================== INPUT ==========================
void Graficos::keyPressEvent(QKeyEvent* event)
{
    Jugador* j = nivel->getJugador();

    if (!j) return;

    if (event->key() == Qt::Key_A)
        j->procesarInput('a');

    if (event->key() == Qt::Key_D)
        j->procesarInput('d');
}

// ========================== CLICK ==========================
void Graficos::mousePressEvent(QMouseEvent* event)
{
    nivel->manejarClick(
        event->position().x(),
        event->position().y()
        );
}
