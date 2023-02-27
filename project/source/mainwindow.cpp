#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include "DockManager.h"
#include <QGraphicsView>

#include "camera_module/include/cameraqhyccd.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

