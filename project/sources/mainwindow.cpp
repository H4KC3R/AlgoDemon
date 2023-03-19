     #include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QGraphicsView>

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

    initializeDisplay();
    uiSignalSlotsInit();
    objectiveSignalSlotsInit();
    setInitialGUIState();
    initializeStructures();
}

MainWindow::~MainWindow() {
    delete ui;
}

// **************************** Slots ********************************* //

void MainWindow::onProcessFinished() {
    ui->cameraStartCaptureButton->setEnabled(true);
    ui->cameraStopCaptureButton->setEnabled(false);
}

void MainWindow::onHardFault(QString errorMsg) {
    processor.stopProcess();
    QMessageBox::warning(this, "Внимание", errorMsg);
}

void MainWindow::onSoftFault(QString errorMsg) {
    QMessageBox::warning(this, "Внимание", errorMsg);
}

void MainWindow::updateFrame(const QImage &frame){
    displayScene->addPixmap(QPixmap::fromImage(frame));
}

void MainWindow::updateFocusingResult(const QImage &frame, double position) {
    ui->cameraFocusRoiLabel->setPixmap(QPixmap::fromImage(frame));
    ui->objectiveFocusValSpinbox->setValue(position);
}

void MainWindow::updateEG(double gain, double exposure){
    ui->cameraGainDSpinBox->setValue(gain);
    ui->cameraExposureDSpinBox->setValue(exposure);
}

void MainWindow::onObjectiveError(QString msg) {
    ui->objectiveErrorLineEdit->clear();
    ui->objectiveErrorLineEdit->setText(msg);
}

void MainWindow::onImageProcessingError(QString msg) {
    QMessageBox::warning(this, "Внимание", msg);
}

// *********************************************************************** //

void MainWindow::uiSignalSlotsInit() {
    //  Camera Connection
    connect(ui->findCamerasButton, &QPushButton::clicked, this, &MainWindow::on_findCamerasButton_clicked);
    connect(ui->connectCameraButton, &QPushButton::clicked, this, &MainWindow::on_connectCameraButton_clicked);
    connect(ui->disconnectCameraButton, &QPushButton::clicked, this, &MainWindow::on_disconnectCameraButton_clicked);

    //  Camera Settings
    connect(ui->cameraBitComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::on_cameraBitComboBox_currentIndexChanged);
    connect(ui->cameraFpsSpinBox, &QSpinBox::valueChanged, this, &MainWindow::on_cameraFpsSpinBox_valueChanged);
    connect(ui->cameraGainDSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_cameraGainDSpinBox_valueChanged);
    connect(ui->cameraGainHSlider, &QSlider::valueChanged, this, &MainWindow::on_cameraGainHSlider_valueChanged);
    connect(ui->cameraExposureDSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_cameraExposureDSpinBox_valueChanged);
    connect(ui->cameraExposureHSlider, &QSlider::valueChanged, this, &MainWindow::on_cameraExposureHSlider_valueChanged);
    connect(ui->cameraSetRoiButton, &QPushButton::clicked, this, &MainWindow::on_cameraSetRoiButton_clicked);

    // Image Process
    connect(ui->debayerCheckBox, &QCheckBox::clicked, this, &MainWindow::on_debayerCheckBox_clicked);
    connect(ui->whiteBalanceCheckBox, &QCheckBox::clicked, this, &MainWindow::on_whiteBalanceCheckBox_clicked);
    connect(ui->contrastEnableCheckBox, &QCheckBox::clicked, this, &MainWindow::on_contrastEnableCheckBox_clicked);
    connect(ui->contrastAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_contrastAlphaSpinBox_valueChanged);
    connect(ui->gammaContrastEnableCheckBox, &QCheckBox::clicked, this, &MainWindow::on_gammaContrastEnableCheckBox_clicked);
    connect(ui->gammaCoeffSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_gammaCoeffSpinBox_valueChanged);

    // Camera Image Capture
    connect(ui->cameraStartCaptureButton, &QPushButton::clicked, this, &MainWindow::on_cameraStartCaptureButton_clicked);
    connect(ui->cameraStopCaptureButton, &QPushButton::clicked, this, &MainWindow::on_cameraStopCaptureButton_clicked);

    // Camera Focusing
    connect(ui->cameraEnableFocusCheckBox, &QCheckBox::clicked, this, &MainWindow::on_cameraEnableFocus_cliked);
    connect(ui->cameraFocusButton, &QPushButton::clicked, this, &MainWindow::on_cameraFocusButton_clicked);
\
    // Camera Autoexposure
    connect(ui->autoExposureCheckBox, &QCheckBox::clicked, this, &MainWindow::on_autoExposureCheckBox_clicked);
    connect(ui->maxPercentHSlider, &QSlider::valueChanged, this, &MainWindow::on_maxPercentHSlider_valueChanged);
    connect(ui->meanHSlider, &QSlider::valueChanged, this, &MainWindow::on_meanHSlider_valueChanged);
    connect(ui->maxRelCoeffHSlider, &QSlider::valueChanged, this, &MainWindow::on_maxRelCoeffHSlider_valueChanged);
    connect(ui->minRelCoeffHSlider, &QSlider::valueChanged, this, &MainWindow::on_minRelCoeffHSlider_valueChanged);

    // Objective Control
    connect(ui->objectiveComFindButton, &QPushButton::clicked, this, &MainWindow::on_objectiveComFindButton_clicked);
    connect(ui->objectiveComConnectButton, &QPushButton::clicked, this, &MainWindow::on_objectiveComConnectButton_clicked);
    connect(ui->objectiveComDisconnectButton, &QPushButton::clicked, this, &MainWindow::on_objectiveComDisconnectButton_clicked);
    connect(ui->objectiveLensFileButton, &QPushButton::clicked, this, &MainWindow::on_objectiveLensFileButton_clicked);
    connect(ui->objectiveSetAppertureButton, &QPushButton::clicked, this, &MainWindow::on_objectiveSetAppertureButton_clicked);
    connect(ui->objectiveSetFocusButton, &QPushButton::clicked, this, &MainWindow::on_objectiveSetFocusButton_clicked);
    connect(ui->objectiveGetFocusButton, &QPushButton::clicked, this, &MainWindow::on_objectiveGetFocusButton_clicked);
}

void MainWindow::setInitialGUIState() {
    ui->findCamerasButton->setEnabled(true);
    ui->cameraComboBox->setEnabled(true);
    ui->modeCameraComboBox->setEnabled(true);
    ui->connectCameraButton->setEnabled(true);
    ui->disconnectCameraButton->setEnabled(false);

    ui->cameraBitComboBox->setEnabled(false);
    ui->cameraFpsSpinBox->setEnabled(false);
    ui->cameraGainDSpinBox->setEnabled(false);
    ui->cameraGainHSlider->setEnabled(false);
    ui->cameraExposureDSpinBox->setEnabled(false);
    ui->cameraExposureHSlider->setEnabled(false);
    ui->cameraRoiGroupBox->setEnabled(false);

    ui->cameraStopCaptureButton->setEnabled(false);
    ui->cameraStartCaptureButton->setEnabled(false);

    ui->cameraEnableFocusCheckBox->setEnabled(false);
    ui->cameraFocusButton->setEnabled(false);

    ui->debayerCheckBox->setEnabled(false);
    ui->whiteBalanceCheckBox->setEnabled(false);
    ui->contrastEnableCheckBox->setEnabled(false);
    ui->contrastAlphaSpinBox->setEnabled(false);
    ui->gammaContrastEnableCheckBox->setEnabled(false);
    ui->gammaCoeffSpinBox->setEnabled(false);

    ui->autoExposureCheckBox->setEnabled(false);
    ui->maxPercentHSlider->setEnabled(false);
    ui->maxRelCoeffHSlider->setEnabled(false);
    ui->meanHSlider->setEnabled(false);
    ui->minRelCoeffHSlider->setEnabled(false);
}

void MainWindow::initializeDisplay() {
    displayView = new dororo::GraphicsView(this);
    ui->horizontalLayout_7->addWidget(displayView);

    displayScene = new QGraphicsScene(this);
    displayView->setScene(displayScene);

    QPixmap img(":/resources/stars.jpg");
    imageMapItem = displayScene->addPixmap(img);
    roiController = new dororo::ViewportController(imageMapItem);

    roiController->setRect(100, 100, 250, 250);
    roiController->setPen(QPen(Qt::GlobalColor::yellow));

    roiController->hide();
}

void MainWindow::proccessorSignalSlotsInit() {
    //  Processor (emitter) and GUI thread (receiver/listener)
    connect(&processor, SIGNAL(processFinished()), this, SLOT(onProcessFinished()));

    //  Camera thread (emitter) and GUI thread (receiver/listener)
    connect(processor.cameraThread, SIGNAL(hardFault(QString)), this, SLOT(onHardFault(QString)));
    connect(processor.cameraThread, SIGNAL(softFault(QString)), this, SLOT(onSoftFault(QString)));

    //  Processing thread (emitter) and GUI thread (receiver/listener)
    connect(processor.processingThread, SIGNAL(newFrame(QImage)), this, SLOT(updateFrame(QImage)));

    // GUI thread (emitter) and Camera thread (receiver/listener)
    connect(this, SIGNAL(bitChanged(BitMode)),
            processor.cameraThread, SLOT(onBitChanged(BitMode)));
    connect(this, SIGNAL(fpsChanged(double)),
            processor.cameraThread, SLOT(onFpsChanged(double)));
    connect(this, SIGNAL(EGChanged(double,double)),
            processor.cameraThread, SLOT(onEGChanged(double,double)));
    connect(this, SIGNAL(roiChanged(RoiBox)),
            processor.cameraThread, SLOT(onRoiChanged(RoiBox)));

    // GUI thread (emitter) and Processing thread (receiver/listener)
    connect(this, SIGNAL(newImageProcessingFlags(ImageProcessingFlags)),
            processor.processingThread, SLOT(updateImageProcessingSettings(ImageProcessingFlags)));
}

void MainWindow::objectiveSignalSlotsInit() {
    //  Objective thread (emitter) and GUI thread (receiver/listener)
    connect(processor.objectiveThread, SIGNAL(newFocusingResult(QImage,double)),
            this, SLOT(updateFocusingResult(QImage,double)));
    connect(processor.objectiveThread, SIGNAL(newEGValues(double,double)),
            this, SLOT(updateEG(double,double)));
    connect(processor.objectiveThread, SIGNAL(objectiveError(QString)),
            this, SLOT(onObjectiveError(QString)));
    connect(processor.objectiveThread, SIGNAL(imageProcessingError(QString)),
            this, SLOT(onImageProcessingError(QString)));

    // GUI thread (emitter) and Objective thread (receiver/listener)
    connect(this, SIGNAL(autoExposureEnabled(bool,double,double)),
            processor.objectiveThread, SLOT(onAutoExposureEnabled(bool,double,double)));
    connect(this, SIGNAL(autoExposureSettingChanged(AutoExposureParams)),
            processor.objectiveThread, SLOT(onAutoExposureSettingChanged(AutoExposureParams)));
    connect(this, SIGNAL(focusingEnabled(bool,cv::Rect)),
            processor.objectiveThread, SLOT(onFocusingEnabled(bool,cv::Rect)));
}

void MainWindow::initializeCameraControls() {
    double min, max, step, currentVal;
    CamParameters camParameters = processor.cameraThread->getParams();

    ///////////////////// BitMode ///////////////////
    if(processor.cameraThread->getControlSettings(transferbit, min, max, step, currentVal)) {
        ui->cameraBitComboBox->setEnabled(true);
        BitMode currentMode =(BitMode)currentVal;
        // Почему то, в режиме фото при 8 битах не получается снять кадр
        if(!camParameters.mIsLiveMode)
            ui->cameraBitComboBox->removeItem(0);
        else {
            if (currentMode == bit8)
                ui->cameraBitComboBox->setCurrentIndex(0);
            else
                ui->cameraBitComboBox->setCurrentIndex(1);
        }
    }
    ////////////////////////////////////////////////

    ////////////////////// FPS /////////////////////
    if(processor.cameraThread->getControlSettings(fps, min, max, step, currentVal)) {
        ui->cameraFpsSpinBox->setEnabled(true);
        ui->cameraFpsSpinBox->setMaximum(max);
        ui->cameraFpsSpinBox->setMinimum(min);
        ui->cameraFpsSpinBox->setSingleStep(step);
        ui->cameraFpsSpinBox->setValue(currentVal);
    }
    ////////////////////////////////////////////////

    ///////////////////// Gain ////////////////////
    if (processor.cameraThread->getControlSettings(gain, min, max, step, currentVal)) {
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

        ui->cameraGainDSpinBox->setValue(currentVal);
    }
    ////////////////////////////////////////////////

    /////////////////// Exposure //////////////////
    if (processor.cameraThread->getControlSettings(exposure, min, max, step, currentVal)) {
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

        ui->cameraExposureDSpinBox->setValue(currentVal);
    }
    ////////////////////////////////////////////////

    /////////////////// CameraType ////////////////
    if(camParameters.mIsMono) {
        ui->cameraTypeValueLabel->setText("Монохромная");

        ui->debayerCheckBox->setEnabled(false);
        ui->whiteBalanceCheckBox->setEnabled(false);
        ui->contrastAlphaSpinBox->setEnabled(false);
        ui->gammaCoeffSpinBox->setEnabled(false);

        ui->autoExposureCheckBox->setEnabled(true);
        ui->cameraEnableFocusCheckBox->setEnabled(true);

        ui->cameraStartXSpinBox->setMaximum(camParameters.mMaximgw - 1);
        ui->cameraStartYSpinBox->setMaximum(camParameters.mMaximgh - 1);

        ui->cameraSizeXSpinBox->setMaximum(camParameters.mMaximgw);
        ui->cameraSizeYSpinBox->setMaximum(camParameters.mMaximgh);
    }
    else {
        ui->cameraTypeValueLabel->setText("Цветная");

        ui->debayerCheckBox->setEnabled(true);
        ui->whiteBalanceCheckBox->setEnabled(true);
        ui->contrastAlphaSpinBox->setEnabled(true);
        ui->gammaCoeffSpinBox->setEnabled(true);

        ui->autoExposureCheckBox->setEnabled(true);
        ui->cameraEnableFocusCheckBox->setEnabled(true);

        ui->cameraStartXSpinBox->setMaximum(camParameters.mMaximgw - 1);
        ui->cameraStartYSpinBox->setMaximum(camParameters.mMaximgh - 1);

        ui->cameraSizeXSpinBox->setMaximum(camParameters.mMaximgw);
        ui->cameraSizeYSpinBox->setMaximum(camParameters.mMaximgh);
    }
    ////////////////////////////////////////////////

    ui->cameraRoiGroupBox->setEnabled(true);
    ui->cameraStartCaptureButton->setEnabled(true);

    ui->cameraComboBox->setEnabled(false);
    ui->connectCameraButton->setEnabled(false);
    ui->modeCameraComboBox->setEnabled(false);

    ui->disconnectCameraButton->setEnabled(true);
    ui->cameraCaptureGroupBox->setEnabled(true);
}

void MainWindow::initializeStructures() {
    param.maxPercent = ui->maxPercentHSlider->value();
    param.mean = ui->meanHSlider->value();
    param.maxRelCoeff = ui->maxRelCoeffHSlider->value();
    param.minRelCoef = ui->minRelCoeffHSlider->value();

    imageProcessingFlags.debayerOn = false;
    imageProcessingFlags.whiteBalanceOn = false;
    imageProcessingFlags.contrastOn = false;
    imageProcessingFlags.contrastValue = ui->contrastAlphaSpinBox->value();
    imageProcessingFlags.gammaContrastOn = false;
    imageProcessingFlags.gammaContrastValue = ui->gammaCoeffSpinBox->value();
}

// ************************** Camera Connection ************************** //

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

    StreamMode mode = (ui->modeCameraComboBox->currentIndex() == 0) ? single : live;
    if(processor.connectToCamera(id, mode)) {
        initializeCameraControls();
        proccessorSignalSlotsInit();
    }
    else
        QMessageBox::warning(this, "Внимание", "Ошибка подключения камеры!\n");
}

void MainWindow::on_disconnectCameraButton_clicked() {
    if(processor.cameraThread->isCameraConnected()) {
        //  Processor (emitter) and GUI thread (receiver/listener)
        disconnect(&processor, SIGNAL(processFinished()), this, SLOT(onProcessFinished()));

        //  Camera thread (emitter) and GUI thread (receiver/listener)
        disconnect(processor.cameraThread, SIGNAL(hardFault(QString)), this, SLOT(onHardFault(QString)));
        disconnect(processor.cameraThread, SIGNAL(softFault(QString)), this, SLOT(onSoftFault(QString)));

        //  Processing thread (emitter) and GUI thread (receiver/listener)
        disconnect(processor.processingThread, SIGNAL(newFrame(QImage)), this, SLOT(updateFrame(QImage)));

        // GUI thread (emitter) and Camera thread (receiver/listener)
        disconnect(this, SIGNAL(bitChanged(BitMode)),
                processor.cameraThread, SLOT(onBitChanged(BitMode)));
        disconnect(this, SIGNAL(fpsChanged(double)),
                processor.cameraThread, SLOT(onFpsChanged(double)));
        disconnect(this, SIGNAL(EGChanged(double,double)),
                processor.cameraThread, SLOT(onEGChanged(double,double)));
        disconnect(this, SIGNAL(roiChanged(RoiBox)),
                processor.cameraThread, SLOT(onRoiChanged(RoiBox)));

        // GUI thread (emitter) and Processing thread (receiver/listener)
        disconnect(this, SIGNAL(newImageProcessingFlags(ImageProcessingFlags)),
                processor.processingThread, SLOT(updateImageProcessingSettings(ImageProcessingFlags)));

        setInitialGUIState();
        processor.disconnectCamera();
    }
    else
        QMessageBox::warning(this,"Внимание","Камера отключена.");
}

// ************************** Camera Settings *************************** //

void MainWindow::on_cameraBitComboBox_currentIndexChanged(int) {
    if(ui->cameraBitComboBox->currentIndex() == 0)
        emit bitChanged(bit8);
    else
        emit bitChanged(bit16);
}

void MainWindow::on_cameraFpsSpinBox_valueChanged(int value) {
    emit fpsChanged(value);
}

void MainWindow::on_cameraGainDSpinBox_valueChanged(double val) {
    ui->cameraGainHSlider->setValue(val);
    emit EGChanged(ui->cameraGainDSpinBox->value(),
                   ui->cameraExposureDSpinBox->value());
}

void MainWindow::on_cameraGainHSlider_valueChanged(int val) {
    ui->cameraGainDSpinBox->setValue(val);    
}

void MainWindow::on_cameraExposureDSpinBox_valueChanged(double val) {
    ui->cameraExposureHSlider->setValue(val / 1000);
    emit EGChanged(ui->cameraGainDSpinBox->value(),
                   ui->cameraExposureDSpinBox->value());
}

void MainWindow::on_cameraExposureHSlider_valueChanged(int val){
    ui->cameraExposureDSpinBox->setValue(val * 1000);
}

void MainWindow::on_cameraSetRoiButton_clicked() {
    RoiBox roi;
    roi.startX = ui->cameraStartXSpinBox->value();
    roi.sizeX = ui->cameraSizeXSpinBox->value();

    roi.startY = ui->cameraStartYSpinBox->value();
    roi.sizeY = ui->cameraSizeYSpinBox->value();

    emit roiChanged(roi);
}

// ************************** Image Processing ************************** //

void MainWindow::on_debayerCheckBox_clicked(bool enabled) {
    imageProcessingFlags.debayerOn = enabled;
    emit newImageProcessingFlags(imageProcessingFlags);
}

void MainWindow::on_whiteBalanceCheckBox_clicked(bool enabled) {
    imageProcessingFlags.whiteBalanceOn = enabled;
    emit newImageProcessingFlags(imageProcessingFlags);
}

void MainWindow::on_contrastEnableCheckBox_clicked(bool enabled) {
    imageProcessingFlags.contrastOn = enabled;
    ui->contrastAlphaSpinBox->setEnabled(enabled);
    imageProcessingFlags.contrastValue = ui->contrastAlphaSpinBox->value();
    emit newImageProcessingFlags(imageProcessingFlags);
}

void MainWindow::on_contrastAlphaSpinBox_valueChanged(double value) {
    imageProcessingFlags.contrastValue = value;
    emit newImageProcessingFlags(imageProcessingFlags);
}

void MainWindow::on_gammaContrastEnableCheckBox_clicked(bool enabled) {
    imageProcessingFlags.gammaContrastOn = enabled;
    ui->gammaCoeffSpinBox->setEnabled(enabled);
    imageProcessingFlags.gammaContrastValue = ui->gammaCoeffSpinBox->value();
    emit newImageProcessingFlags(imageProcessingFlags);
}

void MainWindow::on_gammaCoeffSpinBox_valueChanged(double value) {
    imageProcessingFlags.gammaContrastValue = value;
    emit newImageProcessingFlags(imageProcessingFlags);
}

// ************************ Camera Image Capture ************************ //

void MainWindow::on_cameraStartCaptureButton_clicked() {
    processor.runProcess();
    ui->cameraStartCaptureButton->setEnabled(false);
    ui->cameraStopCaptureButton->setEnabled(true);
}

void MainWindow::on_cameraStopCaptureButton_clicked() {
    processor.stopProcess();
}

// ************************** Camera Focusing *************************** //

void MainWindow::on_cameraEnableFocus_cliked(bool enabled) {
    if(enabled)
        roiController->show();
    else
        roiController->hide();
}

void MainWindow::on_cameraFocusButton_clicked() {
    QRectF roi = roiController->rect();
    double x, y, width, height;
    roi.getRect(&x, &y, &width, &height);
    bool status = ui->cameraEnableFocusCheckBox->isChecked();

    emit focusingEnabled(status, cv::Rect(x, y, width, height));
}

// ************************ Camera AutoExposure ************************* //

void MainWindow::on_autoExposureCheckBox_clicked() {
    bool status = ui->autoExposureCheckBox->isChecked();
    double gain = ui->cameraGainDSpinBox->value();
    double exposure = ui->cameraExposureDSpinBox->value();
    emit autoExposureEnabled(status, gain, exposure);
}

void MainWindow::on_maxPercentHSlider_valueChanged(int value) {
    param.maxPercent = value;
    emit autoExposureSettingChanged(param);
}

void MainWindow::on_meanHSlider_valueChanged(int value) {
    param.mean = value;
    emit autoExposureSettingChanged(param);
}

void MainWindow::on_maxRelCoeffHSlider_valueChanged(int value) {
    param.maxRelCoeff = value;
    emit autoExposureSettingChanged(param);
}

void MainWindow::on_minRelCoeffHSlider_valueChanged(int value) {
    param.minRelCoef = value;
    emit autoExposureSettingChanged(param);
}

// ************************* Objective Control ************************** //

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
        if(processor.objectiveThread->connectObjective(serialPort)) {
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

            auto appertures = processor.objectiveThread->getAppertureList();
            ui->objectiveSetAppertureComboBox->clear();
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
    if(processor.objectiveThread->disconnectObjective()) {
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

    QFile file;
    file.setFileName(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QByteArray bytes = file.readAll();
        file.close();

        /////////////////////  JSON PARSER  //////////////////////
        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson( bytes, &jsonError );
        if(jsonError.error != QJsonParseError::NoError ) {
            QMessageBox::warning(this, "Внимание", "Ошибка чтения JSON файла!\n");
            return ;
        }

        if(document.isObject()) {
            QJsonObject jsonObj = document.object();
            if(jsonObj.keys().indexOf("appertures") == -1)
                QMessageBox::warning(this, "Внимание",
                                     "Неверный формат JSON файла!\n");
            else {
                auto value = jsonObj.take("appertures");
                if(value.isArray()) {
                    auto jsonAppertures = value.toArray();
                    std::vector<double> appertures;
                    ui->objectiveSetAppertureComboBox->clear();

                    foreach (const QJsonValue &v, jsonAppertures) {
                        appertures.push_back(v.toDouble());
                        ui->objectiveSetAppertureComboBox->addItem(QString::number(v.toDouble()));
                    }

                    processor.objectiveThread->setAppertureList(appertures);
                    ui->objectiveLensFileLineEdit->setText(fileName);
                }
            }
        }
    }
    //////////////////////////////////////////////////////////
}

void MainWindow::on_objectiveSetAppertureButton_clicked() {
    ui->objectiveLensFileButton->setEnabled(false);
    ui->objectiveSetAppertureButton->setEnabled(false);
    ui->objectiveSetFocusButton->setEnabled(false);
    ui->objectiveGetFocusButton->setEnabled(false);

    ui->objectiveErrorLineEdit->clear();

    QString strVal = ui->objectiveSetAppertureComboBox->currentText();
    double doubleVal = strVal.toDouble();

    string error = processor.objectiveThread->setDiaphragmLevel(doubleVal);
    if(!error.empty())
        ui->objectiveErrorLineEdit->setText(QString::fromStdString(error));

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
    string error = processor.objectiveThread->setFocusing(val);
    if(!error.empty())
        ui->objectiveErrorLineEdit->setText(QString::fromStdString(error));

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

    double focusingPosition;
    string error = processor.objectiveThread->getCurrentFocusing(focusingPosition);

    if(!error.empty())
        ui->objectiveErrorLineEdit->setText(QString::fromStdString(error));

    ui->objectiveLensFileButton->setEnabled(true);
    ui->objectiveSetAppertureButton->setEnabled(true);
    ui->objectiveSetFocusButton->setEnabled(true);
    ui->objectiveGetFocusButton->setEnabled(true);
}

// *********************************************************************** //

void MainWindow::showEvent(QShowEvent*)
{
    if(!displayScene)
        return;
    QRectF bounds = displayScene->itemsBoundingRect();
    displayView->fitInView(bounds, Qt::KeepAspectRatio);
}

void MainWindow::resizeEvent(QResizeEvent*) {
    if(!displayScene)
        return;
    QRectF bounds = displayScene->itemsBoundingRect();
    displayView->fitInView(bounds, Qt::KeepAspectRatio);
}


