#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "image.h"

using namespace std;
using namespace cv;

void onMouse1(int event, int x, int y, int foo, void *data){
	if (event != EVENT_LBUTTONDOWN)
		return;
	cout << "x = " << x << ", y = " << y << endl;
	}

int open()
{
	VideoCapture cap("../input/videos/ShortBasket.wmv");

	// Check if camera opened successfully
	if (!cap.isOpened()) {
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
	imshow("Frame", frame);

	// Press  ESC on keyboard to exit
	//char c=(char)waitKey(25);
	//if(c==27)
	 // break;

// When everything done, release the video capture object
//cap.release();

// Closes all the frames
//destroyAllWindows();
	return(0);

}

int main()
{
	void *data;

	Image<Vec3b> photo_image = Image<Vec3b>(imread("../input/images/photo.jpg"));
	Image<Vec3b> above_image = Image<Vec3b>(imread("../input/images/above.jpg"));

	imshow("above", above_image);
	setMouseCallback("above", onMouse1, &data);
	open();

	vector<Point2f> positions_1, positions_2;
	// for(size_t index = 0; index < matches_vector.size(); index++){
	// 	DMatch match = matches_vector[index][0];
	// 	positions_1.push_back(m1[match.queryIdx].pt);
	// 	positions_2.push_back(m2[match.trainIdx].pt);
	// }
	// Mat H = findHomography(positions_2, positions_1, RANSAC);
	
	// int k = 1;
	// BFMatcher M(NORM_HAMMING);
	// vector<vector<DMatch>> matches_vector;
	// M.knnMatch(descriptor_1, descriptor_2, matches_vector, k);

	// Mat matches_output;
	// drawMatches(I1, m1, I2, m2, matches_vector, matches_output);
	// imshow("matches", matches_output);


	// Mat K(I1.rows, 2 * I1.cols, CV_8U);
	// cout << "good H " << H << endl;
	// cout << "I1.size " << I1.size() << endl;
	// cout << "I2.size " << I2.size() << endl;
	// warpPerspective(I2, K, H , K.size());
	// cout << "K.size " << K.size() << endl;
	// for (size_t i = 0; i < I1.rows; i++) {
    //     for (size_t j = 0; j < I1.cols; j++) {
    //         K.at<uchar>(i, j) = I1.at<uchar>(i, j);
    //     }
	// }
	// imshow("K", K);

	

	waitKey(0);
	return 0;
}
