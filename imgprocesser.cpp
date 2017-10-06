#include "imgprocesser.h"
#include <QDebug>


using namespace std;


imgProcesser::imgProcesser()
{

}

bool isRepetitive(const string& s)
{
    int count = 0;
    for (int i=0; i<(int)s.size(); i++)
    {
        if ((s[i] == 'i') ||
                (s[i] == 'l') ||
                (s[i] == 'I'))
            count++;
    }
    if (count > ((int)s.size()+1)/2)
    {
        return true;
    }
    return false;
}

cv::Mat imgProcesser::process(const cv::Mat &src)
{
    cv::Mat dst = src.clone();
    cv::Mat out_img = src.clone();

//    pyrDown( dst, dst);
//  pyrDown( dst, dst);



    cv::Mat gray;
    cv::cvtColor(dst,gray,cv::COLOR_BGR2GRAY);
    int cols = gray.cols;
    int rows = gray.rows;
//    qDebug()<<"rows: "<<rows<<"\tcols: "<<cols<<endl;

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
                    && box.height<box.width*10
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

    cv::Mat maskFiltered=cv::Mat::zeros(gray.size(), gray.type());
    for(int i=0;i<contoursFilted.size();i++){
        for (cv::Point p : contoursFilted[i]){
            maskFiltered.at<uchar>(p.y, p.x) = 255;
        }
    }

    dilate(maskFiltered,maskFiltered,Mat());
    dilate(maskFiltered,maskFiltered,Mat());

//    imshow("filter1",maskFiltered);
//    Mat edge;
//    Canny(gray, edge, 100, 200, 3);
//    edge = edge > 0;
//    maskFiltered = maskFiltered.mul(edge);

    dilate(maskFiltered,maskFiltered,Mat());
    dilate(maskFiltered,maskFiltered,Mat());
    erode(maskFiltered,maskFiltered,Mat());
    erode(maskFiltered,maskFiltered,Mat());


    imshow("filter2",maskFiltered);



    double  t_all = (double)cv::getTickCount();

//    double expand=0.2;
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

//    cv::Mat mask = cv::Mat::zeros(gray.size(), gray.type());
    vector<cv::Rect> nm_boxes,nm_boxes2;
//    for (int i = 0; i < boxsFiltered.size(); i++){
//        cv::rectangle(mask, boxsFiltered[i].tl(), boxsFiltered[i].br(), cv::Scalar(255), CV_FILLED); // Draw filled bounding boxes on mask
//    }

    std::vector<std::vector<cv::Point>> contours;
    // Find contours in mask
    // If bounding boxes overlap, they will be joined by this function call
    cv::findContours(maskFiltered, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (int j = 0; j < contours.size(); j++){
        nm_boxes.push_back(cv::boundingRect(contours.at(j)));
    }

    //find >=3 continue box
    int boxN = nm_boxes.size();
    if(boxN>0){
        QSet<int> possible, finalPossible;
        cv::Mat pointsY=cv::Mat::zeros(cv::Size(1,boxN),CV_32FC1); //Nx1
        for (int i=0; i<boxN; i++){
            pointsY.at<float>(i,0) = nm_boxes[i].tl().y;
        }
        cv::Mat ones=cv::Mat::zeros(cv::Size(boxN,1),CV_32FC1);    //Nx1
        cv::Mat eyes=cv::Mat::eye(cv::Size(boxN,boxN),CV_32FC1)*10000;
        Mat subY = pointsY*ones - (pointsY*ones).t()+eyes;
        for(int i=0;i<subY.rows;i++){
            for(int j=0;j<subY.cols;j++){
                if(abs(subY.at<float>(j,i))<nm_boxes[i].height*0.4 &&
                        abs(nm_boxes[i].height-nm_boxes[j].height)<nm_boxes[i].height*0.4){
                    possible.insert(i);
                    possible.insert(j);
                }
            }
        }


        for (QSet<int>::iterator i = possible.begin(); i != possible.end(); ++i){
            int x1 = nm_boxes[*i].tl().x;
            int adjCounts=0;
            int iterCache=0;
            int disCache=0;
            for (QSet<int>::iterator j = possible.begin(); j != possible.end(); ++j){
                int x2 = nm_boxes[*j].tl().x;
                if(abs(x2-x1)<nm_boxes[*i].height*1.5 &&
                        (disCache==0 || abs(abs(x2-x1)-disCache)<nm_boxes[*i].height*0.2)){
                    adjCounts++;
                    if(adjCounts>1){
                        finalPossible.insert(iterCache);
                        finalPossible.insert(*j);
                    }else{
                        iterCache=*j;
                        disCache = abs(x2-x1);
                    }
                }
            }
        }
        for (QSet<int>::iterator i = finalPossible.begin(); i != finalPossible.end(); ++i){
            nm_boxes2.push_back(nm_boxes[*i]);
        }
        nm_boxes.clear();
        nm_boxes = nm_boxes2;
    }

//    for (int i=0; i<(int)nm_boxes.size(); i++){
//        cv::Scalar color = cv::Scalar(rand() % (155) + 100, rand() % 155 + 100, rand() % 155 + 100);
//        cv::rectangle(out_img, nm_boxes[i].tl(), nm_boxes[i].br(), color,2);
//    }


//    qDebug()<<nm_boxes.size()<<endl;

//    t_all = ((double)cv::getTickCount() - t_all)*1000/cv::getTickFrequency();
//    qDebug()<<"grouping time: "<<std::round(t_all)<<" ms"<<endl;


    /*Text Recognition (OCR)*/
    int bottom_bar_height= out_img.rows/7 ;
//    copyMakeBorder(dst, out_img, 0, bottom_bar_height, 0, 0, BORDER_CONSTANT, Scalar(150, 150, 150));
    float scale_font = (float)(bottom_bar_height /85.0);
    float min_confidence1 = 0.f, min_confidence2 = 0.f;
    min_confidence1 = 51.f;
    min_confidence2 = 60.f;

    vector<Mat> detections;
    for (int i=0; i<(int)nm_boxes.size(); i++)
    {
        rectangle(out_img, nm_boxes[i].tl(), nm_boxes[i].br(), Scalar(255,255,0),3);
        Mat group_img = Mat::zeros(dst.rows+2, dst.cols+2, CV_8UC1);
        gray(nm_boxes[i]).copyTo(group_img);//webcam_demo.cpp wrong here
        //copyMakeBorder(group_img,group_img,15,15,15,15,BORDER_CONSTANT,Scalar(0));
        float imgScale = 20.0/group_img.rows;
        Size dsize = Size(int(group_img.cols*imgScale),int(group_img.rows*imgScale));
        cv::resize(group_img,group_img,dsize);
        group_img = 255-group_img;
        int diff1 = (group_img.rows-group_img.cols)/2;
        int diff2 = (group_img.rows-group_img.cols)/2+(group_img.rows-group_img.cols)%2;
        copyMakeBorder(group_img,group_img,4,4,
                       diff1+4,diff2+4,
                       BORDER_CONSTANT,Scalar(255));
        detections.push_back(group_img);
    }

    for (int i=0; i<(int)detections.size(); i++)
    {
        vector<int> out_classes;
        vector<double> out_confidences;

        ocr->eval(detections[i], out_classes, out_confidences);

        QString out = /*QString::number(int(out_confidences[0]*100))+"%"+"_"+*/
                      vocabulary[out_classes[0]]+" ";
//        qDebug() << "OCR output = \"" << out << "\" with confidence "
//             << out_confidences[0]<<endl;

        Size word_size = getTextSize(out.toStdString(), FONT_HERSHEY_SIMPLEX, (double)scale_font, (int)(3*scale_font), NULL);
        rectangle(out_img, nm_boxes[i].tl()-Point(3,word_size.height+3), nm_boxes[i].tl()+Point(word_size.width,0), Scalar(255,0,255),-1);
        putText(out_img, out.toStdString(), nm_boxes[i].tl()-Point(1,1), FONT_HERSHEY_SIMPLEX, scale_font, Scalar(255,255,255),(int)(3*scale_font));
    }
    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
    qDebug()<<"ocr time: "<<t_all<<endl;


    imshow("out", out_img);
    return out_img;
}
