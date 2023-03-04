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

private:
    Ui::MainWindow *ui;

    // ************************** Camera ************************** //

    void initializeCameraControls(CameraQHYCCD* mCamera);

    void on_findCamerasButton_clicked();

    void on_connectCameraButton_clicked();

    void on_disconnectCameraButton_clicked();

    void on_cameraGainDSpinBox_valueChanged(double val);

    void on_cameraGainHSlider_valueChanged(int val);

    void on_cameraExposureDSpinBox_valueChanged(double val);

    void on_cameraExposureHSlider_valueChanged(int val);

    void on_cameraDepthComboBox_currentIndexChanged(int index);

    void on_cameraSetRoiButton_currentIndexChanged();

    // ************************** Objective ************************** //

    void on_objectiveComFindButton_clicked();



private:
    bool isSettingsChangedFlag = false;

    QMap<QString, QString> cameraIdModel;
    CameraQHYCCD* mCamera;
};
#endif // MAINWINDOW_H
