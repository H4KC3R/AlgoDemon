#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include "DockManager.h"
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(CameraQHYCCD::initSDK()) {
        QMessageBox::warning(this, "Внимание", "Драйверы для камеры не установлены!\nКамера недоступна!");
        ui->cameraTab->setEnabled(false);
    }

    connect(ui->findCamerasButton, &QPushButton::clicked, this, &MainWindow::on_findCamerasButton_clicked);
    connect(ui->connectCameraButton, &QPushButton::clicked, this, &MainWindow::on_connectCameraButton_clicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::InitializeCameraControls(CameraQHYCCD* mCamera) {
    double min, max, step;
   if (mCamera->getControlMinMaxStep(gain, min, max, step)) {
       ui->cameraGainFrame->setEnabled(true);
       ui->cameraGainDSpinBox->setMaximum(max);
       ui->cameraGainDSpinBox->setMinimum(min);
       ui->cameraGainDSpinBox->setSingleStep(step);

       ui->cameraGainHSlider->setEnabled(true);
       ui->cameraGainHSlider->setMaximum(max);
       ui->cameraGainHSlider->setMinimum(min);
       ui->cameraGainHSlider->setSingleStep(step);

       ui->cameraGMinValLabel->setText(QString::number(min));
       ui->cameraGMaxValLabel->setText(QString::number(max));
   }

   if (mCamera->getControlMinMaxStep(exposure, min, max, step)) {
       ui->cameraGainFrame->setEnabled(true);
       ui->cameraExposureDSpinBox->setMaximum(max);
       ui->cameraExposureDSpinBox->setMinimum(min);
       ui->cameraExposureDSpinBox->setSingleStep(step);

       ui->cameraExposureHSlider->setEnabled(true);
       ui->cameraExposureHSlider->setMaximum(max);
       ui->cameraExposureHSlider->setMinimum(min);
       ui->cameraExposureHSlider->setSingleStep(step);

       ui->cameraExpMinValLabel->setText(QString::number(min));
       ui->cameraExpMaxValLabel->setText(QString::number(max));
   }

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
        InitializeCameraControls(mCamera);
    }

}

//void MainWindow::on_disconnectCameraButton_clicked() {

//}
