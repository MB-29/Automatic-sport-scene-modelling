#include "homographie.h"

int main()
{
	Matches matches;
	Image<Vec3b> source_image = Image<Vec3b>(imread("../input/images/photo.jpg"));
	Image<Vec3b> target_image = Image<Vec3b>(imread("../input/images/above.jpg"));
	matches.source_image = source_image;
	matches.target_image = target_image;
	imshow("source", source_image);
	imshow("target", target_image);
	setMouseCallback("source", add_point_source, &matches);
	setMouseCallback("target", add_point_target, &matches);

	waitKey();
	cout << "Building homography" << endl;
	Mat homography = findHomography(matches.source_points, matches.target_points);

	Homography_transformation input;
	input.source_image = source_image;
	input.target_image = target_image;
	input.homography_matrix = homography;
	setMouseCallback("source", draw_homographic_pair, &input);

	waitKey();


	// open();


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