#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QGraphicsView"
#include "QGraphicsScene"
#include "QGraphicsItem"

#include <QMainWindow>
#include <QMap>

#include "cameraqhyccd.h"
#include "appprocessor.h"

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

    void uiSignalSlotsInit();
    void setInitialGUIState();
    void proccessorSignalSlotsInit();
    void initializeCameraControls();

signals:
    void newImageProcessingFlags(ImageProcessingFlags imageProcessingFlags);
    void autoExposureEnabled(double gain, double exposure);

    void EGChanged(double gain, double exposure);
    void depthChanged(BitMode bit);
    void roiChanged(RoiBox roi);

private slots:
    void onProcessFinished();

    void updateFrame(const QImage &frame);
    void updateEG(double gain, double exposure);
    void showError(QString errorMsg);

private:
    Ui::MainWindow *ui;
    QGraphicsScene* scene;

    // ****************************** Camera ****************************** //

    void showImage(cv::Mat& image);

    void initializeCameraControls(CameraQHYCCD* mCamera);

    void startSingleCapture();

    void startLiveCapture();

    void startCapture();

    void stopCapture();

    void processImage();

    // ************************** Camera Handler ************************** //

    void on_findCamerasButton_clicked();

    void on_connectCameraButton_clicked();

    void on_disconnectCameraButton_clicked();

    void on_cameraGainDSpinBox_valueChanged(double val);

    void on_cameraGainHSlider_valueChanged(int val);

    void on_cameraExposureDSpinBox_valueChanged(double val);

    void on_cameraExposureHSlider_valueChanged(int val);

    void on_cameraDepthComboBox_currentIndexChanged(int index);

    void on_cameraSetRoiButton_clicked();

    void on_cameraStartCaptureButton_clicked();

    void on_cameraStopCaptureButton_clicked();

    // **************************** Objective ****************************** //

    void on_objectiveComFindButton_clicked();

    void on_objectiveComConnectButton_clicked();

    void on_objectiveComDisconnectButton_clicked();

    void on_objectiveLensFileButton_clicked();

    void on_objectiveSetAppertureButton_clicked();

    void on_objectiveSetFocusButton_clicked();

    void on_objectiveGetFocusButton_clicked();

private:
    QMap<QString, QString> cameraIdModel;
    AppProcessor processor;
};
#endif // MAINWINDOW_H
