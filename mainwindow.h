#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QMainWindow>
#include <opencv2/opencv.hpp>
using namespace cv;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void openCamara();
    void readFame();
    void closeCamara();
    void takingPictures();

    void on_Slider_Threshold_sliderMoved(int position);

    void on_Slider_Contour_sliderMoved(int position);

    void on_MPP_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTimer    *timer;
    QImage    *imag;
    CvCapture *cam;
    IplImage  *frame;
};

#endif // MAINWINDOW_H
