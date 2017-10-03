#ifndef VIDEOREADER_H
#define VIDEOREADER_H

#include <QObject>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTimer>
#include <QDir>
class videoReader : public QObject
{
    Q_OBJECT
public:
    explicit videoReader(QObject *parent = nullptr);
    cv::VideoCapture videoCapture;
    QTimer *timer;
    bool isDigitStr(QString src);

    bool open(QString file);
    bool pause();
    bool isOpened;
    bool stop();
    bool setFrame(int frame);

    bool isImg;
    QDir *dir;

    int totalFrame;
    int currentFrame;
    int fps;
    int imgFps;

    cv::Mat rawFrame;
signals:
    void newArrived();
public slots:
    void timerLoop();
};

#endif // VIDEOREADER_H
