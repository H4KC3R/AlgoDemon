#include "objectivethread.h"
#include "imageprocess.h"
#include "imageblurmetric.h"
#include "mattoqimage.h"

ObjectiveThread::ObjectiveThread(FramePipeline* pipeline, bool isMonoFlag,
                                 double maxExposure, double minExposure,
                                 double maxGain, double minGain,
                                 AutoExposureParams params)
    : QThread(),
      pFramePipeline(pipeline),
      isMono(isMonoFlag)
{
    pObjective = nullptr;
    autoExposureHandler = new AutoExposureHandler(params, maxExposure, minExposure, maxGain, minGain);
    stopped = false;
    mAutoExposureOn = false;
    mFocusingOn = false;
    currentFocusingPosition = 0;
}

ObjectiveThread::~ObjectiveThread() {
    delete autoExposureHandler;
    if(pObjective)
        delete pObjective;
}

double ObjectiveThread::findZero() {
    QMutexLocker locker(&objectiveControlMutex);
    return 0;
}

bool ObjectiveThread::connectObjective(const char* serialPort) {
    QMutexLocker locker(&objectiveControlMutex);
    try {
        pObjective = new ObjectiveController(serialPort);
        if(pObjective->connectToController(serialPort))
            return true;
        else
            return false;
    }
    catch(...) {
        return false;
    }
}

bool ObjectiveThread::disconnectObjective() {
    QMutexLocker locker(&objectiveControlMutex);
    if(pObjective->disconnectController())
        return true;
    else
        return false;
}

string ObjectiveThread::setDiaphragmLevel(double value) {
    QMutexLocker locker(&objectiveControlMutex);
    pObjective->setDiaphragmLevel(value);
    return pObjective->currentError();
}

string ObjectiveThread::setFocusing(int value) {
    QMutexLocker locker(&objectiveControlMutex);
    pObjective->setFocusing(value);
    return pObjective->currentError();
}

string ObjectiveThread::getCurrentFocusing(double& value) {
    QMutexLocker locker(&objectiveControlMutex);
    value = pObjective->getCurrentFocusing();
    currentFocusingPosition = value;
    return pObjective->currentError();
}

std::vector<double> ObjectiveThread::getAppertureList() {
    QMutexLocker locker(&objectiveControlMutex);
    return pObjective->getAppertures();
}

void ObjectiveThread::setAppertureList(std::vector<double> appertures) {
    QMutexLocker locker(&objectiveControlMutex);
    pObjective->setAppertures(appertures);
}

void ObjectiveThread::stopObjectiveThread() {
    stoppedMutex.lock();
    stopped=true;
    stoppedMutex.unlock();
}

bool ObjectiveThread::getIsMono() const {
    return isMono;
}

void ObjectiveThread::setIsMono(bool newIsMono) {
    isMono = newIsMono;
}

void ObjectiveThread::onAutoExposureEnabled(bool status, double gain, double exposure) {
    QMutexLocker locker(&updateSettingsMutex);
    this->mAutoExposureOn = status;
    this->mCurrentGain = gain;
    this->mCurrentExposure = exposure;
}

void ObjectiveThread::onAutoExposureSettingChanged(AutoExposureParams params) {
    QMutexLocker locker(&updateSettingsMutex);
    autoExposureHandler->setParams(params);
}

void ObjectiveThread::onFocusingEnabled(bool status, cv::Rect roi) {
    QMutexLocker locker(&updateSettingsMutex);
    if(pObjective && pObjective->isContollerActive()) {
        this->mFocusingOn = status;
        myRoi = roi;
    }
}

void ObjectiveThread::run() {
    auto frame = pFramePipeline->getFirstFrame();
    while(1) {
           ////////////////  Остановка потока  /////////////////
           stoppedMutex.lock();
           if (stopped) {
               stopped=false;
               qDebug() << "stopped";
               stoppedMutex.unlock();
               break;
           }
           stoppedMutex.unlock();
           /////////////////////////////////////////////////////////////

           updateSettingsMutex.lock();
           int type = ImageProcess::getOpenCvType((BitMode)frame->mBpp, frame->mChannels);
           cvFrame = cv::Mat(frame->mHeight, frame->mWidth, type, frame->pData);

           ///////// GRAYSCALE Формат /////////
           if(!isMono)
               cv::cvtColor(cvFrame, cvFrame, cv::COLOR_BayerRG2GRAY);

           ////////////////////////////////////
           // Обработка изображения //
           ////////////////////////////////////
           if(mAutoExposureOn) {
               if(autoExposureHandler->correct(cvFrame, mCurrentExposure, mCurrentGain)) {
                   mCurrentGain = autoExposureHandler->getGain();
                   mCurrentExposure = autoExposureHandler->getExposure();
                   emit newEGValues(mCurrentGain, mCurrentExposure);
               }
               else
                   emit imageProcessingError("Неверный формат изображения!\nТребуется: 8 бит");
           }

           if(mFocusingOn) {
               double result;
               QMutexLocker locker(&objectiveControlMutex);
               if(ImageBlurMetric::getBlurFFT(cvFrame(myRoi), result)) {
                   currentFocusingPosition = pObjective->getCurrentFocusing();
               }
               else
                   emit imageProcessingError("Изображение должно быть в GrayScale формате!\n");
           }

           ////////////////////////////////////
           // Конец //
           ////////////////////////////////////

           qDebug() << "Objective Thread";
           qFrame = MatToQImage(cvFrame(myRoi));
           updateSettingsMutex.unlock();

           emit newFocusingResult(qFrame, currentFocusingPosition);
           frame = pFramePipeline->nextFrame(frame);
       }
}

