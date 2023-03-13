#include "mattoqimage.h"

QImage MatToQImage(const cv::Mat& mat){
    if(mat.type() == CV_8UC1) {
        const uchar *qImageBuffer = (const uchar*)mat.data;
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return img;
    }
    else if(mat.type() == CV_16UC1){
        const uchar *qImageBuffer = (const uchar*)mat.data;
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale16);
        return img;
    }
    else if(mat.type() == CV_8UC3) {
        const uchar *qImageBuffer = (const uchar*)mat.data;
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_BGR888);
        return img;
    }
    else if(mat.type() == CV_16UC3) {
        const uchar *qImageBuffer = (const uchar*)mat.data;
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_BGR888);
        return img;
    }
    else {
        return QImage();
    }
}
