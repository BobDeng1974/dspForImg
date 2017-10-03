#ifndef IMGPROCESSER_H
#define IMGPROCESSER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QVariantMap>
#include "opencv2/text.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/features2d.hpp"
using namespace cv;
using namespace cv::text;
using namespace std;

class imgProcesser
{
public:
    imgProcesser();
    cv::Mat process(const cv::Mat &src);
    QVariantMap settings;

    vector< Ptr<ERFilter> > er_filters1;
    vector< Ptr<ERFilter> > er_filters2;
    vector<vector<ERStat> > regions; //two channels

    vector< Ptr<OCRTesseract> > ocrs;
    vector< Ptr<OCRHMMDecoder> > decoders;

    const int num_ocrs = 5;

    const String region_types_str[2] = {"ERStats", "MSER"};
    const String grouping_algorithms_str[2] = {"exhaustive_search", "multioriented"};
    const String recognitions_str[2] = {"Tesseract", "NM_chain_features + KNN"};
    const String prefix = "E:\\ChormeDownloads\\";
    //const string voc = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const string voc = "0123456789";
    const int  REGION_TYPE = 1;
    const int  GROUPING_ALGORITHM = 0;
    const int  RECOGNITION = 1;

};

#endif // IMGPROCESSER_H
