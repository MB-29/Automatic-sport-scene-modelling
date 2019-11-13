#include "opencv2/opencv.hpp"
#include <iostream>
#include "video.h"
#include "image.h"
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>




using namespace std;
using namespace cv;


int open()
{	// List of tracker types in OpenCV 3.4.1
	//string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
	// vector <string> trackerTypes(types, std::end(types));

	// Create a tracker
	//string trackerType = trackerTypes[2];

	//Ptr<Tracker> tracker;

	VideoCapture cap("../input/videos/Wildlife.wmv");

	// Check if camera opened successfully
	if (!cap.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	};
	int i = 0;
	cvNamedWindow("Frame", CV_WINDOW_AUTOSIZE);
	bool ok = true;

		while (1) {

			i += 1;
			cout << i << endl;
			Mat frame;

			// Capture frame-by-frame
			cap.read(frame);

			// If the frame is empty, break immediately
			if (frame.empty()) {
				cout << "problem" << endl;
				break;
			}

			// Display the resulting frame
			imshow("Frame", frame);


			// Press  ESC on keyboard to exit
			char c = (char)waitKey(25);
			cout << c << endl;
			if (c == 27) {
				ok = false;
				break;
			}

		}


	// When everything done, release the video capture object
	cap.release();
	return (0);

	// Closes all the frames
	//destroyAllWindows();
}