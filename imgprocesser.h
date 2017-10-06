#ifndef IMGPROCESSER_H
#define IMGPROCESSER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QVariantMap>
#include "opencv2/text.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/features2d.hpp"

using namespace std;
using namespace cv;
using namespace cv::text;
class imgProcesser
{
public:
    imgProcesser();
    cv::Mat process(const cv::Mat &src);
    QVariantMap settings;

    const float scale=1;
    const float fontSize=1;

    const String prefix = "E:\\downloads\\";
    QString vocabulary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    // must have the same order as the classifier output classes
    Ptr<OCRHMMDecoder::ClassifierCallback> ocr = loadOCRHMMClassifierCNN(prefix+"OCRBeamSearch_CNN_model_data.xml.gz");
};

#endif // IMGPROCESSER_H
