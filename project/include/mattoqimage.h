#ifndef MATTOQIMAGE_H
#define MATTOQIMAGE_H

// Qt header files
#include <QtGui>
#include <QDebug>
// OpenCV header files
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

QImage MatToQImage(const cv::Mat&, int type);

#endif // MATTOQIMAGE_H
