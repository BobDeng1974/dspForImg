#include "imgprocesser.h"
#include <QDebug>

using namespace std;
using namespace cv;
using namespace cv::text;

imgProcesser::imgProcesser()
{

}


cv::Mat imgProcesser::process(const cv::Mat &src)
{
    double t_all = (double)getTickCount();
    cv::Mat dst;
    dst = src.clone();
//  pyrDown( dst, dst);
//  pyrDown( dst, dst);


    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
    qDebug()<<"detection time: "<<t_all<<endl;
    t_all = (double)getTickCount();


    return out_img;
}
