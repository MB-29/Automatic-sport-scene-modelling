#include "video.h"
#include "image.h"
#include <opencv2/core/utility.hpp>
//#include <opencv2/tracking/tracker.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>




using namespace std;
using namespace cv;


int open()
{	// List of tracker types in OpenCV 3.4.1
	//string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
	//vector <string> trackerTypes(types, std::end(types));

	// Create a tracker
	//string trackerType = trackerTypes[2];

    //Ptr<TrackerKCF> tracker;

	VideoCapture cap("../input/videos/ShortBasket.mp4");

	// Check if camera opened successfully
	if (!cap.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	};
	int i = 0;
	cvNamedWindow("Frame", CV_WINDOW_AUTOSIZE);
	bool ok = true;
	Rect2d bbox(287, 23, 86, 320);
	Mat frame;

	// Capture frame-by-frame
	cap.read(frame);

	// Uncomment the line below to select a different bounding box 
	bbox = selectROI(frame, false);

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
			 
			// Display bounding box. 
			rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);


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

int main(){
	open();
}