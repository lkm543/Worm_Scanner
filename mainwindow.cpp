#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "time.h"
#include "opencv2/opencv.hpp"
#include<iostream>
#include<fstream>
using namespace cv;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam     = NULL;
       timer   = new QTimer(this);
       imag    = new QImage();

       connect(timer, SIGNAL(timeout()), this, SLOT(readFame()));
       connect(ui->open, SIGNAL(clicked()), this, SLOT(openCamara()));
       //connect(ui->Threshold, SIGNAL(clicked()), this, SLOT(takingPictures()));
       connect(ui->closeCam, SIGNAL(clicked()), this, SLOT(closeCamara()));
}

MainWindow::~MainWindow()
{
    delete ui;

}

std::ofstream fp;

int cols,rows;
//0 inactive 1 active
int flag_close=0,flag_edge=0,flag_threshold=0;
int Value_Threshold=45,Value_Contour=80;
int x_prev=0;
int x_now=0;
int y_prev=0;
int y_now=0;
double velocity,MPP=0.01;
Mat image_threshold,image_gray,image_edge,image_output;
clock_t start,tpf_start,tpf_finish;//tpf=time per frame
String filename = "test.wmv";
VideoCapture capture(filename);
//VideoCapture capture;s
Mat image_Mat;
std::vector<cv::Point> feature_prev, features_next;
void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::openCamara()
{
    fp.open("Data.txt");//開啟檔案
    //fp<<"haha";
    cam = cvCreateCameraCapture(0);
    //flag_close=0;
    ui->Label_Camera_Situation->setText("Camera：On");

    timer->start(30);

}
void MainWindow::readFame()
{       //timer
        start=clock();
        tpf_start=clock();
        capture >> image_Mat;
        //cvFlip(frame,frame,1);
        if(!image_Mat.empty()){
        cols=image_Mat.cols;
        rows=image_Mat.rows;

        //Prepare for velocity calcaulation
        x_prev=x_now;
        y_prev=y_now;

        Mat image_output=cvCreateMat(cols,rows,CV_8UC3);
        Mat image_HSV=cvCreateMat(cols,rows,CV_8UC3);
        Mat image_H=cvCreateMat(cols,rows,CV_8UC1);


        //Get H value
        vector<Mat> channels;
        cv::cvtColor(image_Mat,image_HSV,CV_BGR2HSV);
        split(image_HSV,channels);
        image_H=channels.at(1);

        cv::threshold(image_H, image_H, Value_Threshold, 255, 0);
        //Fill in the contour first to erode, so that we can diminish the noise
        cv::dilate(image_H, image_H, Mat(), Point(-1, -1), 5, 1, 1);
        cv::erode(image_H, image_H, Mat(), Point(-1, -1), 5, 1, 1);
        cv::Canny(image_H, image_output, 30, 30*3, 3);

        //Find the contours. Use the contourOutput Mat so the original image doesn't get overwritten
        vector<Vec4i> hierarchy;
        RNG rng(12345);
        std::vector<std::vector<cv::Point> > contours;
        cv::Mat contourOutput = image_output.clone();
        cv::findContours(contourOutput, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

        //small contour filter
        for (vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end(); ){
            if (it->size()<Value_Contour)
                it=contours.erase(it);
            else
                ++it;
        }

        //Contour left after filter
        if(contours.size()>0){
        // Approximate contours to polygons + get bounding rects and circles
        vector<vector<Point> > contours_poly( contours.size() );
        vector<Rect> boundRect( contours.size() );
        vector<Point2f>center( contours.size() );
        vector<float>radius( contours.size() );

        for( int i = 0; i < contours.size(); i++ )
           { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
             boundRect[i] = boundingRect( Mat(contours_poly[i]) );
             minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
           }

        //New point of centroid
        x_now=center.at(0).x;
        y_now=center.at(0).y;

        //Draw the contours
        cv::Mat contourImage(image_Mat.size(), CV_8UC3, cv::Scalar(0,0,0));
        for( int i = 0; i< contours.size(); i++ ){
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            drawContours( contourImage, contours, i, color, 2, 8, hierarchy, 0, Point() );
            rectangle( contourImage, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        }

        QImage image2((const uchar*)contourImage.data, cols, rows, QImage::Format_RGB888);
        ui->Pic2->setPixmap(QPixmap::fromImage(image2));
        ui->Pic2->resize(ui->Pic2->pixmap()->size());

        ui->Label_Position_X->setText("Position X->"+QString::number(center.at(0).x));
        ui->Label_Position_Y->setText("Position Y->"+QString::number(center.at(0).y));
        }
        //More than one contour,means the variable of contour filter is not enough
        if (contours.size()>1){
        ui->Label_Situation->setText("Low Contour Filter!!!!");
        }
        //Only one contour left
        else if (contours.size()>0){
        ui->Label_Situation->setText("Fine");
        }
        //No contour left
        else{
        ui->Label_Situation->setText("Over Contour Filter!!!!");
        }

        tpf_finish=clock();
        ui->Label_Time->setText("Duration  "+QString::number(start)+"  ms");
        //30 = Start frame
        ui->Label_Time_PerFrame->setText("Frame Duration  "+QString::number(tpf_finish-tpf_start+30)+"  ms");

        //Velocity calculation
        velocity=sqrt(pow((x_now-x_prev),2)+pow((y_now-y_prev),2))*1000/(tpf_finish-tpf_start+30)*MPP;
        ui->Label_Velocity->setText("Velocity ->"+QString::number(velocity));

        //Write into text
        String word=std::to_string(velocity);
        word+="\n";
        fp<<word;

        //Show image
        QImage image((const uchar*)image_Mat.data, cols, rows, QImage::Format_RGB888);
        ui->Pic->setPixmap(QPixmap::fromImage(image));
        ui->Pic->resize(ui->Pic->pixmap()->size());
        QImage image3((const uchar*)image_H.data, cols, rows, QImage::Format_Indexed8);
        ui->Pic3->setPixmap(QPixmap::fromImage(image3));
        ui->Pic3->resize(ui->Pic3->pixmap()->size());
        }

        //End of Video
        else{
            fp.close();
            timer->stop();
        }
}
void MainWindow::takingPictures()
{
}

void MainWindow::closeCamara()
{
        timer->stop();
        ui->Label_Camera_Situation->setText("Camera Pause");
        //cvReleaseCapture(&cam);
        flag_close=1;
}


void MainWindow::on_Slider_Threshold_sliderMoved(int position)
{
    Value_Threshold=position;
    ui->Label_Threshold->setText("Threshold Value "+QString::number(Value_Threshold));
}

void MainWindow::on_Slider_Contour_sliderMoved(int position)
{
    Value_Contour=position;
    ui->Label_Contour->setText("Contour Filter Value "+QString::number(Value_Contour));
}

void MainWindow::on_MPP_textChanged(const QString &arg1)
{
    MPP=arg1.toDouble();
}
