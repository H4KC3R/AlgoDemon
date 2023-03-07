#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!CameraQHYCCD::initSDK()) {
        QMessageBox::warning(this, "Внимание", "Драйверы для камеры не установлены!\nКамера недоступна!");
        ui->cameraTab->setEnabled(false);
    }

    connect(ui->findCamerasButton, &QPushButton::clicked, this, &MainWindow::on_findCamerasButton_clicked);
    connect(ui->connectCameraButton, &QPushButton::clicked, this, &MainWindow::on_connectCameraButton_clicked);
    connect(ui->disconnectCameraButton, &QPushButton::clicked, this, &MainWindow::on_disconnectCameraButton_clicked);

    connect(ui->cameraGainDSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_cameraGainDSpinBox_valueChanged);
    connect(ui->cameraGainHSlider, &QSlider::valueChanged, this, &MainWindow::on_cameraGainHSlider_valueChanged);

    connect(ui->cameraExposureDSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_cameraExposureDSpinBox_valueChanged);
    connect(ui->cameraExposureHSlider, &QSlider::valueChanged, this, &MainWindow::on_cameraExposureHSlider_valueChanged);

    connect(ui->cameraDepthComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::on_cameraDepthComboBox_currentIndexChanged);

    connect(ui->cameraSetRoiButton, &QPushButton::clicked, this, &MainWindow::on_findCamerasButton_clicked);

    connect(ui->objectiveComFindButton, &QPushButton::clicked, this, &MainWindow::on_objectiveComFindButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ************************** Camera ************************** //

void MainWindow::initializeImage() {
    CamParameters params = mCamera->getCameraParameters();
    camImg.h = params.mMaximgh;
    camImg.w = params.mMaximgw;
    camImg.channels = 1;
    camImg.bpp = mCamera->getImageBitMode();
    int type = ImageProcess::getOpenCvType((BitMode)camImg.bpp, camImg.channels);
    camImg.img = cv::Mat(camImg.h, camImg.w, type, 50);

    camImg.length = mCamera->getImgLength();
    camImgPipeline = new uint8_t[camImg.length * 2];
}

void MainWindow::showImage() {
    ui->imageLabel->setPixmap(QPixmap::fromImage(QImage(camImg.img.data, camImg.img.cols, camImg.img.rows, camImg.img.step, QImage::Format_Grayscale8)));
}

void MainWindow::initializeCameraControls(CameraQHYCCD* mCamera) {
    double min, max, step;

    CamParameters params = mCamera->getCameraParameters();
    if(params.mIsMono){
        ui->cameraTypeFrame->setEnabled(true);
        ui->cameraTypeValueLabel->setText("Монохромная");
        ui->debayerCheckBox->setEnabled(false);
        ui->whiteBalanceCheckBox->setEnabled(false);
        ui->contrastsFrame->setEnabled(false);
        ui->autoExposureFrame->setEnabled(true);
        ui->focusControlFrame->setEnabled(true);

        ui->cameraStartXSpinBox->setMaximum(params.mMaximgw - 1);
        ui->cameraStartYSpinBox->setMaximum(params.mMaximgh - 1);

        ui->cameraSizeXSpinBox->setMaximum(params.mMaximgw);
        ui->cameraSizeYSpinBox->setMaximum(params.mMaximgh);
    }

    if(mCamera->getControlMinMaxStep(transferbit, min, max, step)){
        ui->cameraDepthComboBox->setEnabled(true);

        // Почему то, в режиме фото при 8 битах не получается снять кадр
        if(!params.mIsLiveMode) {
            ui->cameraDepthComboBox->removeItem(0);
            mCamera->setImageBitMode(bit16);
        }
        else {
            if (mCamera->getImageBitMode() == 8)
                ui->cameraDepthComboBox->setCurrentIndex(0);
            else
                ui->cameraDepthComboBox->setCurrentIndex(1);
        }
    }

    if (mCamera->getControlMinMaxStep(gain, min, max, step)) {
       ui->cameraGainDSpinBox->setEnabled(true);
       ui->cameraGainDSpinBox->setMaximum(max);
       ui->cameraGainDSpinBox->setMinimum(min);
       if(step == 1)
           ui->cameraGainDSpinBox->setDecimals(0);
       ui->cameraGainDSpinBox->setSingleStep(step);

       ui->cameraGainHSlider->setEnabled(true);
       ui->cameraGainHSlider->setMaximum(max);
       ui->cameraGainHSlider->setMinimum(min);
       ui->cameraGainHSlider->setSingleStep(step);

       ui->cameraGMinValLabel->setText(QString::number(min));
       ui->cameraGMaxValLabel->setText(QString::number(max));

       double gain = mCamera->getGain();
       ui->cameraGainDSpinBox->setValue(gain);
   }

   if (mCamera->getControlMinMaxStep(exposure, min, max, step)) {
       ui->cameraExposureDSpinBox->setEnabled(true);
       ui->cameraExposureDSpinBox->setMaximum(max);
       ui->cameraExposureDSpinBox->setMinimum(min);
       ui->cameraExposureDSpinBox->setSingleStep(step);

       ui->cameraExposureHSlider->setEnabled(true);
       ui->cameraExposureHSlider->setMaximum(max / 1000);
       ui->cameraExposureHSlider->setMinimum(1);
       ui->cameraExposureHSlider->setSingleStep(step);

       ui->cameraExpMinValLabel->setText(QString::number(1));
       ui->cameraExpMaxValLabel->setText(QString::number(max / 1000));

       double exposure = mCamera->getExposure();
       ui->cameraExposureDSpinBox->setValue(exposure);
   }

   ui->cameraRoiGroupBox->setEnabled(true);
}

void MainWindow::on_findCamerasButton_clicked() {
    int num = CameraQHYCCD::searchCamera();
    if(num <= 0 )
        return;

    char id[32];
    QString qId;
    QString qModel;

    for(int i = 0; i < num; i++) {
        if(CameraQHYCCD::getID(i, id)) {
            qId = QString::fromUtf8(id);
            // Если эта камера есть пропускаем
            if(cameraIdModel.count(qId))
                continue;

            qModel = QString::fromStdString(id);
            cameraIdModel[qId] = qModel;
            ui->cameraComboBox->addItem(qModel);
        }
    }
}

void MainWindow::on_connectCameraButton_clicked() {
    if(ui->cameraComboBox->currentIndex() == -1)
        return;

    QString qId = cameraIdModel.value(ui->cameraComboBox->currentText());
    QByteArray bId = qId.toLocal8Bit();
    char* id = bId.data();
    mCamera = new CameraQHYCCD(id);

    StreamMode mode = (ui->modeCameraComboBox->currentIndex() == 0) ? single : live;
    if(mCamera->connect(mode)) {
        initializeCameraControls(mCamera);
        ui->cameraComboBox->setEnabled(false);
        ui->connectCameraButton->setEnabled(false);
        ui->modeCameraComboBox->setEnabled(false);

        ui->disconnectCameraButton->setEnabled(true);
        ui->cameraCaptureGroupBox->setEnabled(true);

        initializeImage();
        showImage();
    }
}

void MainWindow::on_disconnectCameraButton_clicked() {
    return;
}

void MainWindow::on_cameraGainDSpinBox_valueChanged(double val) {
    isSettingsChangedFlag = true;
    ui->cameraGainHSlider->setValue(val);
}

void MainWindow::on_cameraGainHSlider_valueChanged(int val) {
    isSettingsChangedFlag = true;
    ui->cameraGainDSpinBox->setValue(val);
}

void MainWindow::on_cameraExposureDSpinBox_valueChanged(double val) {
    isSettingsChangedFlag = true;
    ui->cameraExposureHSlider->setValue(val / 1000);
}

void MainWindow::on_cameraExposureHSlider_valueChanged(int val){
    isSettingsChangedFlag = true;
    ui->cameraExposureDSpinBox->setValue(val * 1000);
}

void MainWindow::on_cameraDepthComboBox_currentIndexChanged(int index) {
    isSettingsChangedFlag = true;
}

void MainWindow::on_cameraSetRoiButton_currentIndexChanged() {
    isSettingsChangedFlag = true;
}

// ************************** Objective ************************** //

void MainWindow::on_objectiveComFindButton_clicked() {
    return;
}

//void MainWindow::on_disconnectCameraButton_clicked() {

//}
