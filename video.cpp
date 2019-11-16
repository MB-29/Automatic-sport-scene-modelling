#include "video.h"

int open()
{	// List of tracker types in OpenCV 3.4.1



	VideoCapture video("../input/videos/ShortBasket.mp4");

	// Check if camera opened successfully
	if (!video.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	};
	int frame_index = 0;
	Mat frame;
	video >> frame;
	Rect2d window = selectROI("tracker",frame);	

	Ptr<TrackerKCF> tracker = TrackerKCF::create();
	tracker->init(frame, window);

	// NamedWindow("Frame", WINDOW_AUTOSIZE);
	bool ok = true;


		while (1) {

			frame_index += 1;

			// Capture frame-by-frame
			video.read(frame);

			// If the frame is empty, break immediately
			if (frame.empty()) {
				cout << "problem" << endl;
				break;
			}
			tracker->update(frame, window);
			rectangle( frame, window, Scalar( 255, 0, 0 ), 2, 1 );

			// Display the resulting frame
			imshow("Frame ", frame);

			if(waitKey(1)==27)break;


		}


	// When everything done, release the video capture object
	video.release();
	return 0;

	// Closes all the frames
	//destroyAllWindows();
}

int main(){
	open();
}