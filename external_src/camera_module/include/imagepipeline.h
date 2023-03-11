#ifndef IMAGEPIPELINE_H
#define IMAGEPIPELINE_H

#include "camframe.h"
#include <list>
#include <iterator>
#include <mutex>
#include <shared_mutex>

class ImagePipeline
{
public:
    ImagePipeline(size_t size = 5);

    ~ImagePipeline();

    void setFrame(CamFrame& frame);

    const std::list <CamFrame>::iterator getFirstFrame();

    const std::list <CamFrame>::iterator nextFrame(const std::list <CamFrame>::iterator& it);

    int getPipelineSize();

    void clearBuffer();

    uint32_t getFrameCount() const;

private:
    size_t mSize;
    uint32_t frameCount;
    std::list<CamFrame> mList;
    std::shared_mutex mMutex;

};

#endif // IMAGEPIPELINE_H
