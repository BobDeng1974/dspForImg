#include "imgprocesser.h"
#include <QDebug>
#include "SWT/swt.h"
using namespace std;
using namespace swt;

imgProcesser::imgProcesser()
{

}

cv::Mat imgProcesser::process(const cv::Mat &src)
{
    cv::Mat dst = src.clone();
    cv::Mat frame = src.clone();
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
    vector<vector<cv::Point>> mserContours;
    vector<cv::Rect> mserBbox;
    mserExtractor->detectRegions(gray, mserContours,  mserBbox);
    vector<int> remaining;
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
//        qDebug()<< edgeNum*gray.cols/contour.size()<<endl;
        bool flag = contour.size()*100<edgeNum*gray.cols//similar to swt(Stroke Width Transform)
                    && box.height>box.width
                    && box.height*20*scale>gray.rows
                    && box.width*50*scale>gray.cols;
        if(flag){
            remaining.push_back(i);

                    for (cv::Point p : contour){
                        mask.at<uchar>(p.y, p.x) = 255;
                    }

        }

    }
    qDebug()<<remaining.size()<<"/"<<mserBbox.size()<<endl;
//    for (vector<cv::Point> v : mserContours){
//        for (cv::Point p : v){
//            mask.at<uchar>(p.y, p.x) = 255;
//        }
//    }

//    for( int i = 0; i<contours.size(); i++ )
//    {
//        drawContours(mask, contours, i, Scalar(255, 255, 255), 4);
//    }

//    long int start = ms();
//    cv::Mat edges, gaussian;
//    cv::Mat grad_x, grad_y, noUse;
//    cv::Mat componentsMat = frame.clone();
//    cv::Mat validComponentsMat = frame.clone();
//    cv::Mat finalClusterMat = frame.clone();

////    cv::cvtColor(frame, gray, CV_RGB2GRAY);
////    gray = 255-gray;
//    cv::Canny(gray, edges, 100, 300, 3);
////    edges.copyTo(edges,mask);
//    cv::GaussianBlur(gray, gaussian, cv::Size(5, 5), 0, 0);

//    cv::Scharr(gaussian, grad_x, CV_32F, 1, 0);
//    cv::Scharr(gaussian, grad_y, CV_32F, 0, 1);

//    qDebug()<< "Initial image processing (gaussian blur etc.): " << ms() - start << " ms" << "\n";
//    start = ms();

//    std::vector<Point> rays = swt::swt(edges, grad_x, grad_y);
//    qDebug() << "SWT: " << ms() - start << " ms" << "\n";
//    start = ms();

//    cv::Mat swt_mat = cv::Mat::zeros(edges.size().height, edges.size().width, CV_32F);
//    std::vector<Point> uniqueRays;
//    for(int i = 0; i < rays.size(); i++){
//        Point tmp = rays[i];
//        //Remove duplicate values. the swt is initiated as an empty mat, all zeros, so if the point has a value
//        //then we have already visited that point in the rays array, so we delete that point.
//        if(swt_mat.at<float>(tmp.p) == 0.0){
//            uniqueRays.push_back(tmp);
//        }
//        swt_mat.at<float>(tmp.p) = tmp.length;
//    }
//    qDebug() << "After SWT: " << ms() - start << " ms" << "\n";
//    start = ms();

//    std::vector<std::vector<cv::Point> > components = cca(swt_mat, uniqueRays);
//    qDebug() << "CCA: " << ms() - start << " ms" << "\n";
//    start = ms();

//    std::vector<Component> validComponents = filterComponents(swt_mat, components);
//    qDebug() << "Filter: " << ms() - start << " ms" << "\n";
//    start = ms();

//    std::vector<std::vector<Component> > clusters = chain(swt_mat, validComponents, frame);
//    qDebug() << "Chain: " << ms() - start << " ms" << "\n";
//    start = ms();

//    for(int i = 0; i < components.size(); i++){
//        cv::Rect rect = cv::boundingRect(components[i]);
//        cv::rectangle(componentsMat, rect, cv::Scalar(0, 255, 255), 2);
//        if(i < validComponents.size()){
//            cv::rectangle(validComponentsMat, validComponents[i].rect, cv::Scalar(0, 0, 255), 2);
//        }
//    }
//    for(int i = 0; i < clusters.size(); i++){
//        cv::Scalar color = cv::Scalar(rand() % (155) + 100, rand() % 155 + 100, rand() % 155 + 100);
//        for(int q = 0; q < clusters[i].size(); q++){
//            cv::rectangle(finalClusterMat, clusters[i][q].rect, color, 2);
//        }
//    }

    return mask;
}
