#include "imgprocesser.h"
#include <QDebug>


using namespace std;


imgProcesser::imgProcesser()
{

}

cv::Mat imgProcesser::process(const cv::Mat &src)
{
    double  t_all = (double)cv::getTickCount();
    message.clear();

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

    cv::Ptr<cv::MSER> mserExtractor = cv::MSER::create(21,
                         (int)(0.00002*cols*rows), (int)(0.05*cols*rows), 1, 0.7);

//    cv::Ptr<cv::MSER> mserExtractor = cv::MSER::create();
    vector<vector<cv::Point>> mserContours, contoursFilted;
    vector<cv::Rect> mserBbox, boxsFiltered;
    mserExtractor->detectRegions(gray, mserContours,  mserBbox);

//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    message = message + "mser time: " + QString::number((int)t_all) + " ms   ";
//    t_all = (double)cv::getTickCount();

    cv::Mat maskS = cv::Mat::zeros(gray.size(), gray.type());
    for(int i=0;i<mserContours.size();i++){
        vector<cv::Point> contour = mserContours[i];
        cv::Rect box = mserBbox[i];
//        qDebug()<<"contour.size(): " << edgeNum*cols/contour.size()<<endl;

         bool flag =   box.height>box.width
                    && box.height<box.width*10
                    && box.height*20/scale>rows
                    && box.width*50/scale>cols;
        if(flag){
            cv::Mat mask = cv::Mat::zeros(gray.size(), gray.type());
            for (cv::Point p : contour){
                mask.at<uchar>(p.y, p.x) = 255;
                maskS.at<uchar>(p.y, p.x) = 1;
            }
            Mat maskInv = (255-mask)>0;
            dilate(mask,mask,Mat());
            mask = maskInv.mul(mask);
            int edgeNum = cv::sum(mask/255)[0];

            // stroke width < threshold
            if(contour.size()*100/fontSize<edgeNum*cols){
                Mat swt32f;
                distanceTransform(mask, swt32f, CV_DIST_L2, 5);
                Mat tmp_m, tmp_sd;
                meanStdDev(swt32f, tmp_m, tmp_sd, swt32f>0);
                double sd = tmp_sd.at<double>(0,0);
                double m = tmp_m.at<double>(0,0);
                // qDebug()<<" std/m: " + QString::number(sd/m)<<endl;
                if(sd/m<0.5){
                    // stroke width variation < threshold
                    //similar to swt(Stroke Width Transform))
                    // too heavy so put it in second stage
                    boxsFiltered.push_back(box);
                    contoursFilted.push_back(contour);
                }
            }
        }
    }

//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    message = message + "filter time: " + QString::number((int)t_all) + " ms   ";
//    t_all = (double)cv::getTickCount();



    vector<cv::Rect> nm_boxes, nm_boxes2;
    cv::Mat maskFiltered = cv::Mat::zeros(gray.size(), gray.type());
    for(int i=0;i<contoursFilted.size();i++){
        for (cv::Point p : contoursFilted[i]){
            maskFiltered.at<uchar>(p.y, p.x) = 255;
        }
    }
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(maskFiltered, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (int j = 0; j < contours.size(); j++){
        nm_boxes.push_back(cv::boundingRect(contours.at(j)));
    }




    //find >=3 continue box
    int boxN = nm_boxes.size();
//    qDebug()<<"boxN: "<<boxN<<endl;
    for (int i=0; i<(int)nm_boxes.size(); i++){
        cv::Scalar color = cv::Scalar(0, 0, 0);
        cv::rectangle(out_img, nm_boxes[i].tl(), nm_boxes[i].br(), color,2);
    }
    if(boxN>0){
        QSet<int> heightPossible;
        for(int i=0;i<boxN;i++){
            int yi = nm_boxes[i].tl().y;
            int xi = nm_boxes[i].tl().x;
            int thresh = nm_boxes[i].height;
            int sameCounts=0;
            int jCache;
            for(int j=0;j<boxN;j++){
                int yj = nm_boxes[j].tl().y;
                int xj = nm_boxes[j].tl().x;
                if(i!=j){
                    if(abs(nm_boxes[i].height-nm_boxes[j].height)*2<nm_boxes[i].height
                            && abs(yi-yj)*2<thresh && abs(xi-xj)<1.5*thresh){
                        if(sameCounts>0){
                            heightPossible.insert(i);
                            heightPossible.insert(j);
                            heightPossible.insert(jCache);
                        }else{
                            sameCounts++;
                            jCache = j;
                        }
                    }
                }
            }
        }
        for (QSet<int>::iterator i = heightPossible.begin(); i != heightPossible.end(); ++i){
            nm_boxes2.push_back(nm_boxes[*i]);
        }
//        nm_boxes.clear();
//        nm_boxes = nm_boxes2;
    }
    for (int i=0; i<(int)nm_boxes2.size(); i++){
        cv::Scalar color = cv::Scalar(rand() % (155) + 100, rand() % 155 + 100, rand() % 155 + 100);
        cv::rectangle(out_img, nm_boxes2[i].tl(), nm_boxes2[i].br(), color,2);
    }

//    qDebug()<<nm_boxes2.size()<<endl;

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
    for (int i=0; i<(int)nm_boxes2.size(); i++)
    {
    //    rectangle(out_img, nm_boxes2[i].tl(), nm_boxes2[i].br(), Scalar(255,255,0),3);
        Mat group_img = Mat::zeros(dst.rows+2, dst.cols+2, CV_8UC1);
        gray(nm_boxes2[i]).copyTo(group_img);//webcam_demo.cpp wrong here
        //copyMakeBorder(group_img,group_img,15,15,15,15,BORDER_CONSTANT,Scalar(0));

        Mat selectedChildren = Mat::zeros(dst.rows+2, dst.cols+2, CV_8UC1);
        maskS(nm_boxes2[i]).copyTo(selectedChildren);
        int selectedChildren_height = cv::sum(group_img.mul(selectedChildren))[0]/cv::sum(selectedChildren)[0];
        int others_height = cv::sum(group_img.mul(1-selectedChildren))[0]/cv::sum(1-selectedChildren)[0];
        //this is how mser work: select area is high or low?

        threshold(group_img, group_img, 150, 255, THRESH_BINARY|THRESH_OTSU);
        if(selectedChildren_height>others_height){
            group_img = 255-group_img; // convert to black-num-white-bg
        }

        float imgScale = 20.0/group_img.rows;
        Size dsize = Size(int(group_img.cols*imgScale),int(group_img.rows*imgScale));
        cv::resize(group_img,group_img,dsize);



        int diff1 = (group_img.rows-group_img.cols)/2;
        int diff2 = (group_img.rows-group_img.cols)/2+(group_img.rows-group_img.cols)%2;
        copyMakeBorder(group_img,group_img,4,4,
                       diff1+4,diff2+4,
                       BORDER_CONSTANT,Scalar(255));


        detections.push_back(group_img);
//        imshow("to ocr:",group_img);
    }

    if(settings["p1"].toInt()>0){
        if(detections.size()>0){
            imshow("char to ocr",detections[rand()%detections.size()]);
//            return detections[rand()%detections.size()];
        }

    }


    for (int i=0; i<(int)detections.size(); i++)
    {
        string outText;
        outText = ocrTess->run(detections[i], 65.0);
        outText.erase(std::remove(outText.begin(), outText.end(), '\n'), outText.end());
        QString out = QString::fromStdString(outText);
//        qDebug()<<out<<endl;
//        qDebug() << "OCR output "<< i <<" = \"" << out << "\" with confidence "
//             << out_confidences[0]<<endl;

        Size word_size = getTextSize(out.toStdString(), FONT_HERSHEY_SIMPLEX, (double)scale_font, (int)(3*scale_font), NULL);
        rectangle(out_img, nm_boxes2[i].tl()-Point(3,word_size.height+3), nm_boxes2[i].tl()+Point(word_size.width,0), Scalar(255,0,255),-1);
        putText(out_img, out.toStdString(), nm_boxes2[i].tl()-Point(1,1), FONT_HERSHEY_SIMPLEX, scale_font, Scalar(255,255,255),(int)(3*scale_font));
    }
    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    qDebug()<<"ocr time: "<<t_all<<endl;
    message = message + "ocr time: " + QString::number((int)t_all) + " ms   ";
//    imshow("out", out_img);
    return out_img;
}
