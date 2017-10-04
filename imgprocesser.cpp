#include "imgprocesser.h"
#include <QDebug>
using namespace std;

imgProcesser::imgProcesser()
{

}

cv::Mat imgProcesser::process(const cv::Mat &src)
{
    cv::Mat dst = src.clone();
    cv::Mat out = src.clone();
//    pyrDown( dst, dst);
//  pyrDown( dst, dst);
//  qDebug()<<"rows: "<<dst.rows<<"\tcols: "<<dst.cols<<endl;


    cv::Mat gray;
    cv::cvtColor(dst,gray,cv::COLOR_BGR2GRAY);
    int cols = gray.cols;
    int rows = gray.rows;
//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    qDebug()<<"preprocess time: "<<t_all<<endl;
//    t_all = (double)getTickCount();

//    cv::Ptr<cv::MSER> mserExtractor = cv::MSER::create(21,
//                         (int)(0.00002*cols*rows), (int)(0.05*cols*rows), 1, 0.7);
    cv::Ptr<cv::MSER> mserExtractor = cv::MSER::create();
    vector<vector<cv::Point>> mserContours, contoursFilted;
    vector<cv::Rect> mserBbox, boxsFiltered;
    mserExtractor->detectRegions(gray, mserContours,  mserBbox);
    for(int i=0;i<mserContours.size();i++){
        vector<cv::Point> contour = mserContours[i];
        cv::Rect box = mserBbox[i];

        cv::Mat maskT = cv::Mat::zeros(gray.size(), gray.type());
                for (cv::Point p : contour){
                    maskT.at<uchar>(p.y, p.x) = 255;
                }
        cv::Canny(maskT, maskT, 100, 100, 3);
        maskT = maskT/255;
        int edgeNum = cv::sum(maskT)[0];
//        qDebug()<< edgeNum*cols/contour.size()<<endl;
        bool flag = contour.size()*100/fontSize<edgeNum*cols//similar to swt(Stroke Width Transform)
                    && box.height>box.width
                    && box.height*20/scale>rows
                    && box.width*50/scale>cols;
        if(flag){
                    boxsFiltered.push_back(box);
                    contoursFilted.push_back(contour);
//                    for (cv::Point p : contour){
//                        mask.at<uchar>(p.y, p.x) = 255;
//                    }
        }
    }

    double  t_all = (double)cv::getTickCount();

//    float expand=0.3;
//    for(int i=0;i<boxsFiltered.size();i++){
//       cv::Rect box = boxsFiltered[i];
//       cv::Point pTopLeft = box.tl();
//       cv::Point pBottomRight = box.br();

//       pTopLeft.x -= round(expand*box.height);
//       if(pTopLeft.x<0) pTopLeft.x=0;
//       pBottomRight.x += round(expand*box.height);
//       if(pBottomRight.x>cols) pBottomRight.x = cols;
////       pTopLeft.y -= (int)(expand*box.height);
////       if(pTopLeft.y<0) pTopLeft.y=0;
////       pBottomRight.y += (int)(expand*box.height);
////       if(pBottomRight.y>rows) pBottomRight.y = rows;
//       cv::Rect rRect(pTopLeft, pBottomRight);
//       boxsFiltered.at(i) = rRect;
//    }

    cv::Mat mask = cv::Mat::zeros(gray.size(), gray.type());
    vector<cv::Rect> outputBoxes;
    for (int i = 0; i < boxsFiltered.size(); i++){
        cv::rectangle(mask, boxsFiltered[i].tl(), boxsFiltered[i].br(), cv::Scalar(255), CV_FILLED); // Draw filled bounding boxes on mask
    }
    std::vector<std::vector<cv::Point>> contours;
    // Find contours in mask
    // If bounding boxes overlap, they will be joined by this function call
    cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (int j = 0; j < contours.size(); j++){
        outputBoxes.push_back(cv::boundingRect(contours.at(j)));
    }

    for (int i=0; i<(int)outputBoxes.size(); i++){
        cv::Scalar color = cv::Scalar(rand() % (155) + 100, rand() % 155 + 100, rand() % 155 + 100);
        cv::rectangle(out, outputBoxes[i].tl(), outputBoxes[i].br(), color,2);
    }
//    qDebug()<<outputBoxes.size()<<endl;

//    t_all = ((double)cv::getTickCount() - t_all)*1000/cv::getTickFrequency();
//    qDebug()<<"grouping time: "<<std::round(t_all)<<" ms"<<endl;



    return out;
}
