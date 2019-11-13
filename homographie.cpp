#include "homographie.h"


void add_point_source(int event, int x, int y, int foo, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	Matches *matches = (Matches *)data;
	int count = matches->source_points.size();
	cout << "x = " << x << ", y = " << y << endl;
	Point point = Point(x, y);
	circle(matches->source_image, point, 2, Scalar(0, 255, 0), 2);
	putText(matches->source_image, to_string(count), point, FONT_HERSHEY_PLAIN, 2, 2);
	imshow("source", matches->source_image);
	matches->source_points.push_back(point);
	cout << "size of source points : " << count << endl;
}
void add_point_target(int event, int x, int y, int foo, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	Matches *matches = (Matches *) data;
	int count = matches->target_points.size();
	cout << "x = " << x << ", y = " << y << endl;
	Point point = Point(x, y);
	circle(matches->target_image, point, 2, Scalar(0, 255, 0), 2);
	putText(matches->target_image, to_string(count), point, FONT_HERSHEY_PLAIN, 2, 2);
	imshow("target", matches->target_image);
	matches->target_points.push_back(point);
	cout << "size of target points : " << count << endl;
}

Point homographic_image(const Mat &homography_matrix, Point input_point){
	Vec3d input_point_3D(input_point.x, input_point.y, 1);
	Mat output = homography_matrix * ((Mat)input_point_3D) ;  
	double u = output.at<double>(0,0);
	double v = output.at<double>(0,1);
	double w = output.at<double>(0,2);
	return Point(u/w, v/w);
}

void draw_homographic_pair(int event, int x, int y, int foo, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	Homography_transformation *input = (Homography_transformation *)data;
	cout << "Applying homography" << endl;

	cout << "drawing a circle" << endl;
	circle(input->source_image, Point(x,y), 2, Scalar(0, 0, 255), 2);
	imshow("source", input->source_image);


	Point target_point = homographic_image(input->homography_matrix, Point(x,y));

	circle(input->target_image, target_point, 2, Scalar(0, 0, 255), 2);
	cout << "target point : " << target_point << endl;
	imshow("target", input->target_image);
}
