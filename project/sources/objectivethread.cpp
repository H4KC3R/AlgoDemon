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
    focusingFSM = nullptr;
    pObjective = nullptr;
    autoExposureHandler = new AutoExposureHandler(params, maxExposure, minExposure, maxGain, minGain);
    stopped = false;
    mAutoExposureOn = false;
    mFocusingOn = false;
    currentPosition = 0;
}

ObjectiveThread::~ObjectiveThread() {
    if(focusingFSM) {
        delete focusingFSM;
        focusingFSM = nullptr;
    }

    delete autoExposureHandler;
    if(pObjective)
        delete pObjective;
}

bool ObjectiveThread::connectObjective(const char* serialPort) {
    try {
        pObjective = new ObjectiveController(serialPort);
        if(pObjective->connectToController(serialPort)) {
            focusingFSM = new FocusingFSM(pObjective);
            return true;
        }
        else
            return false;
    }
    catch(...) {
        return false;
    }
}

bool ObjectiveThread::disconnectObjective() {
    delete focusingFSM;
    focusingFSM = nullptr;
    if(pObjective->disconnectController())
        return true;
    else
        return false;
}

string ObjectiveThread::setDiaphragmLevel(double value) {
    pObjective->setDiaphragmLevel(value);
    return pObjective->currentError();
}

string ObjectiveThread::setFocusing(int value) {
    pObjective->setFocusing(value);
    return pObjective->currentError();
}

string ObjectiveThread::getCurrentFocusing(double& value) {
    value = pObjective->getCurrentFocusing();
    return pObjective->currentError();
}

std::vector<double> ObjectiveThread::getAppertureList() {
    return pObjective->getAppertures();
}

void ObjectiveThread::setAppertureList(std::vector<double> appertures) {
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

bool ObjectiveThread::focusingOn() const {
    return mFocusingOn;
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
        maxSharpnessMetric = 0;
        sharpImagePositon = 0;
        myRoi = roi;
        focusingFSM->stopFocus();
    }
    else
        emit focusingStop("Объектив отключен!\n");
}

void ObjectiveThread::run() {
    auto frame = pFramePipeline->getFirstFrame();
    while(1) {
        ////////////////  Остановка потока  /////////////////
        stoppedMutex.lock();
        if (stopped) {
            stopped=false;
            stoppedMutex.unlock();
            break;
        }
        stoppedMutex.unlock();
        /////////////////////////////////////////////////////////////

        updateSettingsMutex.lock();
        // При остановке CameraThread, кадры не помещаются в pipeline
        // и pipeline закрыт на чтение
        if(!pFramePipeline->getPipelineActive()) {
            updateSettingsMutex.unlock();
            continue;
        }

        int type = ImageProcess::getOpenCvType((BitMode)frame->mBpp, frame->mChannels);
        cvFrame = cv::Mat(frame->mHeight, frame->mWidth, type, frame->pData).clone();

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
            else {
                mAutoExposureOn = false;
                emit autoExposureStop("Неверный формат изображения!\nТребуется: 8 бит");
            }
        }

        if(mFocusingOn) {
            double sharpnessMetric;
            if(ImageBlurMetric::getBlurFFT(cvFrame(myRoi), sharpnessMetric)) {
                focusingFSM->focus(sharpnessMetric);
                currentPosition = focusingFSM->getCurrentPosition();
            }
            else {
                mFocusingOn = false;
                emit focusingStop("Изображение должно быть в GrayScale формате!\n");
            }
        }
        ////////////////////////////////////
        // Конец //
        ////////////////////////////////////

        qFrame = MatToQImage(cvFrame(myRoi), cvFrame.type());
        updateSettingsMutex.unlock();

        emit newFocusingResult(qFrame, currentPosition);
        frame = pFramePipeline->nextFrame(frame);
    }
}

