#include "videoreader.h"
#include <QFileInfo>
#include <QDebug>
videoReader::videoReader(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerLoop()));
    imgFps = 1;
}

bool videoReader::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();// convert QString to char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //not a number
        return false;
    }
    else
    { //number
        return true;
    }
}

bool videoReader::open(QString file)
{
    currentFrame = 0;
    if(isDigitStr(file)){
        isImg = false;
        if(!videoCapture.open(file.toInt())){
            qDebug()<<":( opening IP camera failed"<<endl;
            return false;
        }else{
            qDebug()<<":) opening IP camera OK"<<endl;
            fps=videoCapture.get(CV_CAP_PROP_FPS);
            if(fps==0)
                fps=20;
            timer->setInterval(1000/fps);
            qDebug()<<"fps is: "<<fps<<endl;

            return true;
        }
    }else{
        QFileInfo fileInfo(file);
        if(fileInfo.isDir()){
            isImg = true;
            dir = new QDir(file);
            QStringList filter;
            filter << QLatin1String("*.png");
            filter << QLatin1String("*.jpeg");
            filter << QLatin1String("*.jpg");
            filter << QLatin1String("*.bmp");
            dir->setNameFilters(filter);
            totalFrame = dir->entryInfoList().size();
            if(totalFrame<=0){
                qDebug()<<":( image dir has no image"<<endl;
                return false;
            }else{
                qDebug()<<":) opening image dir OK"<<endl;
                fps = imgFps;
                timer->setInterval(1000/fps);
                qDebug()<<"fps is: "<<fps<<endl;
                return true;
            }

        }else if(fileInfo.isFile()){
            isImg = false;
            if(!videoCapture.open((file.toStdString().c_str()))){
                qDebug()<<":( opening video file failed"<<endl;
                return false;
            }else{
                totalFrame = videoCapture.get(CV_CAP_PROP_FRAME_COUNT);
                fps=videoCapture.get(CV_CAP_PROP_FPS);
                if(fps==0)
                    fps=30;
                timer->setInterval(1000/fps);
                qDebug()<<":) opening video file OK"<<endl;
                qDebug()<<"fps is: "<<fps<<endl;
                return true;
            }
        }else{
            return false;
        }
    }
}

bool videoReader::stop()
{
    if(videoCapture.isOpened())
        videoCapture.release();

    if(timer->isActive())
        timer->stop();

    currentFrame = 0;
    return true;
}

void videoReader::timerLoop()
{
    if(!isImg){
        if(totalFrame>0)// not webcam
        videoCapture.set(CV_CAP_PROP_POS_FRAMES,currentFrame);
        videoCapture>>rawFrame;
        if(totalFrame>0)// not webcam
        currentFrame=int(videoCapture.get(CV_CAP_PROP_POS_FRAMES));
        if(rawFrame.empty()){
            qDebug()<<"video end"<<endl;
            timer->stop();
        }else{
            emit newArrived();
        }
    }else{
        QFileInfoList filelistinfo = dir->entryInfoList();
        QString imageFile = filelistinfo.at(currentFrame).absoluteFilePath();
        if(currentFrame<totalFrame){
            rawFrame = cv::imread(imageFile.toStdString().c_str());
            currentFrame++;
            emit newArrived();
        }else{
            qDebug()<<"image folder end"<<endl;
            timer->stop();
        }
    }
}
