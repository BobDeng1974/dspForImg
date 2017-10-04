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
//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    qDebug()<<"preprocess time: "<<t_all<<endl;
//    t_all = (double)getTickCount();
    cv::Mat mask = cv::Mat::zeros(gray.size(), gray.type());
//    cv::Ptr<cv::MSER> mserExtractor = cv::MSER::create(21,
//                         (int)(0.00002*gray.cols*gray.rows), (int)(0.05*gray.cols*gray.rows), 1, 0.7);
    cv::Ptr<cv::MSER> mserExtractor = cv::MSER::create();
    vector<vector<cv::Point>> mserContours, contoursFilted;
    vector<cv::Rect> mserBbox, boxsFiltered;
    mserExtractor->detectRegions(gray, mserContours,  mserBbox);
    for(int i=0;i<mserContours.size();i++){
        vector<cv::Point> contour = mserContours[i];
        vector<cv::Point> conTest;
        cv::Rect box = mserBbox[i];

        cv::Mat maskT = cv::Mat::zeros(gray.size(), gray.type());
                for (cv::Point p : contour){
                    maskT.at<uchar>(p.y, p.x) = 255;
                }
        cv::Canny(maskT, maskT, 100, 100, 3);

        for (cv::Point p : contour){
            if(maskT.at<uchar>(p.y, p.x) >0){
                conTest.push_back(p);
            }
        }

        maskT = maskT/255;
        int edgeNum = cv::sum(maskT)[0];
//        qDebug()<< edgeNum*gray.cols/contour.size()<<endl;
        bool flag = contour.size()*100/fontSize<edgeNum*gray.cols//similar to swt(Stroke Width Transform)
                    && box.height>box.width
                    && box.height*20/scale>gray.rows
                    && box.width*50/scale>gray.cols;
        if(flag){
                    boxsFiltered.push_back(box);
                    contoursFilted.push_back(conTest);
//                    for (cv::Point p : contour){
//                        mask.at<uchar>(p.y, p.x) = 255;
//                    }
        }
    }

//    if(boxsFiltered.size()>0){
//        for (int i=0; i<(int)boxsFiltered.size(); i++)
//        {
//            cv::Scalar color = cv::Scalar(rand() % (155) + 100, rand() % 155 + 100, rand() % 155 + 100);
//            rectangle(out, boxsFiltered[i].tl(), boxsFiltered[i].br(), color,2);
//        }
//    }

    double  t_all = (double)cv::getTickCount();
    vector< vector<cv::Vec2i> > nm_region_groups;
    vector<cv::Rect> nm_boxes;
    vector<vector<cv::text::ERStat> > regions; //two channels
    regions.clear();
    vector<cv::Mat> channels;
    channels.clear();
    channels.push_back(gray);
    channels.push_back(255-gray);
    if (contoursFilted.size() > 0){
        cv::text::MSERsToERStats(gray, contoursFilted, regions);
        cv::text::erGrouping(dst, channels, regions, nm_region_groups, nm_boxes, cv::text::ERGROUPING_ORIENTATION_HORIZ);
        if(nm_boxes.size()>0){
            for (int i=0; i<(int)nm_boxes.size(); i++)
            {
                cv::rectangle(out, nm_boxes[i].tl(), nm_boxes[i].br(), cv::Scalar(255,255,0),2);
            }
        }
        t_all = ((double)cv::getTickCount() - t_all)*1000/cv::getTickFrequency();
        qDebug()<<"grouping time: "<<std::round(t_all)<<" ms"<<endl;
    }


    return out;
}
