#include "homographie.h"
#include "video.h"

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
	waitKey(0);
	return 0;
}