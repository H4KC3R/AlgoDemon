#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QGraphicsView"
#include "QGraphicsScene"
#include "QGraphicsItem"

#include <QMainWindow>
#include <QMap>

#include "appprocessor.h"

#include "widgets/viewportcontroller.h"
#include "widgets/graphicsview.h"

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

signals:
    void fpsChanged(double fps);
    void EGChanged(double gain, double exposure);
    void roiChanged(RoiBox roi);

    void newImageProcessingFlags(ImageProcessingFlags imageProcessingFlags);

    void autoExposureEnabled(bool status, double gain, double exposure);
    void autoExposureSettingChanged(AutoExposureParams params);
    void focusingEnabled(bool status, cv::Rect roi);

private slots:
    void onProcessFinished();

    void onHardFault(QString errorMsg);
    void onSoftFault(QString errorMsg);

    void updateFrame(const QImage &frame);

    void updateFocusingResult(const QImage& frame,double position);
    void updateEG(double gain, double exposure);
    void onObjectiveError(QString msg);
    void onImageProcessingError(QString msg);

private:
    Ui::MainWindow *ui;

    QMap<QString, QString> cameraIdModel;
    AppProcessor processor;

    dororo::GraphicsView* displayView;
    QGraphicsScene displayScene;
    QGraphicsPixmapItem imageMapItem;
    dororo::ViewportController* roiController;

    AutoExposureParams param;
    ImageProcessingFlags imageProcessingFlags;

    // *********************************************************************** //

    void uiSignalSlotsInit();

    void setInitialGUIState();

    void initializeDisplay();

    void proccessorSignalSlotsInit();

    void objectiveSignalSlotsInit();

    void initializeCameraControls();

    void initializeStructures();

    void SingleCapture();

    // ************************** Camera Connection ************************** //

    void on_findCamerasButton_clicked();

    void on_connectCameraButton_clicked();

    void on_disconnectCameraButton_clicked();

    // ************************** Camera Settings *************************** //

    void on_cameraFpsSpinBox_valueChanged(int value);

    void on_cameraGainDSpinBox_valueChanged(double val);

    void on_cameraGainHSlider_valueChanged(int val);

    void on_cameraExposureDSpinBox_valueChanged(double val);

    void on_cameraExposureHSlider_valueChanged(int val);

    void on_cameraSetRoiButton_clicked();

    // ************************** Image Processing ************************** //

    void on_debayerCheckBox_clicked(bool enabled);

    void on_whiteBalanceCheckBox_clicked(bool enabled);

    void on_contrastEnableCheckBox_clicked(bool enabled);

    void on_contrastAlphaSpinBox_valueChanged(double value);

    void on_gammaContrastEnableCheckBox_clicked(bool enabled);

    void on_gammaCoeffSpinBox_valueChanged(double value);

    // ************************ Camera Image Capture ************************ //

    void on_cameraStartCaptureButton_clicked();

    void on_cameraStopCaptureButton_clicked();

    // ************************** Camera Focusing *************************** //

    void on_cameraEnableFocus_cliked(bool enabled);

    void on_cameraFocusButton_clicked();

    // ************************ Camera AutoExposure ************************* //

    void on_autoExposureCheckBox_clicked();

    void on_maxPercentHSlider_valueChanged(int value);

    void on_meanHSlider_valueChanged(int value);

    void on_maxRelCoeffHSlider_valueChanged(int value);

    void on_minRelCoeffHSlider_valueChanged(int value);

    // ************************* Objective Control ************************** //

    void on_objectiveComFindButton_clicked();

    void on_objectiveComConnectButton_clicked();

    void on_objectiveComDisconnectButton_clicked();

    void on_objectiveLensFileButton_clicked();

    void on_objectiveSetAppertureButton_clicked();

    void on_objectiveSetFocusButton_clicked();

    void on_objectiveGetFocusButton_clicked();

    // QWidget interface
protected:
    void showEvent(QShowEvent *event);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};
#endif // MAINWINDOW_H
