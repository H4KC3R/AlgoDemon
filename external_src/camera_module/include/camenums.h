#ifndef CAMENUMS_H
#define CAMENUMS_H

enum StreamMode {
    single = 0,
    live
};

enum CameraControls {
    gain                = 6,        //!< camera gain
    offset              = 7,        //!< camera offset
    exposure            = 8,        //!< expose time (us)

    speed               = 9,        //!< control the camera transfer speed.
                                    //!< if the value is‘0’means low speed transmission, if ‘1’means high speed

    transferbit         = 10,       //!< image depth bits

    fps          = 12,       //!< control the USB bandwidth,(only for QHY5II
                                    //!< and QHY5LII to reduce the USB bandwidth . Value range: (30-0), 0 - USB
                                    //!< transmission is the fastest , 30 - transmission is the slowest)
};

enum BitMode{
    bit8 = 8,
    bit16 = 16
};

enum CameraStatus {
    idle = 0,
    failed,
    singleCapture,
    liveCapture
};

#endif // CAMENUMS_H
