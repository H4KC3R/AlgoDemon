#ifndef STRUCTURES_H
#define STRUCTURES_H

struct ImageProcessingFlags{
    bool debayerOn;
    bool whiteBalanceOn;
    bool autoExposureOn;

    bool contrastOn;
    double contrastParam;

    bool gammaContrastOn;
    double gammaContrastParam;
};

struct RoiBox{
    uint32_t startX;
    uint32_t startY;
    uint32_t sizeX;
    uint32_t sizeY;
};


#endif // STRUCTURES_H
