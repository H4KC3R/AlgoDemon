#include "imagepipeline.h"

ImagePipeline::ImagePipeline(uint32_t length) {
    firstFrame.data = new uint8_t[length];
    secondFrame.data = new uint8_t[length];

    mList = {firstFrame, secondFrame};
}

ImagePipeline::~ImagePipeline() {
    mList.clear();
    delete firstFrame.data;
    firstFrame.data = nullptr;

    delete secondFrame.data;
    secondFrame.data = nullptr;
}

int ImagePipeline::getPipelineSize() {
    return mList.size();
}

const std::list <CamImage>::iterator ImagePipeline::getFirstFrame() {
    std::list <CamImage>::iterator it = mList.begin();
    if(it != mList.end()) {
        std::shared_lock lock(mMutex);
        it = mList.begin();
    }
    return it;
}

const std::list <CamImage>::iterator ImagePipeline::nextFrame(const std::list <CamImage>::iterator& it) {
    std::list <CamImage>::iterator next;
    if(count == 1) {
        next = std::next(it, 1);
        count++;
    }
    else {
        next = mList.begin();
        count = 1;
    }
    return next;
}

int32_t ImagePipeline::getCount() const {
    return count;
}

