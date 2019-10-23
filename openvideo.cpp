#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "image.h"

using namespace std;
using namespace cv;

int main()
{
	VideoCapture cap("../input/videos/ShortBasket.wmv");

	 // Check if camera opened successfully
	  if(!cap.isOpened()){
		cout << "Error opening video stream or file" << endl;
		return -1;
	  }
 
		Mat frame;
		// Capture frame-by-frame
		cap >> frame;
  
		// If the frame is empty, break immediately
		//if (frame.empty())
		  //break;
 
		// Display the resulting frame
		imshow( "Frame", frame );
 
		// Press  ESC on keyboard to exit
		//char c=(char)waitKey(25);
		//if(c==27)
		 // break;
  
	// When everything done, release the video capture object
	//cap.release();
 
	// Closes all the frames
	//destroyAllWindows();

	waitKey(0);
	return 0;

}