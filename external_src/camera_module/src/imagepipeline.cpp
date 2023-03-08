#include "imagepipeline.h"

ImagePipeline::ImagePipeline() {
}

ImagePipeline::~ImagePipeline() {
    mList.clear();
}

void ImagePipeline::clearPipeline() {
    mList.clear();
}

const int ImagePipeline::getPipelineSize() {
    return mList.size();
}

int ImagePipeline::getCount() {
    return count;
}

void ImagePipeline::setFrame(CamImage *frame) {
    if(mList.size() == 2) {
        if(count == mSize){
            // Доступ ко второму элементу
            std::shared_lock lock(mMutex);
            auto it = std::next(mList.cbegin(), 1);
            it = mList.erase(it);
            mList.insert(it,*frame);
            count = 1;
        }
        else {
            std::shared_lock lock(mMutex);
            auto it = mList.cbegin();
            it = mList.erase(it);
            mList.insert(it,*frame);
            count++;
        }
    }
    else {
        std::shared_lock lock(mMutex);
        mList.push_back(*frame);
    }
}

const std::list <CamImage>::const_iterator ImagePipeline::getFirstFrame() {
    std::list <CamImage>::const_iterator it = mList.cbegin();
    if(it == mList.cend()) {
        std::shared_lock lock(mMutex);
        it = mList.cbegin();
    }
    return it;
}

const std::list <CamImage>::const_iterator ImagePipeline::nextFrame(const std::list <CamImage>::const_iterator& it) {
    std::list <CamImage>::const_iterator next = std::next(it, 1);
    if (next == mList.cend()) {
        std::shared_lock lock(mMutex);
        next = mList.cbegin();
    }
    return next;
}

