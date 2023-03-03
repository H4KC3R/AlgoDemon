#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "cameraqhyccd.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace ads {class CDockManager; }

// Чтобы компилятор понимал какую версию string использовать
using std::__cxx11::basic_string;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    InitializeCameraControls(CameraQHYCCD* mCamera);

    void on_findCamerasButton_clicked();

    void on_connectCameraButton_clicked();

private:
    QMap<QString, QString> cameraIdModel;
    CameraQHYCCD* mCamera;


};
#endif // MAINWINDOW_H
