#ifndef IMGPROCESSER_H
#define IMGPROCESSER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QVariantMap>
#include "opencv2/core/utility.hpp"
#include "opencv2/features2d.hpp"
using namespace cv;
using namespace std;

class imgProcesser
{
public:
    imgProcesser();
    cv::Mat process(cv::Mat &src);
    QVariantMap settings;


};

#endif // IMGPROCESSER_H
