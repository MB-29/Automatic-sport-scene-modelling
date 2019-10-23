#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat I1 = imread("../IMG_0045.JPG", IMREAD_GRAYSCALE);
	Mat I2 = imread("../IMG_0046.JPG", IMREAD_GRAYSCALE);

	imshow("I1", I1);
	imshow("I2", I2);

	Ptr<AKAZE> D = AKAZE::create();
	Mat descriptor_1, descriptor_2;
	vector<KeyPoint> m1, m2;
	D->detectAndCompute(I1, noArray(), m1, descriptor_1);
	D->detectAndCompute(I2, noArray(), m2, descriptor_2);
	Mat J1, J2;
	drawKeypoints(I1, m1, J1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("J1", J1);
	drawKeypoints(I1, m2, J2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("J2", J1);

	int k = 1;
	BFMatcher M(NORM_HAMMING);
	vector<vector<DMatch>> matches_vector;
	M.knnMatch(descriptor_1, descriptor_2, matches_vector, k);

	Mat matches_output;
	drawMatches(I1, m1, I2, m2, matches_vector, matches_output);
	imshow("matches", matches_output);

	vector<Point2f> positions_1, positions_2;
	for(size_t index = 0; index < matches_vector.size(); index++){
		DMatch match = matches_vector[index][0];
		positions_1.push_back(m1[match.queryIdx].pt);
		positions_2.push_back(m2[match.trainIdx].pt);
	}
	Mat H = findHomography(positions_2, positions_1, RANSAC);

	Mat K(I1.rows, 2 * I1.cols, CV_8U);
	cout << "good H " << H << endl;
	cout << "I1.size " << I1.size() << endl;
	cout << "I2.size " << I2.size() << endl;
	warpPerspective(I2, K, H , K.size());
	cout << "K.size " << K.size() << endl;
	for (size_t i = 0; i < I1.rows; i++) {
        for (size_t j = 0; j < I1.cols; j++) {
            K.at<uchar>(i, j) = I1.at<uchar>(i, j);
        }
	}
	imshow("K", K);

	waitKey(0);
	return 0;
}
