#include "imgprocesser.h"
#include <QDebug>

using namespace std;
using namespace cv;
using namespace cv::text;

//ERStat extraction is done in parallel for different channels
class Parallel_extractCSER: public cv::ParallelLoopBody
{
private:
    vector<Mat> &channels;
    vector< vector<ERStat> > &regions;
    vector< Ptr<ERFilter> > er_filter1;
    vector< Ptr<ERFilter> > er_filter2;

public:
    Parallel_extractCSER(vector<Mat> &_channels, vector< vector<ERStat> > &_regions,
                         vector<Ptr<ERFilter> >_er_filter1, vector<Ptr<ERFilter> >_er_filter2)
        : channels(_channels),regions(_regions),er_filter1(_er_filter1),er_filter2(_er_filter2) {}

    virtual void operator()( const cv::Range &r ) const
    {
        for (int c=r.start; c < r.end; c++)
        {
            er_filter1[c]->run(channels[c], regions[c]);
            er_filter2[c]->run(channels[c], regions[c]);
        }
    }
    Parallel_extractCSER & operator=(const Parallel_extractCSER &a);
};

//OCR recognition is done in parallel for different detections
template <class T>
class Parallel_OCR: public cv::ParallelLoopBody
{
private:
    vector<Mat> &detections;
    vector<string> &outputs;
    vector< vector<Rect> > &boxes;
    vector< vector<string> > &words;
    vector< vector<float> > &confidences;
    vector< Ptr<T> > &ocrs;

public:
    Parallel_OCR(vector<Mat> &_detections, vector<string> &_outputs, vector< vector<Rect> > &_boxes,
                 vector< vector<string> > &_words, vector< vector<float> > &_confidences,
                 vector< Ptr<T> > &_ocrs)
        : detections(_detections), outputs(_outputs), boxes(_boxes), words(_words),
          confidences(_confidences), ocrs(_ocrs)
    {}

    virtual void operator()( const cv::Range &r ) const
    {
        for (int c=r.start; c < r.end; c++)
        {
            ocrs[c%ocrs.size()]->run(detections[c], outputs[c], &boxes[c], &words[c], &confidences[c], OCR_LEVEL_WORD);
        }
    }
    Parallel_OCR & operator=(const Parallel_OCR &a);
};

imgProcesser::imgProcesser()
{
    for (int i=0; i<2; i++)
    {
        Ptr<ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1(prefix + "trained_classifierNM1.xml"),8,0.00015f,0.13f,0.2f,true,0.1f);
        Ptr<ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2(prefix + "trained_classifierNM2.xml"),0.5);
        er_filters1.push_back(er_filter1);
        er_filters2.push_back(er_filter2);
    }
    regions.resize(2);

    for (int o=0; o<num_ocrs; o++)
    {
        ocrs.push_back(OCRTesseract::create());
    }

    Mat transition_p;
    string filename =prefix + "OCRHMM_transitions_table.xml";
    FileStorage fs(filename, FileStorage::READ);
    fs["transition_probabilities"] >> transition_p;
    fs.release();
    Mat emission_p = Mat::eye(62,62,CV_64FC1);

    for (int o=0; o<num_ocrs; o++)
    {
        decoders.push_back(OCRHMMDecoder::create(loadOCRHMMClassifierNM(prefix + "OCRHMM_knn_model_data.xml.gz"),
                           voc, transition_p, emission_p));
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

void er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions, vector<Vec2i> group, Mat& segmentation)
{
    for (int r=0; r<(int)group.size(); r++)
    {
        ERStat er = regions[group[r][0]][group[r][1]];
        if (er.parent != NULL) // deprecate the root region
        {
            int newMaskVal = 255;
            int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
            floodFill(channels[group[r][0]],segmentation,Point(er.pixel%channels[group[r][0]].cols,er.pixel/channels[group[r][0]].cols),
                      Scalar(255),0,Scalar(er.level),Scalar(0),flags);
        }
    }
}

cv::Mat imgProcesser::process(const cv::Mat &src)
{
    double t_all = (double)getTickCount();
    cv::Mat dst;
    dst = src.clone();
//    pyrDown( dst, dst);
//  pyrDown( dst, dst);
//  qDebug()<<"rows: "<<dst.rows<<"\tcols: "<<dst.cols<<endl;


    Mat gray;
    vector<Mat> channels;
    cvtColor(dst,gray,COLOR_BGR2GRAY);
    Mat out_img = gray.clone();
    channels.push_back(gray);
    channels.push_back(255-gray);

    regions[0].clear();
    regions[1].clear();

//    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//    qDebug()<<"preprocess time: "<<t_all<<endl;
    t_all = (double)getTickCount();

    switch (REGION_TYPE)
    {
    case 0: // ERStats
        parallel_for_(cv::Range(0, (int)channels.size()), Parallel_extractCSER(channels, regions, er_filters1, er_filters2));
        break;
    case 1: // MSER
        vector<vector<Point> > contours;
        vector<Rect> bboxes;
        Ptr<MSER> mser = MSER::create(21, (int)(0.00002*gray.cols*gray.rows), (int)(0.05*gray.cols*gray.rows), 1, 0.7);
        mser->detectRegions(gray, contours, bboxes);

        for (int i=0; i<(int)bboxes.size(); i++)
        {
            rectangle(out_img, bboxes[i].tl(), bboxes[i].br(), Scalar(255,255,0),2);
        }

        //Convert the output of MSER to suitable input for the grouping/recognition algorithms
//        if (contours.size() > 0)
//            MSERsToERStats(gray, contours, regions);
        break;
    }
    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
    qDebug()<<"detection time: "<<t_all<<endl;
    t_all = (double)getTickCount();

//    vector< vector<Vec2i> > nm_region_groups;
//    vector<Rect> nm_boxes;
//    switch (GROUPING_ALGORITHM)
//    {
//    case 0: // exhaustive_search
//        erGrouping(dst, channels, regions, nm_region_groups, nm_boxes, ERGROUPING_ORIENTATION_HORIZ);
//        break;
//    case 1: //multioriented
//        erGrouping(dst, channels, regions, nm_region_groups, nm_boxes, ERGROUPING_ORIENTATION_ANY, prefix+"trained_classifier_erGrouping.xml", 0.5);
//        break;
//    }
//    if(nm_boxes.size()>0){
//        for (int i=0; i<(int)nm_boxes.size(); i++)
//        {
//            rectangle(out_img, nm_boxes[i].tl(), nm_boxes[i].br(), Scalar(255,255,0),2);
//        }
//        qDebug()<<endl<<endl<<endl;
//        qDebug()<<"detected"<<endl;

//        t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
//        qDebug()<<"grouping time: "<<t_all<<endl;
//        qDebug()<<endl<<endl<<endl;
//        t_all = (double)getTickCount();
//    }


//    /*Text Recognition (OCR)*/
//    int bottom_bar_height= out_img.rows/7 ;
//    copyMakeBorder(dst, out_img, 0, bottom_bar_height, 0, 0, BORDER_CONSTANT, Scalar(150, 150, 150));
//    float scale_font = (float)(bottom_bar_height /85.0);
//    vector<string> words_detection;
//    float min_confidence1 = 0.f, min_confidence2 = 0.f;
//    if (RECOGNITION == 0)
//    {
//        min_confidence1 = 51.f;
//        min_confidence2 = 60.f;
//    }
//    vector<Mat> detections;
//    for (int i=0; i<(int)nm_boxes.size(); i++)
//    {
//        rectangle(out_img, nm_boxes[i].tl(), nm_boxes[i].br(), Scalar(255,255,0),3);
//        Mat group_img = Mat::zeros(dst.rows+2, dst.cols+2, CV_8UC1);
//        er_draw(channels, regions, nm_region_groups[i], group_img);
//        group_img(nm_boxes[i]).copyTo(group_img);
//        copyMakeBorder(group_img,group_img,15,15,15,15,BORDER_CONSTANT,Scalar(0));
//        detections.push_back(group_img);
//    }
//    vector<string> outputs((int)detections.size());
//    vector< vector<Rect> > boxes((int)detections.size());
//    vector< vector<string> > words((int)detections.size());
//    vector< vector<float> > confidences((int)detections.size());
//    // parallel process detections in batches of ocrs.size() (== num_ocrs)
//    for (int i=0; i<(int)detections.size(); i=i+(int)num_ocrs)
//    {
//        Range r;
//        if (i+(int)num_ocrs <= (int)detections.size())
//            r = Range(i,i+(int)num_ocrs);
//        else
//            r = Range(i,(int)detections.size());

//        switch(RECOGNITION)
//        {
//        case 0: // Tesseract
//            parallel_for_(r, Parallel_OCR<OCRTesseract>(detections, outputs, boxes, words, confidences, ocrs));
//            break;
//        case 1: // NM_chain_features + KNN
//            parallel_for_(r, Parallel_OCR<OCRHMMDecoder>(detections, outputs, boxes, words, confidences, decoders));
//            break;
//        }
//    }
//    for (int i=0; i<(int)detections.size(); i++)
//    {
//        outputs[i].erase(remove(outputs[i].begin(), outputs[i].end(), '\n'), outputs[i].end());
//        //cout << "OCR output = \"" << outputs[i] << "\" length = " << outputs[i].size() << endl;
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
//    }
////    t_all = ((double)getTickCount() - t_all)*1000/getTickFrequency();
////    qDebug()<<"ocr time: "<<t_all<<endl;

//    int text_thickness = 1+(out_img.rows/500);
//    string fps_info = format("%2.1f Fps. %dx%d", (float)(1000 / t_all), dst.cols, dst.rows);
//    putText(out_img, fps_info, Point( 10,out_img.rows-5 ), FONT_HERSHEY_DUPLEX, scale_font, Scalar(255,0,0), text_thickness);
//    putText(out_img, region_types_str[REGION_TYPE], Point((int)(out_img.cols*0.5), out_img.rows - (int)(bottom_bar_height / 1.5)), FONT_HERSHEY_DUPLEX, scale_font, Scalar(255,0,0), text_thickness);
//    putText(out_img, grouping_algorithms_str[GROUPING_ALGORITHM], Point((int)(out_img.cols*0.5),out_img.rows-((int)(bottom_bar_height /3)+4) ), FONT_HERSHEY_DUPLEX, scale_font, Scalar(255,0,0), text_thickness);
//    putText(out_img, recognitions_str[RECOGNITION], Point((int)(out_img.cols*0.5),out_img.rows-5 ), FONT_HERSHEY_DUPLEX, scale_font, Scalar(255,0,0), text_thickness);


    return out_img;
}
