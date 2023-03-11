#include <QCoreApplication>
#include "camframe.h"
#include "imagepipeline.h"

int main(int argc, char *argv[])
{
    uint32_t length = 5;
    CamFrame frame(length);
    qDebug() << frame.pData;
    frame.pData[0] = 1;
    frame.pData[1] = 3;
    frame.pData[3] = 5;

    ImagePipeline imgPipeline(5);
    for(int i = 0; i < imgPipeline.getPipelineSize(); i++){
        imgPipeline.setFrame(frame);
    }
    return 0;
}
