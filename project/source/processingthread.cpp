#include "processingthread.h"
#include "mattoqimage.h"

ProcessingThread::ProcessingThread(FramePipeline *pipeline, bool isMonoFlag,
                                   double maxExposure, double minExposure, double maxGain,
                                   double minGain, AutoExposureParams params) : QThread(),
    pFramePipeline(pipeline), isMono(isMonoFlag)
{
    autoExposureHandler = new AutoExposureHandler(maxExposure, minExposure, maxGain, minGain, params);
}

ProcessingThread::~ProcessingThread() {
    delete autoExposureHandler;
}

void ProcessingThread::stopProcessingThread() {
    stoppedMutex.lock();
    stopped=true;
    stoppedMutex.unlock();
}

void ProcessingThread::run() {
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

        updateMembersMutex.lock();
        int type = ImageProcess::getOpenCvType((BitMode)frame->bpp, frame->channels);
        cvFrame = cv::Mat(frame->h, frame->w, type, frame->data);

        ////////////////////////////////////
        // Обработка изображения //
        ////////////////////////////////////
        if(!isMono && debayerOn)
            ImageProcess::debayerImg(cvFrame, cvFrame);

        if(!isMono && whiteBalanceOn)
            ImageProcess::whiteBalanceImg(cvFrame, cvFrame);

        if(!isMono && contrastOn)
            ImageProcess::contrastImg(cvFrame, cvFrame, contrastParam);

        if(!isMono && gammaContrastOn)
            ImageProcess::gammaContrastImg(cvFrame, cvFrame, gammaContrastParam);

        if(autoExposureOn) {
            if(!isMono)
                cv::cvtColor(cvFrame, cvFrameGrayscale,  cv::COLOR_BGR2GRAY);
            else
                cvFrameGrayscale = cvFrame.clone();

            if(autoExposureHandler->correct(cvFrameGrayscale, currentExposure, currentGain)) {
                currentGain = autoExposureHandler->getGain();
                currentExposure = autoExposureHandler->getExposure();
                emit newEGValues(currentGain, currentExposure);
            }
            else
                emit error("Неверный формат изображения!\nТребуется: 8 бит");
        }
        ////////////////////////////////////
        // Конец //
        ////////////////////////////////////

        auto qFrame = MatToQImage(matFrame);
        updateMembersMutex.unlock();

        emit newFrame(qFrame);
        frame = pFramePipeline->nextFrame(frame);
    }
}

void ProcessingThread::updateImageProcessingFlags(ImageProcessingFlags imageProcessingFlags) {
    QMutexLocker locker(&updateMembersMutex);
    this->debayerOn = imageProcessingFlags.debayerOn;
    this->whiteBalanceOn = imageProcessingFlags.whiteBalanceOn;
    this->contrastOn = imageProcessingFlags.contrastOn;
    this->gammaContrastOn = imageProcessingFlags.gammaContrastOn;
    this->autoExposureOn = imageProcessingFlags.autoExposureOn;

    this->contrastParam = imageProcessingFlags.contrastParam;
    this->gammaContrastParam = imageProcessingFlags.gammaContrastParam;
}

void ProcessingThread::onAutoExposureEnabled(double gain, double exposure) {
    QMutexLocker locker(&updateMembersMutex);
    this->currentGain = gain;
    this->currentExposure = exposure;
}
