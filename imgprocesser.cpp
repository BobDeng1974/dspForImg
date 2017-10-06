#include "imgprocesser.h"
#include <QDebug>


using namespace std;


imgProcesser::imgProcesser()
{
    for (int o=0; o<num_ocrs; o++)
    {
        ocrs.push_back(OCRTesseract::create());
    }
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
    vector<cv::Rect> nm_boxes;
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

//    for (int i=0; i<(int)nm_boxes.size(); i++){
//        cv::Scalar color = cv::Scalar(rand() % (155) + 100, rand() % 155 + 100, rand() % 155 + 100);
//        cv::rectangle(out_img, nm_boxes[i].tl(), nm_boxes[i].br(), color,2);
//    }


//    qDebug()<<nm_boxes.size()<<endl;

//    t_all = ((double)cv::getTickCount() - t_all)*1000/cv::getTickFrequency();
//    qDebug()<<"grouping time: "<<std::round(t_all)<<" ms"<<endl;


    /*Text Recognition (OCR)*/
    int bottom_bar_height= out_img.rows/7 ;
    copyMakeBorder(dst, out_img, 0, bottom_bar_height, 0, 0, BORDER_CONSTANT, Scalar(150, 150, 150));
    float scale_font = (float)(bottom_bar_height /85.0);
    vector<string> words_detection;
    float min_confidence1 = 0.f, min_confidence2 = 0.f;
    min_confidence1 = 51.f;
    min_confidence2 = 60.f;

    vector<Mat> detections;
    for (int i=0; i<(int)nm_boxes.size(); i++)
    {
        rectangle(out_img, nm_boxes[i].tl(), nm_boxes[i].br(), Scalar(255,255,0),3);
        Mat group_img = Mat::zeros(dst.rows+2, dst.cols+2, CV_8UC1);
        maskFiltered(nm_boxes[i]).copyTo(group_img);//webcam_demo.cpp wrong here
        //copyMakeBorder(group_img,group_img,15,15,15,15,BORDER_CONSTANT,Scalar(0));
        float imgScale = 24.0/group_img.rows;
        Size dsize = Size(int(group_img.cols*imgScale),int(group_img.rows*imgScale));
        cv::resize(group_img,group_img,dsize);
        group_img = 255-group_img;
        copyMakeBorder(group_img,group_img,4,4,4,4,BORDER_CONSTANT,Scalar(255));
        detections.push_back(group_img);
    }
    vector<string> outputs((int)detections.size());
    vector< vector<Rect> > boxes((int)detections.size());
    vector< vector<string> > words((int)detections.size());
    vector< vector<float> > confidences((int)detections.size());
    // parallel process detections in batches of ocrs.size() (== num_ocrs)
    for (int i=0; i<(int)detections.size(); i=i+(int)num_ocrs){
        Range r;
        if (i+(int)num_ocrs <= (int)detections.size())
            r = Range(i,i+(int)num_ocrs);
        else
            r = Range(i,(int)detections.size());

        parallel_for_(r, Parallel_OCR<cv::text::OCRTesseract>(detections, outputs, boxes, words, confidences, ocrs));
    }
    for (int i=0; i<(int)detections.size(); i++)
    {
        imshow("detection",detections[i]);
        qDebug()<<QString::fromStdString(outputs[i])<<endl;
        i=i;
//        outputs[i].erase(remove(outputs[i].begin(), outputs[i].end(), '\n'), outputs[i].end());
//        if (outputs[i].size() < 3)
//            continue;

//        for (int j=0; j<(int)boxes[i].size(); j++)
//        {
//            boxes[i][j].x += nm_boxes[i].x-15;
//            boxes[i][j].y += nm_boxes[i].y-15;

//            //cout << "  word = " << words[j] << "\t confidence = " << confidences[j] << endl;
//            if ((words[i][j].size() < 2) || (confidences[i][j] < min_confidence1) ||
//                    ((words[i][j].size()==2) && (words[i][j][0] == words[i][j][1])) ||
//                    ((words[i][j].size()< 4) && (confidences[i][j] < min_confidence2)) ||
//                    isRepetitive(words[i][j]))
//                continue;
//            words_detection.push_back(words[i][j]);
//            rectangle(out_img, boxes[i][j].tl(), boxes[i][j].br(), Scalar(255,0,255),3);
//            Size word_size = getTextSize(words[i][j], FONT_HERSHEY_SIMPLEX, (double)scale_font, (int)(3*scale_font), NULL);
//            rectangle(out_img, boxes[i][j].tl()-Point(3,word_size.height+3), boxes[i][j].tl()+Point(word_size.width,0), Scalar(255,0,255),-1);
//            putText(out_img, words[i][j], boxes[i][j].tl()-Point(1,1), FONT_HERSHEY_SIMPLEX, scale_font, Scalar(255,255,255),(int)(3*scale_font));
//        }
    }
//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    qDebug()<<"ocr time: "<<t_all<<endl;

//    int text_thickness = 1+(out_img.rows/500);
//    string fps_info = format("%2.1f Fps. %dx%d", (float)(1000 / t_all), dst.cols, dst.rows);
//    putText(out_img, fps_info, Point( 10,out_img.rows-5 ), FONT_HERSHEY_DUPLEX, scale_font, Scalar(255,0,0), text_thickness);
//    putText(out_img, region_types_str[REGION_TYPE], Point((int)(out_img.cols*0.5), out_img.rows - (int)(bottom_bar_height / 1.5)), FONT_HERSHEY_DUPLEX, scale_font, Scalar(255,0,0), text_thickness);
//    putText(out_img, grouping_algorithms_str[GROUPING_ALGORITHM], Point((int)(out_img.cols*0.5),out_img.rows-((int)(bottom_bar_height /3)+4) ), FONT_HERSHEY_DUPLEX, scale_font, Scalar(255,0,0), text_thickness);
//    putText(out_img, recognitions_str[RECOGNITION], Point((int)(out_img.cols*0.5),out_img.rows-5 ), FONT_HERSHEY_DUPLEX, scale_font, Scalar(255,0,0), text_thickness);


    return out_img;
}
