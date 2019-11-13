#include "video.h"

int open()
{	// List of tracker types in OpenCV 3.4.1



	Ptr<TrackerKCF> tracker = TrackerKCF::create();

	VideoCapture cap("../input/videos/ShortBasket.mp4");

	// Check if camera opened successfully
	if (!cap.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	};
	int i = 0;
	// NamedWindow("Frame", WINDOW_AUTOSIZE);
	bool ok = true;

		while (1) {

			i += 1;
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
	return 0;

	// Closes all the frames
	//destroyAllWindows();
}

int main(){
	open();
}