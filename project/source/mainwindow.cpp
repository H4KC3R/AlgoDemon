#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QGraphicsView>
#include <QtConcurrent/QtConcurrent>
#include "QtConcurrent/qtconcurrentrun.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QFileDialog>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

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
    connect(ui->cameraSetRoiButton, &QPushButton::clicked, this, &MainWindow::on_cameraSetRoiButton_clicked);

    connect(ui->cameraStartCaptureButton, &QPushButton::clicked, this, &MainWindow::on_cameraStartCaptureButton_clicked);
    connect(ui->cameraStopCaptureButton, &QPushButton::clicked, this, &MainWindow::on_cameraStopCaptureButton_clicked);

    qRegisterMetaType< cv::Mat >("cv::Mat");
    connect(this, SIGNAL(imageReady(cv::Mat)), this, SLOT(on_imageReady(cv::Mat)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(captureFinished()), this, SLOT(on_captureFinished()),
            Qt::QueuedConnection);

    connect(ui->objectiveComFindButton, &QPushButton::clicked, this, &MainWindow::on_objectiveComFindButton_clicked);
    connect(ui->objectiveComConnectButton, &QPushButton::clicked, this, &MainWindow::on_objectiveComConnectButton_clicked);
    connect(ui->objectiveComDisconnectButton, &QPushButton::clicked, this, &MainWindow::on_objectiveComDisconnectButton_clicked);
    connect(ui->objectiveLensFileButton, &QPushButton::clicked, this, &MainWindow::on_objectiveLensFileButton_clicked);
    connect(ui->objectiveSetAppertureButton, &QPushButton::clicked, this, &MainWindow::on_objectiveSetAppertureButton_clicked);
    connect(ui->objectiveSetFocusButton, &QPushButton::clicked, this, &MainWindow::on_objectiveSetFocusButton_clicked);
    connect(ui->objectiveGetFocusButton, &QPushButton::clicked, this, &MainWindow::on_objectiveGetFocusButton_clicked);

    connect(this, SIGNAL(focusGetted(double)), this, SLOT(on_focusGetted(double)),
            Qt::QueuedConnection);
}

MainWindow::~MainWindow() {
    delete ui;
}

// **************************** Slots ********************************* //

void MainWindow::on_imageReady(cv::Mat img) {
    showImage(img);
}

void MainWindow::on_captureFinished() {
    ui->cameraStartCaptureButton->setEnabled(true);
    ui->cameraStopCaptureButton->setEnabled(false);
    ui->cameraDepthComboBox->setEnabled(true);
}

void MainWindow::on_focusGetted(double val) {
    ui->objectiveFocusValSpinbox->setValue(val);

    if(!mObjective->currentError().empty())
        ui->objectiveErrorLineEdit->setText(QString::fromStdString(mObjective->currentError()));

    ui->objectiveLensFileButton->setEnabled(true);
    ui->objectiveSetAppertureButton->setEnabled(true);
    ui->objectiveSetFocusButton->setEnabled(true);
    ui->objectiveGetFocusButton->setEnabled(true);
}

// ****************************** Camera ****************************** //

void MainWindow::initializePipeline() {
    CamParameters params = mCamera->getCameraParameters();
    quint32 length = (params.mMaximgh * params.mMaximgw) * 2;
    mPipeline = new ImagePipeline(length);
}

void MainWindow::showImage(cv::Mat& image) {
    ui->imageLabel->setPixmap(QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step,
                                                        QImage::Format_Grayscale8)));

    cv::namedWindow("Camera image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Camera image", 600, 600);
    cv::imshow("Camera image", image);
    cv::waitKey(0);
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
   ui->cameraStartCaptureButton->setEnabled(true);
}

void MainWindow::startSingleCapture() {
    if(mCamera->startSingleCapture()){
        auto firstFrame = mPipeline->getFirstFrame();
        if(mCamera->getImage(firstFrame->w, firstFrame->h, firstFrame->bpp,
                             firstFrame->channels, firstFrame->data)) {
            firstFrame->time = std::chrono::steady_clock::now();
            processImage();
        }
        else
            QMessageBox::warning(this, "Внимание", "Ошибка чтения кадра!\n");
    }
    else
        QMessageBox::warning(this, "Внимание", "Не удалось начать съемку!\n");
    emit captureFinished();
}

void MainWindow::startLiveCapture() {
    if(mCamera->startLiveCapture()) {
        bool ready = false;
        auto frame = mPipeline->getFirstFrame();
        while(mCamera->status() == liveCapture) {
            qDebug() << "startLiveCapture" << frame->data;
            while(ready == false && mCamera->status() == liveCapture)
                ready = mCamera->getImage(frame->w, frame->h, frame->bpp,
                                          frame->channels, frame->data);
            frame->time = std::chrono::steady_clock::now();
            processNewImage = true;
            frame = mPipeline->nextFrame(frame);
        }
    }
    else
        QMessageBox::warning(this, "Внимание", "Ошибка чтения кадра!\n");
    emit captureFinished();
}

void MainWindow::processImage() {
    auto it = mPipeline->getFirstFrame();
    while(isConnected) {
        if(processNewImage){
            qDebug() << "Process image" << it->data;

            /*
            processNewImage = false;
            int type = ImageProcess::getOpenCvType((BitMode)it->bpp, it->channels);
            cv::Mat img = cv::Mat(it->h, it->w, type, it->data);
            emit imageReady(img);*/
            it = mPipeline->nextFrame(it);
        }
    }
}

// ************************** Camera Handler ************************** //

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
    try {
        mCamera = new CameraQHYCCD(id);
    }
    catch(...){
        QMessageBox::warning(this, "Внимание", "Ошибка подключения камеры!\n");
    }

    StreamMode mode = (ui->modeCameraComboBox->currentIndex() == 0) ? single : live;
    if(mCamera->connect(mode)) {
        initializeCameraControls(mCamera);
        ui->cameraComboBox->setEnabled(false);
        ui->connectCameraButton->setEnabled(false);
        ui->modeCameraComboBox->setEnabled(false);

        ui->disconnectCameraButton->setEnabled(true);
        ui->cameraCaptureGroupBox->setEnabled(true);

        initializePipeline();

        isConnected = true;
//        auto lambda = [this]() {
//            processImage();
//        };

//        QtConcurrent::run(lambda);
    }
}

void MainWindow::on_disconnectCameraButton_clicked() {
    delete mPipeline;
}

void MainWindow::on_cameraGainDSpinBox_valueChanged(double val) {
    ui->cameraGainHSlider->setValue(val);
//    mCamera->setGain(val);
}

void MainWindow::on_cameraGainHSlider_valueChanged(int val) {
    ui->cameraGainDSpinBox->setValue(val);    
}

void MainWindow::on_cameraExposureDSpinBox_valueChanged(double val) {
    ui->cameraExposureHSlider->setValue(val / 1000);
//    mCamera->setExposure(val);

}

void MainWindow::on_cameraExposureHSlider_valueChanged(int val){
    ui->cameraExposureDSpinBox->setValue(val * 1000);
}

void MainWindow::on_cameraDepthComboBox_currentIndexChanged(int index) {
    if(mCamera->status() != idle)
        return;
    if(ui->cameraDepthComboBox->currentIndex() == 0)
        mCamera->setImageBitMode(bit8);
    else
        mCamera->setImageBitMode(bit16);
}

void MainWindow::on_cameraSetRoiButton_clicked() {
    quint32 startX = ui->cameraStartXSpinBox->value();
    quint32 sizeX = ui->cameraSizeXSpinBox->value();

    quint32 startY = ui->cameraStartYSpinBox->value();
    quint32 sizeY = ui->cameraSizeYSpinBox->value();

    CamParameters params = mCamera->getCameraParameters();

    // Если все нули
    if((startX + startY + sizeX + sizeY) == 0)
        mCamera->setImageSize(0, 0, params.mMaximgw, params.mMaximgh);
    else if((startX + sizeX <= params.mMaximgw) && (startY + sizeY <= params.mMaximgh))
        mCamera->setImageSize(startX, startY, sizeX, sizeY);
    else
        QMessageBox::warning(this, "Внимание", "Заданный размер выходит за пределы допустимого размера!\n");
}

void MainWindow::on_cameraStartCaptureButton_clicked() {
    if(mCamera->status() != idle)
        return;

    auto lambda = [this]() {
        if(mCamera->startLiveCapture()) {
            bool ready = false;
            auto frame = mPipeline->getFirstFrame();
            forever {
                qDebug() << "startLiveCapture" << frame->data;
                if(!isConnected)
                    return;
            }
//            while(mCamera->status() == liveCapture) {
//                qDebug() << "startLiveCapture" << frame->data;
//                while(ready == false && mCamera->status() == liveCapture)
//                    ready = mCamera->getImage(frame->w, frame->h, frame->bpp,
//                                              frame->channels, frame->data);
//                frame->time = std::chrono::steady_clock::now();
//                processNewImage = true;
//                frame = mPipeline->nextFrame(frame);
//            }
        }
        else
            QMessageBox::warning(this, "Внимание", "Ошибка чтения кадра!\n");
        emit captureFinished();

//        CamParameters params = mCamera->getCameraParameters();
//        if(params.mIsLiveMode)
//            startLiveCapture();
//        else
//            startSingleCapture();
    };

    ui->cameraStartCaptureButton->setEnabled(false);
    ui->cameraStopCaptureButton->setEnabled(true);
    ui->cameraDepthComboBox->setEnabled(false);
    QtConcurrent::run(lambda);
}

void MainWindow::on_cameraStopCaptureButton_clicked() {
    isConnected =false;
    ui->cameraStopCaptureButton->setEnabled(false);
    CamParameters params = mCamera->getCameraParameters();
    if(params.mIsLiveMode)
        mCamera->stopLiveCapture();
    else
        mCamera->stopSingleCapture();
}

// **************************** Objective ****************************** //

void MainWindow::on_objectiveComFindButton_clicked() {
    auto ports = QSerialPortInfo::availablePorts();
    for (qint32 i = 0; i < ports.size(); ++i)
        ui->objectiveComPortsComboBox->addItem(ports[i].portName());
}

void MainWindow::on_objectiveComConnectButton_clicked() {
    if(ui->objectiveComPortsComboBox->currentIndex() == -1)
        return;

    try {
        QByteArray ba = ui->objectiveComPortsComboBox->currentText().toLocal8Bit();
        const char* serialPort = ba.data();
        mObjective = new ObjectiveController(serialPort);
        if(mObjective->connectToController(serialPort)) {
            ui->objectiveComPortsComboBox->setEnabled(false);
            ui->objectiveComConnectButton->setEnabled(false);

            ui->objectiveComDisconnectButton->setEnabled(true);
            ui->objectiveLensFileButton->setEnabled(true);
            ui->objectiveSetAppertureButton->setEnabled(true);
            ui->objectiveSetAppertureComboBox->setEnabled(true);
            ui->objectiveSetFocusButton->setEnabled(true);
            ui->objectiveSetFocusSpinBox->setEnabled(true);
            ui->objectiveGetFocusButton->setEnabled(true);
            ui->objectiveFocusValSpinbox->setEnabled(true);

            auto appertures = mObjective->getAppertures();
            for (auto& i : appertures)
                ui->objectiveSetAppertureComboBox->addItem(QString::number(i));
        }
        else
            QMessageBox::warning(this, "Внимание", "Ошибка подключения объектива!\nВозможно неверно указан COM порт.");
    }
    catch(...){
        QMessageBox::warning(this, "Внимание", "Ошибка подключения объектива!\nВозможно неверно указан COM порт.");
    }
}

void MainWindow::on_objectiveComDisconnectButton_clicked() {
    if(mObjective->disconnectController()) {
        ui->objectiveComPortsComboBox->setEnabled(true);
        ui->objectiveComConnectButton->setEnabled(true);

        ui->objectiveComDisconnectButton->setEnabled(false);
        ui->objectiveLensFileButton->setEnabled(false);
        ui->objectiveSetAppertureButton->setEnabled(false);
        ui->objectiveSetAppertureComboBox->setEnabled(false);
        ui->objectiveSetFocusButton->setEnabled(false);
        ui->objectiveSetFocusSpinBox->setEnabled(false);
        ui->objectiveGetFocusButton->setEnabled(false);
        ui->objectiveFocusValSpinbox->setEnabled(false);
    }
}

void MainWindow::on_objectiveLensFileButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Аппертуры"), "/home", tr("JSON Files (*.json)"));
    ui->objectiveLensFileLineEdit->setText(fileName);

}

void MainWindow::on_objectiveSetAppertureButton_clicked() {
    ui->objectiveLensFileButton->setEnabled(false);
    ui->objectiveSetAppertureButton->setEnabled(false);
    ui->objectiveSetFocusButton->setEnabled(false);
    ui->objectiveGetFocusButton->setEnabled(false);

    ui->objectiveErrorLineEdit->clear();
    int val = ui->objectiveSetAppertureComboBox->currentIndex();
    mObjective->setDiaphragmLevel(val);
    if(!mObjective->currentError().empty())
        ui->objectiveErrorLineEdit->setText(QString::fromStdString(mObjective->currentError()));

    ui->objectiveLensFileButton->setEnabled(true);
    ui->objectiveSetAppertureButton->setEnabled(true);
    ui->objectiveSetFocusButton->setEnabled(true);
    ui->objectiveGetFocusButton->setEnabled(true);
}

void MainWindow::on_objectiveSetFocusButton_clicked() {
    ui->objectiveLensFileButton->setEnabled(false);
    ui->objectiveSetAppertureButton->setEnabled(false);
    ui->objectiveSetFocusButton->setEnabled(false);
    ui->objectiveGetFocusButton->setEnabled(false);

    ui->objectiveErrorLineEdit->clear();
    int val = ui->objectiveSetFocusSpinBox->value();
    mObjective->setFocusing(val);
    if(!mObjective->currentError().empty())
        ui->objectiveErrorLineEdit->setText(QString::fromStdString(mObjective->currentError()));

    ui->objectiveLensFileButton->setEnabled(true);
    ui->objectiveSetAppertureButton->setEnabled(true);
    ui->objectiveSetFocusButton->setEnabled(true);
    ui->objectiveGetFocusButton->setEnabled(true);
}

void MainWindow::on_objectiveGetFocusButton_clicked() {
    ui->objectiveLensFileButton->setEnabled(false);
    ui->objectiveSetAppertureButton->setEnabled(false);
    ui->objectiveSetFocusButton->setEnabled(false);
    ui->objectiveGetFocusButton->setEnabled(false);
    ui->objectiveErrorLineEdit->clear();

    auto lambda = [this]() {
        double currentFocusing = mObjective->getCurrentFocusing();
        emit focusGetted(currentFocusing);
    };
    QtConcurrent::run(lambda);
}


