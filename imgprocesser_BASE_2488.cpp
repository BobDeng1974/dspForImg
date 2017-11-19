#include "imgprocesser.h"
#include <QDebug>

using namespace std;
using namespace cv;

imgProcesser::imgProcesser()
{

}


cv::Mat imgProcesser::process(cv::Mat &src)
{
    double t_all = (double)getTickCount();

    cv::Mat dst;
    dst = src.clone();

    int levels = log2(dst.cols/16);
    for(int i=0; i<levels ; i++){
            pyrDown(dst,dst);
    }


//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    qDebug()<<"detection time: "<<t_all<<endl;
//    t_all = (double)getTickCount();

    qDebug()<<"------------------"<<endl;
    return dst;
}
