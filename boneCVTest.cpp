
#include<iostream>
#include<opencv2/opencv.hpp>
#include <pthread.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <pthread.h>
using namespace std;
using namespace cv;

/*
void *getPicture(Mat *newMatrix, VideoCapture cap)
{
	cap >> newMatrix;
	if (newMatrix.empty())
	{
		cout << "Failed to capture an image" << endl;
		return;
	}
	pthread_exit(NULL);
}
*/

void *serialCommunicate(void *serialText)
{
	std::string* str = reinterpret_cast<std::string*>(serialText);
	cout << *str << endl;
	pthread_exit(NULL);
}

int main()
{
	pthread_t test_thread;
	string *userData = new string("somevalue");
	pthread_create(&test_thread, NULL, serialCommunicate, (void *) userData);
	int iLowH = 0;
	int iHighH = 179;
	
	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

    VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH,450); //1920 - 640 - 360
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,1000); //1080 - 480 - 800
    if(!cap.isOpened()){
	    cout << "Failed to connect to the camera." << endl;
    }
    Mat frame, edges, imgThresh; //Takes one second
    
    for (int i = 0; i < 4; i++)
    {
    	cap >> frame;
    	if(frame.empty())
	    {
			cout << "Failed to capture an image" << endl;
			return -1;
   	    }
  	cvtColor(frame, edges, CV_BGR2GRAY); //Takes one second
   	Mat imgHSV; //The thresholded image to be used
	cvtColor(frame, imgHSV, COLOR_BGR2HSV);	
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresh);
	//Canny(edges, edges, 7, 50, 3); // 0, 30, 3
	
    }
	

    imwrite("edges.png", imgThresh);
    //imwrite("capture.png", frame);
    return 0;
}
