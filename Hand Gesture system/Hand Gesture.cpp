// Hand Gesture system.cpp : Defines the entry point for the console application.

//Define for all necessary headers
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* _beginthread, _endthread */
#include <process.h>
#include <stddef.h>

//timer for counter
#include <time.h>

//opencv headers
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/objdetect/objdetect.hpp>

// hide the windows console which is displayed in the background of the 
#include <windows.h>
#include <WinUser.h>
#include <iostream>
#define _WIN32_WINNT 0x0500

//boost
#include <boost\thread.hpp>

//using namespaced
using namespace std;
using namespace cv;


//Hide the console... No one should see it, unless required.
void windowHideConsole()
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow( hWnd, SW_HIDE);
}

//hand structure model for detail gesture the is form the touch UI
//which will be mimiced by the system
struct handRatio
{
	//all distance from the center of the palm
	double legth1, defect1;
	double legth2, defect2;
	double legth3, defect3;
	double legth4, defect4;
	double legth5, defect5;

	double legth6, defect6;
	double legth7, defect7;
	double legth8, defect8;
	double legth9, defect9;
	double legth10, defect10;
};

/*
	The Class eventTrigger is used to trigger the mouse event on the system.
	This is (3rd)Three Module which is involved with the system <<<<Event Triggering
*/

//Event trigger for the mouse events
class eventTrigger
{
	private:

	//mouse varaibles
	int xMouse,yMouse;
		
	//adapter setting
	int screenHeight,screenWidth;
	int cameraHeight,cameraWidth;
	int totalPixel;

	double virtualScreenRatioHeight;
	double virtualScreenRatioWidth;
		
	double xRatio,yRatio;

	public:

	eventTrigger(int ScreenHeight,int ScreenHWidth,int CameraHeight,int CameraWidth)
	{
		screenHeight=ScreenHeight;
		screenWidth=ScreenHWidth;
		cameraHeight=CameraHeight;
		cameraWidth=CameraWidth;

		totalPixel = screenWidth * screenHeight;

		//calculate ratio
		yRatio = (double)((double)screenHeight/(double)cameraHeight);
		xRatio = (double)((double)screenWidth/(double)cameraWidth);

		//convertion ratio for the virtual screen
		virtualScreenRatioHeight = (double)(65536/(double)screenHeight);
		virtualScreenRatioWidth = (double)(65536/(double)screenWidth);

		//reset the mouse to the center of the screen
		yMouse = virtualScreenRatioHeight * ((double)screenHeight/2);
		xMouse = virtualScreenRatioWidth * ((double)screenWidth/2);
		setmousePosition(0,0);

		cout<<"\n\nMouseEventTrigger Object Created...\nscreen height:"<<screenHeight<<" screen width:"<<screenWidth<<" camera height:"<<cameraHeight<<"camera width:"<<cameraWidth;
		cout<<"\nMouse X:"<<xMouse<<"Mouse Y:"<<yMouse;
	}

	void setmousePosition(int dx,int dy)
	{
		dx = (double)virtualScreenRatioWidth  * ((double)dx * xRatio);
		dy = (double)virtualScreenRatioHeight * ((double)dy * yRatio);

		if(((long)(xMouse+dx)<=65535) && ((long)(xMouse+dx)>0))
		{
			xMouse=xMouse+dx;
		}


		if((long)(yMouse+dy)<=65535 && ((long)(yMouse+dy)>0))
		{
			yMouse=yMouse+dy;
		}

		if(xMouse>=0 && xMouse<=65535 && yMouse>=0 && yMouse<=65535)
		{
			INPUT input;
			input.type=INPUT_MOUSE;
			//set coordinates
			input.mi.dx=xMouse;
			input.mi.dy=yMouse;

			//inital Flag of the mouse
			input.mi.dwFlags=(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK|MOUSEEVENTF_MOVE);
			input.mi.mouseData=0;
			input.mi.dwExtraInfo=NULL;
			input.mi.time=0;
			SendInput(1,&input,sizeof(INPUT));
		}
		//cout<<"\nx:"<<xMouse<<" y:"<<yMouse;
	}

	//Right mouse hold
	void setMouseRightHold()
	{
		if(xMouse>=0 && xMouse<=65535 && yMouse>=0 && yMouse<=65535)
		{
			INPUT input;
			input.type=INPUT_MOUSE;

			//Point of the mouse
			input.mi.dx=xMouse;
			input.mi.dy=yMouse;

			//SET inital state of the mouse -> Flag varialbe 
			input.mi.dwFlags=(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK|MOUSEEVENTF_MOVE|MOUSEEVENTF_RIGHTDOWN); 
			input.mi.mouseData=0;
			input.mi.dwExtraInfo=NULL;
			input.mi.time=0;
			SendInput(1,&input,sizeof(INPUT));
		}
		//cout<<"\nx:"<<x<<" y:"<<y<<" Hold Right"; 
	}

	//Right mouse leave
	void setMouseRightLeave()
	{	
		if(xMouse>=0 && xMouse<=65535 && yMouse>=0 && yMouse<=65535)
		{
			INPUT input;
			input.type=INPUT_MOUSE;
			//Point of the mouse
			input.mi.dx=xMouse;
			input.mi.dy=yMouse;

			//SET inital state of the mouse -> Flag varialbe 
			input.mi.dwFlags=(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK|MOUSEEVENTF_MOVE|MOUSEEVENTF_RIGHTUP); //|MOUSEEVENTF_MOVE)|MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_RIGHTUP
			input.mi.mouseData=0;
			input.mi.dwExtraInfo=NULL;
			input.mi.time=0;
			SendInput(1,&input,sizeof(INPUT));
		}
		//cout<<"\nx:"<<x<<" y:"<<y<<" Leave Right"; 
	}

	//Left mouse Hold		
	void setMouseLeftHold()
	{
		if(xMouse>=0 && xMouse<=65535 && yMouse>=0 && yMouse<=65535)
		{
			INPUT input;
			input.type=INPUT_MOUSE;

			//Point of the mouse
			input.mi.dx=xMouse;
			input.mi.dy=yMouse;
			
			//SET inital state of the mouse -> Flag varialbe 
			input.mi.dwFlags=(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK|MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN); //|MOUSEEVENTF_MOVE)|MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_RIGHTUP
			input.mi.mouseData=0;
			input.mi.dwExtraInfo=NULL;
			input.mi.time=0;
			SendInput(1,&input,sizeof(INPUT));
		}
		//cout<<"\nx:"<<x<<" y:"<<y<<" Hold Left"; 
	}

	//Left mouse leave
	void setMouseLeftLeave()
	{
		if(xMouse>=0 && xMouse<=65535 && yMouse>=0 && yMouse<=65535)
		{
			INPUT input;
			input.type=INPUT_MOUSE;

			//Point of the mouse
			input.mi.dx=xMouse;
			input.mi.dy=yMouse;

			//SET inital state of the mouse -> Flag varialbe 
			input.mi.dwFlags=(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK|MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTUP); //|MOUSEEVENTF_MOVE)|MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_RIGHTUP
			input.mi.mouseData=0;
			input.mi.dwExtraInfo=NULL;
			input.mi.time=0;
			SendInput(1,&input,sizeof(INPUT));
		}
		//cout<<"\nx:"<<x<<" y:"<<y<<" Leave Left"; 
	}

	//Single CLick of the Left Button
	void setMouseLeftClick()
	{
		setMouseLeftHold();
		setMouseLeftLeave();
		//cout<<"\n>>x:"<<xMouse<<" y:"<<yMouse<<" Single Left Click";
	}
	
	//Single CLick of the Right Button
	void setMouseRightClick()
	{
		setMouseRightHold();
		setMouseRightLeave();
		//cout<<"\n>>x:"<<xMouse<<" y:"<<yMouse<<" Single Right Click";
	}

	//Double CLick of the Left Button
	void setMouseLeftDoubleClick()
	{
		setMouseLeftClick();
		setMouseLeftClick();
		//cout<<"\nx:"<<x<<" y:"<<y<<" Double Left Click";
	}

	//Double CLick of the Right Button
	void setMouseRightDoubleClick()
	{
		setMouseRightClick();
		setMouseRightClick();
		//cout<<"\nx:"<<x<<" y:"<<y<<" Double Right Click";
	}

}*mousePosition;

/*
	The Class cameraSoftware is used to analysis and detection the various objects and also the image enhancement 
	This is (1st and 2nd))First & Second Module which is involved with the system <<<<Event Triggering
*/
class cameraSoftware
{
	private:
			
	//sort point array
	struct pointWrapper
	{
		Point pointValue;
		int sortValues;
	};

	/*Camera setting*/
	bool isFlipImage;
	
	//adapter setting
	int screenHeight,screenWidth;
	int cameraHeight,cameraWidth;
	int correctionHeight,correctionWidth;
	int totalPixel;

	double xRatio,yRatio;
	double xCorrection,yCorrection;

	/*The camera variables and the feed which can be used in the other process of the system*/
	VideoCapture *capture; //camera capture variable
	bool runCamFeed;	//A variable to run the loop of the camera
	Mat imgSrcMat; //The Source matrixof the Video Feed

	Mat imgDestMat; //The matrix for the Video Feed
	Mat imgDestMatHSV; //HSV version of the image
	Mat imgDestMatGrayScale; // the matrix for the video feed as gray scale
	Mat imgDestMatEdge; //Edge Detection matrix
	
	Mat background; //background matrix
    Mat foreground; //foreground matrix

	//get screen resolution for the given os
	void calculateScreenResolution()
	{   
		cout<<"\nGetting screen resolution for convertion...";
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		/* 
			The top left corner will have coordinates (0,0)
			and the bottom right corner will have coordinates
			(horizontal, vertical) 
		*/  
		screenWidth = desktop.right;
		screenHeight = desktop.bottom;

		totalPixel = screenWidth * screenHeight;

		cout<<"\nReset Error Correction For width, height and lost ratio...";
		correctionHeight=0;
		correctionWidth=0;
		xCorrection=1;
		yCorrection=1;
	}

	void correctionResolution(Point point1,Point point2)
	{
		static int area;
		int width= point2.x - point1.x,height= point2.y - point1.y;

		if(area < (width*height))
		{
			area = width * height;
			correctionWidth = (int)((double)width/(double)2);
			correctionHeight = (int)((double)height/(double)2);

			xCorrection = ((double)cameraWidth/(double)(cameraWidth-width));
			yCorrection = ((double)cameraHeight/(double)(cameraHeight-height));

			cout<<"\nCorrection Activated";
			cout<<"\nwidth:"<<width<<"height:"<<height;
		}
	}

	//find the biggest contour so as to return to the system skin detection
	int findBiggestContour(vector<vector<Point> > contours)
	{
		int indexOfBiggestContour = -1;
		int sizeOfBiggestContour = 0;
		
		for (unsigned int i = 0; i < contours.size(); i++)
		{
			if(contours[i].size() > sizeOfBiggestContour)
			{
			    sizeOfBiggestContour = contours[i].size();
				indexOfBiggestContour = i;
			}
		}
		return indexOfBiggestContour;
	}

	//palm Detection this is used to find the mouse pointer NOTE::A RECTANGLE IS RETURNED BACK TO THE CALL
	Point detectFaceMask( Mat imgSrc, Point &rpts1, Point &rpts2, int &isFound)
	{
		//reset isFound
		isFound=0;

		static Point fpts1,fpts2,midPt;

		//Palm detection prototype
		// Create memory for calculations
		static CvMemStorage* storage=0;

		// Create a new Haar classifier
		static CvHaarClassifierCascade* cascade=0;

		// Create a string that contains the cascade name  <<<<>>>>>CHANGE
		char* cascade_name =  "F:\\Personal and Projects\\project\\Hand Gesture System\\Hand Gesture system\\Release\\face.xml";

		// Load the HaarClassifierCascade
		cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    
		// Check whether the cascade has loaded successfully. Else report and error and quit
		if( !cascade )
		{
			fprintf( stderr, "\nERROR: Could not load classifier cascade\n" );
			exit(0);
		}

		// Allocate the memory storage
		storage = cvCreateMemStorage(0);

		float scale = 1;

		// Create a new image based on the input image
		IplImage* temp = &IplImage(imgSrc);
		
		// Create two points to represent the hand locations
		Point pt1, pt2;
		double area=0;

		int i;

		// Clear the memory storage which was used before
		cvClearMemStorage( storage );

		// Find whether the cascade is loaded, to find the faces. If yes, then:
		if( cascade )
		{
			// There can be more than one face in an image. So create a growable sequence of faces.
			// Detect the objects and store them in the sequence
			CvSeq* faces = cvHaarDetectObjects( temp, cascade, storage,1.3, 3, CV_HAAR_DO_CANNY_PRUNING,Size(100,100));

			// Loop the number of faces found.
			for( i = 0; i < (faces ? faces->total : 0); i++ )
			{
				//I found a palm
				isFound=1;

				// Create a new rectangle for drawing the face
				CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

				// Find the dimensions of the face,and scale it if necessary
				pt1.x = r->x*(1/scale);
				pt2.x = (r->x+r->width)*scale;
				pt1.y = r->y*(1/scale);
				pt2.y = (r->y+r->width)*scale;

				if((r->width * r->height * scale) > area)
				{
					fpts1 = pt1;
					fpts2 = pt2;
					
					midPt.x = (fpts1.x + fpts2.x)/2; 
					midPt.y = (fpts1.y + fpts2.y)/2;

					area = (r->width * r->height * scale);
				}
			}


		}
		rpts1 = fpts1;
		rpts2 = fpts2;

		return (midPt);
	}

	//palm Detection this is used to find the mouse pointer NOTE::A RECTANGLE IS RETURNED BACK TO THE CALL
	Point detectFistMask( Mat imgSrc, Point &rpts1, Point &rpts2, int &isFound)
	{
		//reset isFound
		isFound=0;

		static Point fpts1,fpts2,midPt;

		//Palm detection prototype
		// Create memory for calculations
		static CvMemStorage* storage=0;

		// Create a new Haar classifier
		static CvHaarClassifierCascade* cascade=0;

		// Create a string that contains the cascade name  <<<<>>>>>CHANGE
		char* cascade_name =  "F:\\Personal and Projects\\project\\Hand Gesture System\\Hand Gesture system\\Release\\fist.xml";

		// Load the HaarClassifierCascade
		cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    
		// Check whether the cascade has loaded successfully. Else report and error and quit
		if( !cascade )
		{
			fprintf( stderr, "\nERROR: Could not load classifier cascade\n" );
			exit(0);
		}

		// Allocate the memory storage
		storage = cvCreateMemStorage(0);

		float scale = 1;

		// Create a new image based on the input image
		IplImage* temp = &IplImage(imgSrc);
		
		// Create two points to represent the hand locations
		Point pt1, pt2;
		double area=0;

		int i;

		// Clear the memory storage which was used before
		cvClearMemStorage( storage );

		// Find whether the cascade is loaded, to find the faces. If yes, then:
		if( cascade )
		{
			// There can be more than one face in an image. So create a growable sequence of faces.
			// Detect the objects and store them in the sequence
			CvSeq* faces = cvHaarDetectObjects( temp, cascade, storage,1.2, 3, CV_HAAR_DO_CANNY_PRUNING,Size(100,100));

			// Loop the number of faces found.
			for( i = 0; i < (faces ? faces->total : 0); i++ )
			{
				//I found a palm
				isFound=1;

				// Create a new rectangle for drawing the face
				CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

				// Find the dimensions of the face,and scale it if necessary
				pt1.x = r->x*(1/scale);
				pt2.x = (r->x+r->width)*scale;
				pt1.y = r->y*(1/scale);
				pt2.y = (r->y+r->width)*scale;

				if((r->width * r->height * scale) > area)
				{
					fpts1 = pt1;
					fpts2 = pt2;
					
					midPt.x = (fpts1.x + fpts2.x)/2; 
					midPt.y = (fpts1.y + fpts2.y)/2;

					area = (r->width * r->height * scale);
				}
			}


		}
		rpts1 = fpts1;
		rpts2 = fpts2;

		return (midPt);
	}
	
	//Hand Detection to mask out of the hand, this will be useful for complex gestures
	Point detectHandMask( Mat imgSrc, Point rpts1, Point rpts2, Point &HLpts1, Point &HLpts2)
	{

		Point fpts1,fpts2,midPt;
		static  Mat finalMat = Mat::zeros(imgSrc.size(), CV_8UC1);

		static Point finalPt1,finalPt2,finalMidPt;

		//Palm detection prototype
		// Create memory for calculations
		static CvMemStorage* storage=0;

		// Create a new Haar classifier
		static CvHaarClassifierCascade* cascade=0;

		// Create a string that contains the cascade name  <<<<>>>>>CHANGE => haarcascade_frontalface_default.xml
		char* cascade_name =  "F:\\Personal and Projects\\project\\Hand Gesture System\\Hand Gesture system\\Release\\hand.xml";

		// Load the HaarClassifierCascade
		cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    
		// Check whether the cascade has loaded successfully. Else report and error and quit
		if( !cascade )
		{
			fprintf( stderr, "\nERROR: Could not load classifier cascade\n" );
			exit(0);
		}

		// Allocate the memory storage
		storage = cvCreateMemStorage(0);

		float scale = 1;

		// Create a new image based on the input image
		IplImage* temp = &IplImage(imgSrc);
		
		// Create two points to represent the hand locations
		Point pt1, pt2;
		double area=0;

		int i;

		// Clear the memory storage which was used before
		cvClearMemStorage( storage );

		// Find whether the cascade is loaded, to find the faces. If yes, then:
		if( cascade )
		{
			// There can be more than one face in an image. So create a growable sequence of faces.
			// Detect the objects and store them in the sequence
			CvSeq* faces = cvHaarDetectObjects( temp, cascade, storage,1.2, 3, CV_HAAR_DO_CANNY_PRUNING,Size(100, 100));

			// Loop the number of faces found.
			for( i = 0; i < (faces ? faces->total : 0); i++ )
			{
				// Create a new rectangle for drawing the face
				CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

				// Find the dimensions of the face,and scale it if necessary
				pt1.x = r->x*scale * (1/scale);
				pt2.x = (r->x+r->width)*scale;
				pt1.y = r->y*scale * (1/scale);
				pt2.y = (r->y+r->height)*scale;

				if((r->width * r->height * scale) > area)
				{
					fpts1 = pt1;
					fpts2 = pt2;
					
					midPt.x = (fpts1.x + fpts2.x)/2; 
					midPt.y = (fpts1.y + fpts2.y)/2;

					area = (r->width * r->height * scale);
				}
			}
			if((midPt.x > rpts1.x && midPt.x < rpts2.x) && (midPt.y > rpts1.y && midPt.y < rpts2.y))
			{
				finalPt1 = fpts1;
				finalPt2 = fpts2;

				finalMidPt = midPt;
			}
		}
		HLpts1 = finalPt1; 
		HLpts2 = finalPt2; 

		return (finalMidPt);
	}
		
	//Fist Detection to find if a default click operation was found.
	Point detectPalmMask( Mat imgSrc, Point &rpts1, Point &rpts2,int &isFound)
	{
		isFound = 0;

		Point fpts1,fpts2,midPt;
		static  Mat finalMat = Mat::zeros(imgSrc.size(), CV_8UC1);

		static Point finalPt1,finalPt2,finalMidPt;

		//Palm detection prototype
		// Create memory for calculations
		static CvMemStorage* storage=0;

		// Create a new Haar classifier
		static CvHaarClassifierCascade* cascade=0;

		// Create a string that contains the cascade name  <<<<>>>>>CHANGE => haarcascade_frontalface_default.xml
		char* cascade_name =  "F:\\Personal and Projects\\project\\Hand Gesture System\\Hand Gesture system\\Release\\palm.xml";

		// Load the HaarClassifierCascade
		cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    
		// Check whether the cascade has loaded successfully. Else report and error and quit
		if( !cascade )
		{
			fprintf( stderr, "\nERROR: Could not load classifier cascade\n" );
			exit(0);
		}

		// Allocate the memory storage
		storage = cvCreateMemStorage(0);

		float scale = 1;

		// Create a new image based on the input image
		IplImage* temp = &IplImage(imgSrc);
		
		// Create two points to represent the hand locations
		Point pt1, pt2;
		double area=0;

		int i;

		// Clear the memory storage which was used before
		cvClearMemStorage( storage );
		 
		// Find whether the cascade is loaded, to find the faces. If yes, then:
		if( cascade )
		{
			// There can be more than one face in an image. So create a growable sequence of faces.
			// Detect the objects and store them in the sequence
			CvSeq* faces = cvHaarDetectObjects( temp, cascade, storage,1.2, 3, CV_HAAR_DO_CANNY_PRUNING,Size(100,100));

			// Loop the number of faces found.
			for( i = 0; i < (faces ? faces->total : 0); i++ )
			{
				//fist found
				isFound=1;

				// Create a new rectangle for drawing the face
				CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

				// Find the dimensions of the face,and scale it if necessary
				pt1.x = r->x*scale * (1/scale);
				pt2.x = (r->x+r->width)*scale;
				pt1.y = r->y*scale * (1/scale);
				pt2.y = (r->y+r->height)*scale;

				if((r->width * r->height * scale) > area)
				{
					fpts1 = pt1;
					fpts2 = pt2;
					
					midPt.x = (fpts1.x + fpts2.x)/2; 
					midPt.y = (fpts1.y + fpts2.y)/2;

					area = (r->width * r->height * scale);
				}
			}
			if((midPt.x > rpts1.x && midPt.x < rpts2.x) && (midPt.y > rpts1.y && midPt.y < rpts2.y))
			{
				finalPt1 = fpts1;
				finalPt2 = fpts2;

				finalMidPt = midPt;
			}
		}
		rpts1 = finalPt1; 
		rpts2 = finalPt2; 

		return (finalMidPt);
	}

	//Shift of matrix MODULE NEED TO BE REPAIRED
	Mat shiftMatrixPositive(Mat sourceMat,int shiftValueX,int shiftValueY)
	{
		Mat dstMat = Mat::zeros(sourceMat.size(), CV_8UC1);

		//Matrix shift
		for( int i = shiftValueX; i < sourceMat.rows; i++ )
   		{
			for( int j = shiftValueY; j < sourceMat.cols; j++ )
   			{
				dstMat.at<unsigned char>(i,j) = sourceMat.at<unsigned char>(((i+shiftValueX)%sourceMat.rows),((j+shiftValueY)%sourceMat.cols));
				//cout<<dstMat.at<unsigned char>(i,j)<<" ";
			}
		}
		return dstMat;
	}

	//Shift of matrix MODULE NEED TO BE REPAIRED
	Mat shiftMatrixNegative(Mat sourceMat,int shiftValueX,int shiftValueY)
	{
		Mat dstMat;
		sourceMat.copyTo(dstMat);

		//Matrix shift
		for( int i = sourceMat.rows-1; i >=shiftValueX; i-- )
   		{
			for( int j = sourceMat.cols-1; j >=shiftValueY ;j-- )
   			{
				dstMat.at<unsigned char>(i,j) = sourceMat.at<unsigned char>(((i+shiftValueX)%sourceMat.rows),((j+shiftValueY)%sourceMat.cols));
				//cout<<dstMat.at<unsigned char>(i,j)<<" ";
			}
		}
		return dstMat;
	}

	//edge detection Canny apparently a good algorithm for edge detection
	Mat cannyEdgeDetection(Mat srcGrayScale, int lowerThreshold=9)
	{
		//variables required for detection
		int edgeThresh = 1;
		int lowThreshold=lowerThreshold;
		int const max_lowThreshold = 100;
		int ratio = 3;
		int kernel_size = 3;
		
		Mat detectedEdges,dst;

		/// Reduce noise with a kernel 3x3
		//detectedEdges = srcGrayScale;
		blur(srcGrayScale, detectedEdges, Size(3,3));

		/// Canny detector
		Canny(detectedEdges,detectedEdges,lowThreshold, lowThreshold*ratio,kernel_size);

		 /// Using Canny's output as a mask, we display our result
		dst = Scalar::all(0);

		srcGrayScale.copyTo(dst,detectedEdges);
		
		return dst;
	}

	//Find the biggest contour in an image and flood fills it...
	Mat contourAdjustment(Mat sourceMat)
	{
		vector<vector<Point>> contours;
		findContours(sourceMat.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		int biggest = findBiggestContour(contours);

		Mat dst = Mat::zeros(sourceMat.size(),sourceMat.type());
		drawContours(dst, contours,biggest, Scalar(255), CV_FILLED);

		return dst;
	}

	//Top three contour >>>that is head,right and left hand...this is useful if the system
	Mat greatestThreeContour(Mat sourceMat)
	{
		int thresh = 100;
		int max_thresh = 255;
		
		Mat imgSrcGrayScale = sourceMat.clone();
		blur(imgSrcGrayScale,imgSrcGrayScale,Size(3,3));

		Mat threshold_output;
		
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		/// Detect edges using Threshold
		//threshold_output = cannyEdgeDetection(imgSrcGrayScale,5);
		threshold(imgSrcGrayScale, threshold_output, thresh,255, THRESH_BINARY);

		findContours(threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_CODE, Point(0, 0));

		Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC1);

		int indexOfBiggestContour=0;
		int indexOfBiggerContour=0;
		int indexOfBigContour=0;

		int sizeOfBiggestContour=0;
		int sizeOfBiggerContour=0;
		int sizeOfBigContour=0;

		for (int i = 0; i < contours.size(); i++)
		{
			double distance=0;
			for(int j=0; j<contours[i].size(); j++)
			{
				Point FirstPt = contours[i][j];
				Point SecondPt = contours[i][((j+1)%contours[i].size())];
				
				distance = distance + distanceBetweenPts(FirstPt,SecondPt);
			}	
			
			if(distance > indexOfBiggestContour)
			{
				sizeOfBigContour = sizeOfBiggerContour;
				sizeOfBiggerContour = sizeOfBiggestContour;
				sizeOfBiggestContour = distance;

				indexOfBigContour = indexOfBiggerContour;
				indexOfBiggerContour = indexOfBiggestContour;
			    indexOfBiggestContour = i;
			}
			else if(distance > indexOfBiggerContour)
			{
				sizeOfBigContour = sizeOfBiggerContour;
				sizeOfBiggerContour = distance;

				indexOfBigContour = indexOfBiggerContour;
				indexOfBiggerContour = i;
			}
			else if(distance > indexOfBigContour)
			{
				sizeOfBigContour = distance;

				indexOfBigContour = i;
			}
			else{}
		}

		drawContours( drawing, contours, indexOfBiggestContour, Scalar(255),-1, 8, vector<Vec4i>(), 0, Point());
		drawContours( drawing, contours, indexOfBiggerContour, Scalar(255),-1, 8, vector<Vec4i>(), 0, Point());
		drawContours( drawing, contours, indexOfBigContour, Scalar(255),-1, 8, vector<Vec4i>(), 0, Point());

		cout<<"\nbig:"<<indexOfBigContour<<"bigger:"<<indexOfBiggerContour<<"biggest:"<<indexOfBiggestContour;
		return drawing;
	}

	//contrast Adjustment and brightness adjustment <my fuzzy logic> -- not the best but does a bit of the job
	void getHistogramInfo(Mat imgSourceMat, double &contrastValue,int &brightnessValue)
	{
		double minValue,maxValue,density;
		double highLevelPixel;

		cv::Point minBin,maxBin;

		Mat result;
		int histSize = 20; //from 0 to 20 >> value 5

		/// Set the ranges ( for B,G,R) )
		float range[] = { 0, 256 } ; //the upper boundary is exclusive
		const float* histRange = { range };
		bool uniform = true; bool accumulate = false;

		calcHist(&imgSourceMat,1,0,Mat(),result,1,&histSize,&histRange,uniform,accumulate);
		minMaxLoc(result,&minValue,&maxValue,&minBin,&maxBin);

		//mid Value for the system 
		highLevelPixel = (result.at<float>(0,0) + result.at<float>(1,0)+result.at<float>(2,0)+result.at<float>(3,0)+result.at<float>(4,0)+result.at<float>(19,0) + result.at<float>(18,0)+result.at<float>(17,0) + result.at<float>(16,0)+ result.at<float>(15,0));
		density = (double)(highLevelPixel/totalPixel);

		density = (( 0.1 - (density)) + 1 );
		//cout<<density<<"\n";

		//17-MAXBIN.... my fuzzy logic for brightness adjustment
		brightnessValue = (int)((15-maxBin.y)*2);
		contrastValue = density;

		//cout<<"Min:"<<minValue<<" max:"<<maxValue<<"minBin:"<<minBin.y<<"maxBin:"<<maxBin.y<<"Constrast:"<<midValue1<<"brightness:"<<brightnessValue<<"\n";
	}

	// Black and White Skin Detection
	Mat blackWhiteImageOfSkin(Mat skinMatrix)
	{
		Mat blackWhiteImage;

		blur(skinMatrix,skinMatrix,Size(3,3));

		inRange(skinMatrix, Scalar(0, 38,50), Scalar(25, 173, 255), blackWhiteImage);
		//inRange(skinMatrix, Scalar(0, 30,80,0), Scalar(20, 150, 200,0), blackWhiteImage);
		//inRange(skinMatrix, Scalar(0, 10, 60), Scalar(20, 150, 255),blackWhiteImage);

		return (blackWhiteImage);
	}

	//Min x point from a set of points
	Point minValuedXPoint(vector<vector<Point>> hull,int contourSize,int hullIndex)
	{
		static Point myPoint;
		static int count=0;

		static pointWrapper myPointerWrap[5];

		if(count<5)
		{
			Point low = hull[hullIndex][0];
		
			for(int i=1;i< hull[hullIndex].size();i++)
			{
				Point temp = hull[hullIndex][i];

				if(temp.x < low.x)
					low = temp;
			}

			myPointerWrap[count].pointValue = low;
			myPointerWrap[count].sortValues = low.x + low.y;
			count++;
		}
		else
		{
			count=0;//sort to remove old values
			myPoint = myPointerWrap[2].pointValue;
		}
		return myPoint;
	}

	//Distance between two points
	double distanceBetweenPts(Point f,Point l)
	{
		int x1 = f.x;
		int y1 = f.y;

		int x2 = l.x;
		int y2 = l.y;

		return (pow(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)),0.5));
	}

	//Convex Hull Detection <<>>
	Mat convexHullDetection(Mat imgSrcGrayScaleOrg)
	{
		//static mat
		/// Draw contours + hull results
		static Mat drawing1 = Mat::zeros(imgSrcGrayScaleOrg.size(), CV_8UC1);
		static Mat drawing2 = Mat::zeros(imgSrcGrayScaleOrg.size(), CV_8UC1);
		static int count=0;

		int thresh = 100;
		int max_thresh = 255;
		
		Mat imgSrcGrayScale = imgSrcGrayScaleOrg.clone();
		blur(imgSrcGrayScale,imgSrcGrayScale,Size(3,3));

		Mat threshold_output;
		
		std::vector<vector<cv::Point>> contours;
		vector<Vec4i> hierarchy;

		/// Detect edges using Threshold
		//threshold_output = cannyEdgeDetection(imgSrcGrayScale,5);
		threshold(imgSrcGrayScale, threshold_output, thresh,255, THRESH_BINARY);

		/// Find contours
		findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		/// Find the convex hull object for each contour
		vector<vector<Point>>hull(contours.size());
		
		//moments for the hand
		/// Get the moments
		vector<Moments> mu(contours.size());
		
		///  Get the mass centers:
		vector<Point2f> mc( contours.size() );

		//Distance value so as to see which has highest perimeter in the hull polygon
		double perimeterHigh=0,perimeterHigher=0;
		int indexHigh=0,indexHigher=0;

		for( int i = 0; i < contours.size(); i++ )
		{  
			convexHull( Mat(contours[i]), hull[i],false,true); 

			//moment unit
			mu[i] = moments( contours[i], false );
			//mass center
			mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

			if(hull[i].size() >7 && hull[i].size() <20 )
			{
				double distance=0;
				for(int j=0; j<hull[i].size(); j++)
				{
					Point FirstPt = hull[i][j];
					Point SecondPt = hull[i][((j+1)%hull[i].size())];
				
					distance = distance + distanceBetweenPts(FirstPt,SecondPt);
				}
				
				if(perimeterHigher<distance)
				{
					perimeterHigh=perimeterHigher;
					perimeterHigher=distance;

					indexHigh = indexHigher;
					indexHigher = i;
				}
				else if(perimeterHigh<distance)
				{
					perimeterHigh=distance;
					indexHigh = i;
				}
				else{}
			}
		}

		if(count==0 && contours.size()!=0)
		{
			drawing1 = Mat::zeros(imgSrcGrayScaleOrg.size(), CV_8UC1);
			drawContours( drawing1, contours, indexHigher, Scalar(255),-1, 8, vector<Vec4i>(), 0, Point());
			drawContours( drawing1, hull, indexHigher, Scalar(200), 1, 8, vector<Vec4i>(), 0, Point());

			//Convex Defect
			std::vector<cv::Vec4i> defect;
			std::vector<int> convexHull_IntIdx; 

			convexHull(contours[indexHigher], convexHull_IntIdx, true); 
			convexityDefects(contours[indexHigher],convexHull_IntIdx,defect);
			
			circle(drawing1,Point(mc[indexHigher].x,mc[indexHigher].y),10,Scalar(150),-1,8,0);

			count++;

			return drawing2;
		}
		
		if(count==1 && contours.size()!=0)
		{
			
			drawing2 = Mat::zeros(imgSrcGrayScaleOrg.size(), CV_8UC1);
			drawContours( drawing2, contours, indexHigher, Scalar(255),-1, 8, vector<Vec4i>(), 0, Point());
			drawContours( drawing2, hull, indexHigher, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point());

			circle(drawing1,Point(mc[indexHigher].x,mc[indexHigher].y),10,Scalar(150),-1,8,0);
			count=0;

			return drawing1;
		}
		return (Mat::zeros(imgSrcGrayScaleOrg.size(), CV_8UC1));
	}

	//This is used to find the avarage point for a certain value
	Point averagePointValue(Point midValue,int maxAverage)
	{
		static int count=0;
		static Point arrayValue[5];
		static Point average(0,0);

		if(count<maxAverage)
		{
			arrayValue[count] = midValue;
			count++;
		}
		else
		{
			for(int j=0;j<5;j++)
			{
				average = average + arrayValue[j];
			}
			average = Point((average.x/maxAverage),(average.y/maxAverage));
			count=0;
		}
		return average;
	}

	//changes the contrast and brightness
	Mat contrastBrightnessAdjusted(Mat imgSourceMat,double contrastValue, int brightnessValue)
	{
		/// Read image given by user
		Mat image = imgSourceMat;
		Mat new_image = Mat::zeros( image.size(), image.type() );

		/// Do the operation new_image(i,j) = alpha*image(i,j) + beta
		for( int y = 0; y < image.rows; y++ )
		{ 
			for( int x = 0; x < image.cols; x++ )
			{ 
				for( int c = 0; c < 3; c++ )
				{
					new_image.at<Vec3b>(y,x)[c] =saturate_cast<uchar>( contrastValue*( image.at<Vec3b>(y,x)[c] ) + brightnessValue );
				}
			}
		}
		return new_image;
	}

	//gets the initalBackground
	//background subtraction Object) -bg
	void initialBackground(BackgroundSubtractorMOG2 &bg)
	{
		Mat source,flippedSource;
		
		Mat myFore = Mat::zeros(source.size(),CV_8UC1);
		std::vector<std::vector<cv::Point> > contours;
		
		for(int i=0;i<50;i++)
		{
			capture->retrieve(source);

			if(isFlipImage)
				flip(source,flippedSource,1);
			else
				flippedSource=source;

			bg.operator ()(flippedSource,foreground);
 			bg.getBackgroundImage(background);

			erode(foreground,foreground,cv::Mat(),Point(-1,-1));
			dilate(foreground,foreground,cv::Mat(),Point(-1,-1));
		}	
	}	

	//get the current background 
	Mat currentBackground(BackgroundSubtractorMOG2 &bg,Mat source)
	{
		Mat myFore = Mat::zeros(source.size(),CV_8UC1);
		std::vector<std::vector<cv::Point> > contours;

        bg.operator ()(source,foreground,0.0001);
        bg.getBackgroundImage(background);

        erode(foreground,foreground,Mat(),Point(-1,-1));
        dilate(foreground,foreground,Mat(),Point(-1,-1));

        findContours(foreground,contours,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		drawContours(myFore,contours,-1,cv::Scalar(255),-1);

		return myFore;
	}

	//Relative mouse tracker
	Point mouseMotionDetection(Point referencePoints1,Point referencePoints2,Point referenceMidPoints, Point currentPoints, double sensitivity = 8)
	{
		//If the refernce mid point is with the bound then movement is not required
		if(currentPoints.x > referencePoints1.x && currentPoints.y > referencePoints1.y && currentPoints.x < referencePoints2.x && currentPoints.y < referencePoints2.y)
		{
			return Point(0,0);
		}
		else
		{
			int dx=0,dy=0;
			Point disDir = referenceMidPoints - currentPoints;

			if(disDir.x <0)
			{
				dx = currentPoints.x - referencePoints1.x;
			}
			else
			{
				dx = currentPoints.x -  referencePoints2.x;
			}

			if(disDir.y <0)
			{
				dy = currentPoints.y - referencePoints1.y;
			}
			else
			{
				dy = currentPoints.y - referencePoints2.y;
			}

			dx= (int)((double)dx/sensitivity);
			dy= (int)((double)dy/sensitivity);

			return Point(dx,dy);

		}
	}

	//Comman HSV & GrayScale
	void convertImageSystem(Mat &imgDestMat,Mat &imgDestMatGrayScale,Mat &imgDestMatHSV)
	{
		//gray scal convertion......
		cvtColor(imgDestMat,imgDestMatGrayScale,CV_RGB2GRAY);
		//SKIN Detection beta version => bINARY iMAGE
		cvtColor(imgDestMat,imgDestMatHSV,CV_BGR2HSV);

	}

	//Contrast and brightness system
	Mat convertImageContrastBrightness(Mat imgDestMat)
	{
			double contrastValue=1.0;
			int brightnessValue=0;

			//Apply Histogram Equalization
			getHistogramInfo(imgDestMatGrayScale,contrastValue,brightnessValue);
		
			//contrast and brightness adjustment for cols 
			imgDestMat = contrastBrightnessAdjusted(imgDestMat,contrastValue,brightnessValue);

			return imgDestMat;
	}

	//Get Skin detection with error correction 
	Mat getImageSkinDetection(Mat imgDestMatHSV)
	{
		imgDestMatHSV = blackWhiteImageOfSkin(imgDestMatHSV);

		dilate(imgDestMatHSV,imgDestMatHSV,Mat(),Point(-1,-1),5);
		erode(imgDestMatHSV,imgDestMatHSV,Mat(),Point(-1,-1),5);
		dilate(imgDestMatHSV,imgDestMatHSV,Mat(),Point(-1,-1),2);
			
		imgDestMatHSV = imgDestMatHSV | shiftMatrixPositive(imgDestMatHSV,5,0) | shiftMatrixPositive(imgDestMatHSV,0,5) | shiftMatrixNegative(imgDestMatHSV,5,0) | shiftMatrixNegative(imgDestMatHSV,0,5);

		return imgDestMatHSV;
	}

	//Get Edge of object with enhance Edge Detection
	Mat getImageEdgeEnhance(Mat imgDestMatGrayScale)
	{
		imgDestMatEdge =  cannyEdgeDetection(imgDestMatGrayScale,9);
		dilate(imgDestMatEdge,imgDestMatEdge,Mat(),Point(-1,-1));
		erode(imgDestMatEdge,imgDestMatEdge,Mat(),Point(-1,-1));
		
		return imgDestMatEdge;
	}

	//main get video feed function
	int getVideoFeed(int escapeKey=27)
	{
		mousePosition = new eventTrigger(screenHeight,screenWidth,cameraHeight,cameraWidth);

		//background subtractor protocal
		BackgroundSubtractorMOG2 bg(30,6,false);

		//face detection so that i can eliminate that better caculation
		Point midPointFace(0,0),lowPointFace(0,0),highPointFace(0,0);
		Point refFacePt1(0,0),refFacePt2(0,0);

		int isFaceFound=0;
		Size detectedFace(0,0),camshiftDetectedFace(0,0);
		bool camShiftPrimaryDetectionFace = false;
		
		////found variables
		int isFoundFist=0,isFoundPalm=0;
		int isFistFirst=1,isPalmFirst=1;
		int fistNotDetectedCount=0;

		////ideal hand count is to check the ideal nature of the hand
		bool camShiftPrimaryDetectionFist = false;

		//fist find and palm
		Point pt1,pt2;//specifies the points of the rectangle
		Point midPointPalm,midPointHand,midPointFist; // the midpoint values for the various object detected
		Point minPointPalm,maxPointPalm; //low and upper bound of fist detection
		Point minHandPt1,minHandPt2; //low and upper bound for hand
		
		Size detectedFist(0,0),camshiftDetectedFist(0,0);

		//Reference Pointer for the mouse
		Point mousePointerReference(0,0);
		Point referencePalmPt1(0,0);
		Point referencePalmPt2(0,0);

		//time controls for events
		time_t currentTime = time(0);

		//duration for hand detection and fist detection
		time_t handTime = time(0);
		time_t handClickTime = time(0);

		double handElapseTime = 0;

		Point recordedHandPoint1,recordedHandPoint2;

		//mat for mask hadn
		Mat handMask = Mat::zeros(imgDestMat.size(),CV_8UC1);

		/*
			This is a simple function to detect a background
		*/
		cout<<"\nBackground detection active....look at the monitor and stay still";
		initialBackground(bg);
		Mat foregroundMask = Mat::zeros(imgDestMat.size(),CV_8UC1);

		// Show the image captured from the camera in the window and repeat
		while (runCamFeed)
		{
			// Get one frame
			capture->retrieve(imgSrcMat);

			if(isFlipImage)
				flip(imgSrcMat,imgSrcMat,1);
		
			imgDestMat=imgSrcMat;

			//Get Orginal HSV and Org. GrayScale for the image
			convertImageSystem(imgDestMat,imgDestMatGrayScale,imgDestMatHSV);
			
			//Adjust brightness and contrast based on the histogram 
			imgDestMat = convertImageContrastBrightness(imgDestMat);
			
			//foreground & background extraction
			foregroundMask = currentBackground(bg,imgSrcMat);
			imgDestMatGrayScale.copyTo(imgSrcMat,foregroundMask);	
			
			//the new edge detected
			imgDestMatEdge = getImageEdgeEnhance(imgDestMatGrayScale);

			////skin detection ....the function need a hsv based image
			imgDestMatHSV = getImageSkinDetection(imgDestMatHSV);		

			//Find the edge and hsv mask matrix <<THis is more effective that grayscale tracking in cmashift
			Mat imgDestMatBoundary;
			imgDestMatEdge.copyTo(imgDestMatBoundary,imgDestMatHSV);

			//Face detection 
			if(!isFaceFound)
			{
				midPointFace = detectFaceMask(imgDestMatGrayScale,lowPointFace,highPointFace,isFaceFound);
				rectangle(imgDestMat,lowPointFace,highPointFace,Scalar(0,255,255),1,8,0);

				camShiftPrimaryDetectionFace = false;
			}
			else
			{
				static Point refFacePt1,refFacePt2;
				int detectedHeight,detectedWidth;
				static Point distanceFacePts(0,0);

				try
				{
					if(camShiftPrimaryDetectionFist==false)
					{
						refFacePt1 = lowPointFace;
						refFacePt2 = highPointFace;
						camShiftPrimaryDetectionFace=true;

						detectedHeight = highPointFace.y - lowPointFace.y;
						detectedWidth  = highPointFace.x - lowPointFace.x;
					}	
					else
					{
						detectedHeight = highPointFace.y - lowPointFace.y;
						detectedWidth = highPointFace.x - lowPointFace.x;
					}

					Rect faceMaskRect(refFacePt1,refFacePt2);

					TermCriteria theTerm = TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10,1);
					RotatedRect trackBox =  CamShift(imgDestMatBoundary,faceMaskRect,theTerm);
				
					midPointFace = trackBox.center;
					camshiftDetectedFace = trackBox.size;

					refFacePt1 = Point(midPointFace.x - detectedWidth/2 ,midPointFace.y - detectedHeight/2);
					refFacePt2 = Point(refFacePt1.x + detectedWidth,refFacePt1.y + detectedHeight);

					rectangle(imgDestMat,refFacePt1,refFacePt2,Scalar(23,12,40),1,8,0);//flood fill the face with black

					if((((double)detectedFace.area() * 0.90 > (double)camshiftDetectedFace.area()) && ((double)detectedFace.area() * 1.10 < (double)camshiftDetectedFace.area())))
					{
						isFoundFist=0; 
						camShiftPrimaryDetectionFist=false;
						cout<<"\nSize inadequate fist";
					}
				}
				catch(...)
				{
					isFaceFound=0;
				}
			}

			////////if i find a palm then no need to check for fist
			//Palm detection <<why it very accurate compared to whole hand
			midPointPalm = detectPalmMask(imgDestMat,minPointPalm,maxPointPalm,isFoundPalm);

			//fist finder for mouse tracker>> 
			if(isFoundFist==0 && isFoundPalm==0)
			{
				//Find the midpoint of the fist
				midPointFist = detectFistMask(imgDestMatGrayScale,pt1,pt2,isFoundFist);
				
				detectedFist.height = pt2.y-pt1.y;
				detectedFist.width = pt2.x-pt1.x;

				if(midPointFist.x > lowPointFace.x && midPointFist.y > lowPointFace.y && midPointFist.x < highPointFace.x && midPointFist.y < highPointFace.y )
				{
					isFoundFist=0;
				}

				camShiftPrimaryDetectionFist = false;
			}
			else
			{
				static Point refPt1,refPt2;
				int detectedHeight,detectedWidth;
				static Point distancePts(0,0);

				try
				{
					if(camShiftPrimaryDetectionFist==false)
					{
						refPt1 = pt1;
						refPt2 = pt2;
						camShiftPrimaryDetectionFist=true;

						distancePts = pt2 - pt1;
						distancePts = Point(distancePts.x/8,distancePts.y/8);

						detectedHeight = pt2.y - pt1.y;
						detectedWidth = pt2.x - pt1.x;
					}	
					else
					{
						detectedHeight = pt2.y - pt1.y;
						detectedWidth = pt2.x - pt1.x;
					}

					Rect palmMaskRect(refPt1,refPt2);
					Rect regionOfInterest(pt1 - distancePts,pt2 + distancePts);

					TermCriteria theTerm = TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10,1);
					RotatedRect trackBox =  CamShift(imgDestMatBoundary,palmMaskRect,theTerm);
				
					midPointFist = trackBox.center;
					camshiftDetectedFist = trackBox.size;

					refPt1 = Point(midPointFist.x - detectedWidth/2,midPointFist.y - detectedHeight/2);
					refPt2 = Point(refPt1.x + detectedWidth,refPt1.y + detectedHeight);

					//cout<<"\n refPt x="<<refPt2.x<<" refPt y="<<refPt2.y<<" width="<<detectedWidth<<" height="<<detectedHeight;
					//fist is not a fist <<to small or big for fist
					if((((double)detectedFist.area() * 0.90 > (double)camshiftDetectedFist.area()) && ((double)detectedFist.area() * 1.10 < (double)camshiftDetectedFist.area())))
					{
						isFoundFist=0; 
						camShiftPrimaryDetectionFist=false;
						cout<<"\nSize inadequate fist";
					}
					
					//fist is not in fist area
					if(!(((pt1.x - distancePts.x)<=midPointFist.x)&&((pt1.y - distancePts.y)<=midPointFist.y)&& ((pt2.x + distancePts.x)>=midPointFist.x)&& ((pt2.y + distancePts.y)>=midPointFist.y)))
					{
						isFoundFist=0;
						camShiftPrimaryDetectionFist=false;
						cout<<"\nDistance inadequate fist";
					}

					//if the tracker gets a value outside the range
					if(midPointFist.x > lowPointFace.x && midPointFist.y > lowPointFace.y && midPointFist.x < highPointFace.x && midPointFist.y < highPointFace.y )
					{
						isFoundFist=0;
					}

					rectangle(imgDestMat,pt1,pt2,Scalar(255,0,0),1,8,0);
					rectangle(imgDestMat,refPt1,refPt2,Scalar(0,255,0),1,8,0);
					rectangle(imgDestMat,pt1-distancePts,pt2+distancePts,Scalar(0,0,255),1,8,0);

				}
				catch(...)
				{
					isFoundFist=0;
					camShiftPrimaryDetectionFist=false;
				}
			}

			//hand and fist for click operation and complex operation
			midPointHand = detectHandMask(imgDestMatGrayScale,pt1,pt2,minHandPt1,minHandPt2);

			//get currentTime
			currentTime = time(0);

			//get mask for advance gesture like finger click
			if((minHandPt1 != recordedHandPoint1) && (minHandPt2 != recordedHandPoint2))
			{
				recordedHandPoint1 = minHandPt1;
				recordedHandPoint2 = minHandPt2;

				handMask = Mat::zeros(imgDestMat.size(),CV_8UC1);
				rectangle(handMask,recordedHandPoint1,recordedHandPoint2,Scalar(255),-1);
			
				handTime = time(0);
			}

			//cout<<"\nx:"<<midPointFist.x<<"y:"<<midPointFist.y;
			//cout<<"\ndiffTime:"<<difftime(handTime,currentTime);

			if(difftime(handTime,currentTime)==-8)
			{
				handMask = Mat::zeros(imgDestMat.size(),CV_8UC1);				
			}

			//the below is the code which keeps a reference frame if the palm is detected for the first time, Else 
			//calculates the difference in
			if(isFoundFist)
			{
				if(isFistFirst)
				{
					mousePointerReference = midPointFist;
					Point dist = pt2 - pt1;
					referencePalmPt1 = pt1 + (Point(dist.x/4,dist.y/4));
					referencePalmPt2 = pt2 - (Point(dist.x/4,dist.y/4));

					isFistFirst=0;
					cout<<"\nMouse Movement online....";
				}
				else
				{
					rectangle(imgDestMat,pt1,pt2,Scalar(255,255,255),1,8,0);
					rectangle(imgDestMat,referencePalmPt1,referencePalmPt2,Scalar(255,0,0),1,8,0);

					fistNotDetectedCount=0;

					Point referenceDisplacement = mouseMotionDetection(referencePalmPt1,referencePalmPt2,mousePointerReference,midPointFist,18);

					//testing for coordinates
					//cout<<"\nx:"<<referenceDisplacement.x<<"y:"<<referenceDisplacement.y;
					
					if( referenceDisplacement.x + referenceDisplacement.y !=0)
					{
						mousePosition->setmousePosition(referenceDisplacement.x,referenceDisplacement.y);
					}

				}
				isPalmFirst=1;
			}
			else
			{
				fistNotDetectedCount++;

				if(fistNotDetectedCount>=6)
				{
					isFistFirst=1;
				}
			}

			//Palm found for click basic operation
			if(isFoundPalm && isPalmFirst)
			{
				if(isPalmFirst>=1)
				{
					isPalmFirst++;
					isFoundFist=0;

					//reduces the chance of fail positive <<
					if(isPalmFirst==3)
					{
						mousePosition->setMouseLeftClick();
						cout<<"\nMouse Single left click....";
						isPalmFirst=0;

					}
				}
			}
			
			//FINAL USER WINDOW FOR IDENTIFICATION
			namedWindow("Main",CV_WINDOW_AUTOSIZE);
			imshow("Main",imgDestMat);

			//testing of the System Point
			//cout<<"\nPalm-1 x:"<<pt1.x<<" Palm-1 y:"<<pt1.y<<"\nPalm-2 x:"<<pt2.x<<" Palm-2 y:"<<pt2.y<<" Palm midx:"<<midPointPalm.x<<" Palm midy:"<<midPointPalm.y;
			//cout<<"\nHand-1 x:"<<minHandPt1.x<<" Hand-1 y:"<<minHandPt1.y<<"\nHand-2 x:"<<minHandPt1.x<<" Hand-2 y:"<<minHandPt1.y<<" Hand midx:"<<midPointHand.x<<" Hand midy:"<<midPointHand.y;
			//cout<<"\nPalm x:"<<midPointPalm.x<<" Palm y:"<<midPointPalm.y;


			//imgDestMat.copyTo(imgDestMatEdge,fistMask);
			imgDestMatHSV.copyTo(imgDestMat,handMask);
			
			//threshold(imgDestMatEdge,imgDestMatEdge,200,255,THRESH_BINARY);
			//dilate(imgDestMatEdge,imgDestMatEdge,Mat(),Point(-1,-1),3);
			//imgDestMatEdge.copyTo(imgDestMat,imgDestMatHSV);

			//imgDestMatEdge.copyTo(imgDestMat,imgDestMatHSV);
			//imgDestMatHSV = greatestThreeContour(imgDestMatHSV);
			//the outline basic using xor
			//Mat imgDestMatOutline;

			//get the final image if possible
			//Mat imgDestMatCon = convexHullDetectionInveredNoiseRemover(imgDestMatHSV);
			//imgDestMatGrayScale.copyTo(imgDestMat,(imgDestMatHSV&imgDestMatEdge)); //  imgDestMatEdge
			
			imgDestMat = contourAdjustment(imgDestMat);

			Mat imgDestMatCon = convexHullDetection(imgDestMat);

			if (!capture->isOpened())
			{
				fprintf( stderr, "\nERROR: Can't get a valid frame to process...\n" );
				getchar();
				return 0;
			}

			//// Create a window in which the captured images will be presented....this is for testing of the code...
			/*
			namedWindow("HSV",CV_WINDOW_AUTOSIZE);
			imshow("HSV",imgDestMatHSV);
			
			namedWindow("EDGE",CV_WINDOW_AUTOSIZE);
			imshow("EDGE",imgDestMatEdge);
			
			namedWindow("MAT",CV_WINDOW_AUTOSIZE);
			imshow("MAT",imgDestMat);

			namedWindow("CON",CV_WINDOW_AUTOSIZE);
			imshow("CON",imgDestMatCon);
			*/

			// Do not release the frame!
			//If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
			//remove higher bits using AND operator
			if ((cvWaitKey(10) & 255) == escapeKey) 
			{
				break;
			}
		}
		
		return 1;
	}
	
	//to map the hand to the screen
	void calculateConvertionRatio()
	{
		cout<<"\nRecalculate the convertion ratio";
		capture->retrieve(imgSrcMat);

		cameraHeight= imgSrcMat.size().height;
		cameraWidth = imgSrcMat.size().width;

		yRatio = (double)(screenHeight/cameraHeight);
		xRatio = (double)(screenWidth/cameraWidth);

		cout<<"\nScreen Height:"<<screenHeight<<"\nScreen Width"<<screenWidth<<"\nCamera Height:"<<cameraHeight<<"\nCamera Width:"<<cameraWidth<<"\nHeight Factor:"<<yRatio<<"\nWidth Factor:"<<xRatio;
	}

	//device capture mod
	bool captureVideoFeed()
	{
		capture = new VideoCapture(CV_CAP_ANY);

		if (!capture->isOpened() )
		{
			fprintf( stderr, "\nERROR: Can't Get a camera video feed. Please Check the camera and the required software \n" );
			getchar();
			runCamFeed = false;
			return false;
		}

		calculateConvertionRatio();

		runCamFeed=true;
		return true;
	}

public :

	cameraSoftware()
	{
		isFlipImage=true;

		calculateScreenResolution();
	}

	bool camerasSetting(bool isFlip)
	{
		isFlipImage=isFlip;
		calculateScreenResolution();
	}

	/*Capture camera feedback from any available camera*/
	bool captureVideoFeedThread()
	{
		return captureVideoFeed();
	}

	/* the producer for the images*/
	void getVideoFeedThread()
	{
		cout<<"\nVideo Feed Thread Online...";
		getVideoFeed();
	}

	/*Remove all captured hardware resources*/
	void destroyTakenResource()
	{
		cout<<"\nThis is Galactus, Destroyer of World!!!...\nTerminating the captured hardware resources.";
		// Release the capture device housekeeping
		delete(capture);

		return ;
	}

};


void handGestureSystem()
{
	//I dont want the console unless debugging is needed.
	//windowHideConsole();

	/* Calling GetDC with argument 0 retrieves the desktop's DC */
    HDC hDC_Desktop = GetDC(0);

    /* Draw a simple blue rectangle on the desktop */
    //RECT rect = { 20, 20, 200, 200 };
    //HBRUSH blueBrush=CreateSolidBrush(RGB(0,0,255));
    //FillRect(hDC_Desktop, &rect, blueBrush);

	//creates the cameraSoftware Object which will account for the actually working code
	cameraSoftware myCamera;
	
	//Starts the camera Feed from the default webcamera available on the system.
	myCamera.captureVideoFeedThread();
	
	/*The producer thread will generate the required image feed for the consumers to process
	  thanks a lot to the boost thread libraries.....K*/
	boost::thread myProducer(&cameraSoftware::getVideoFeedThread,&myCamera);

	//Welcome to the force ----Jedi ---> This will join the producer thread to the thread pool
	myProducer.join();

	//kills the complete object
	myCamera.destroyTakenResource();
	myProducer.detach();

	return;
}


int _tmain(int argc, _TCHAR* argv[])
{
	handGestureSystem();
    return 0;
}