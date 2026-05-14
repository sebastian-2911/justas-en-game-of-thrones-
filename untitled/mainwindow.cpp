#include "mainwindow.h"
#include "graficos.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    Graficos* g = new Graficos(this);
    setCentralWidget(g);
}

MainWindow::~MainWindow()
{
    delete ui;
}
