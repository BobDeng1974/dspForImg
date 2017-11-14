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
<<<<<<< HEAD
=======
    dst = src.clone();

    int levels = log2(dst.cols/16);
    vector<cv::Mat> images(levels+1);
    images[levels] = dst.clone();
    for(int i=levels-1;i>=0;i--){
        pyrDown(images[i+1],images[i]);
    }
>>>>>>> 127a72f0ccb9ce8002e841655f6db2e5fbc4bb5a

    dst = images[2];


//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    qDebug()<<"detection time: "<<t_all<<endl;
//    t_all = (double)getTickCount();

    qDebug()<<"------------------"<<endl;
    return dst;
}
