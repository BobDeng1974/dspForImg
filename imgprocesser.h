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

    vector< Ptr<OCRTesseract> > ocrs;
    const int num_ocrs = 5;
    const String region_types_str[2] = {"ERStats", "MSER"};
    const String grouping_algorithms_str[2] = {"exhaustive_search", "multioriented"};
    const String recognitions_str[2] = {"Tesseract", "NM_chain_features + KNN"};
    const String prefix = "E:\\ChormeDownloads\\";
    const string voc = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int  REGION_TYPE = 1;
    const int  GROUPING_ALGORITHM = 0;
    const int  RECOGNITION = 0;
};

//OCR recognition is done in parallel for different detections
template <class T>
class Parallel_OCR: public cv::ParallelLoopBody
{
private:
    vector<Mat> &detections;
    vector<string> &outputs;
    vector< vector<Rect> > &boxes;
    vector< vector<string> > &words;
    vector< vector<float> > &confidences;
    vector< Ptr<T> > &ocrs;

public:
    Parallel_OCR(vector<Mat> &_detections, vector<string> &_outputs, vector< vector<Rect> > &_boxes,
                 vector< vector<string> > &_words, vector< vector<float> > &_confidences,
                 vector< Ptr<T> > &_ocrs)
        : detections(_detections), outputs(_outputs), boxes(_boxes), words(_words),
          confidences(_confidences), ocrs(_ocrs)
    {}

    virtual void operator()( const cv::Range &r ) const
    {
        for (int c=r.start; c < r.end; c++)
        {
            ocrs[c%ocrs.size()]->run(detections[c], outputs[c], &boxes[c], &words[c], &confidences[c], OCR_LEVEL_WORD);
        }
    }
    Parallel_OCR & operator=(const Parallel_OCR &a);
};

#endif // IMGPROCESSER_H
