#include "imgprocesser.h"
#include <QDebug>

using namespace std;
using namespace cv;

imgProcesser::imgProcesser()
{

}

int countColors(Mat &img){
    vector<Vec3b> set;
    for(int i=0; i<img.rows; i++)
        for(int j=0; j<img.cols; j++){
            Vec3b color = img.at<Vec3b>(i,j);

            bool inSet=false;
            for(int setIter=0;setIter<set.size();setIter++){
                if(color==set[setIter]){
                    inSet = true;
                    break;
                }
            }
            if(!inSet){
                set.push_back(color);
            }
        }
    return set.size();
}

cv::Mat imgProcesser::process(cv::Mat &src)
{
    double t_all = (double)getTickCount();

    cv::Mat dst;

    dst = src.clone();

    int levels = log2(dst.cols/16);
    vector<cv::Mat> images(levels+1);
    images[levels] = dst.clone();
    for(int i=levels-1;i>=0;i--){
        pyrDown(images[i+1],images[i]);
    }

    dst = images[0];

    resize(dst,dst,images[levels].size(), 0, 0, INTER_NEAREST);

//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    qDebug()<<"detection time: "<<t_all<<endl;
//    t_all = (double)getTickCount();

    qDebug()<<"------------------"<<endl;
    return dst;
}
