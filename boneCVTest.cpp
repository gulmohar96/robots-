
#include<iostream>
#include<opencv2/opencv.hpp>
#include <stdlib.h>
#include <pthread.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <pthread.h>
using namespace std;
using namespace cv;

//#define intToString ( x ) dynamic_cast< std::ostringstream & >( \
//		( std::ostringstream() << std::dec << x ) ).str() // This is the macro for converting an int into a string. Copied this one haha, better than mine


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

bool entireProgramFinished = false; // This boolean represents when the program should stop sending signals to the two arduinos
RNG rng(12345);
unsigned const int MIN_AREA_OF_CONTOUR = 500; // THis represents the min size of the object that will be detected

struct robotSpeed {
	int speedOne;
	int speedTwo;
	int speedThree;
	int speedFour;
	char leftSideDirection; // Varies between S, B, and F. Guess what they represent
	char rightSideDirection;
} speedControl;

struct servoControls {
	int gunXCoordinate;
	int gunYCoordinate;
	int gunFire;
} gunControl;


string IntegerToString ( int number ) // Little helper function that I use to convert integers to strings
{
	ostringstream ss; // Just use a quick ostringstream
	ss << number;
	return ss.str();
}

string CharToString ( char character )
{
	stringstream ss;
	string s;
	ss << character;
	ss >> s;
	return s;
}

void initSerial()
{
	speedControl.speedOne = 0;
	speedControl.speedTwo = 0;
	speedControl.speedThree = 0;
	speedControl.speedFour = 0;
	speedControl.leftSideDirection = 'S'; // This needs to be checked to make sure it isn't a boolean in Arduino
	speedControl.rightSideDirection = 'S';
	
	gunControl.gunXCoordinate = 40;
	gunControl.gunYCoordinate = 75;
	gunControl.gunFire = 0;
} // Turn the below serial communication into a seperate function with the thread.join inside the function :)

void *serialCommunicate(void*)
{	
	while (!entireProgramFinished) 
	{
		string serialData = "sudo python serialTest.py " + IntegerToString( speedControl.speedOne ) + " " + IntegerToString( speedControl.speedTwo ) + " "
							+ IntegerToString( speedControl.speedThree ) + " " + IntegerToString( speedControl.speedFour ) 
							+ " " + CharToString( speedControl.leftSideDirection ) + " " + CharToString( speedControl.rightSideDirection ) + 
							+ " " + IntegerToString( gunControl.gunXCoordinate ) + " " + IntegerToString( gunControl.gunYCoordinate ) 
							+ " " + IntegerToString( gunControl.gunFire );
		//std::string* str = reinterpret_cast<std::string*>(serialText);
		cout << serialData << endl;
		//std::string strNew = *str;
		system(serialData.c_str()); // May god forgive me for using c_str
		pthread_exit(NULL);
	}
}

int main()
{
	entireProgramFinished = false;
	initSerial();
	pthread_t test_thread;
	pthread_create(&test_thread, NULL, serialCommunicate, NULL);
	int iLowH = 100;
	int iHighH = 140;
	
	int iLowS = 135;
	int iHighS = 255;

	int iLowV = 135;
	int iHighV = 255;
	
    VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH,360); //1920 - 640 - 360 - 450
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,800); //1080 - 480 - 800 - 1000
    if(!cap.isOpened()){
	    cout << "Failed to connect to the camera." << endl;
    }
    Mat frame, edges, imgThresh, drawing; //Takes one second
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	

    for (int i = 0; i < 6; i++)
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

	// Now what we need to do is find the contours of the image
	
		findContours(imgThresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	
	//Now that we've found the contours of the blue things, we can draw a rectangle around them:
		
		unsigned int noOfLargeObjects = 0;
		
		vector<vector<Point> > contours_poly( contours.size() );
		vector<Rect> boundRect ( contours.size() );

		for (int i = 0; i < contours.size(); i++)
		{
			if (contourArea(contours[i]) >= MIN_AREA_OF_CONTOUR) 
			{		
				approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
				boundRect[noOfLargeObjects] = boundingRect( Mat(contours_poly[i]) );
				noOfLargeObjects++;
			}
		}

		// Ok now we need to merge the rectangles before we can actually draw them, because several ones will be drawn strangely 

/*		for (int i = 0; i < noOfLargeObjects; i++ )
		{
			for (int j = 0; j < noOfLargeObjects; j++)
			{

			}
		}		
*/
		// Ok now we can actually draw the rectangles
			
		for ( int i = 0; i < noOfLargeObjects; i++ )
		{
			// cout << "Area: " << contourArea(contours[i]) << endl; -- this won't work in this context btw, don't try it :P
			// Scalar colour = Scalar( 0, 255, 255 ); // B/G/R - yellow - can remove just for reference
			// drawContours( frame, contours, i, colour, 2, 8, hierarchy, 0, Point() ); // This draws the shape of the device - yellow
			rectangle( frame, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 255), 2, 8, 0); // This just draws a rectangle around - yellow
		}	

	//Canny(edges, edges, 7, 50, 3); // 0, 30, 3
	
    }
	
	
	int success = pthread_join(test_thread, NULL);
	if (success) {
		cout << "Unable to terminate serial communication, " << success << endl;
		exit(-1);
	}	
	
	entireProgramFinished = true;
    imwrite("capture.png", frame);
    return 0;
}
