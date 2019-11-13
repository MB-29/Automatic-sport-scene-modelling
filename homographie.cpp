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

void apply_homography(int event, int x, int y, int foo, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	Homography_transformation *input = (Homography_transformation *)data;
	Vec3d point(x,y, 1);
	cout << "Homography = " << input->homography_matrix << endl;
	cout << "Applying homography" << endl;
	Mat output = input->homography_matrix * ((Mat)point) ;  
	cout << "drawing a circle" << endl;

	circle(input->source_image, Point(x,y), 2, Scalar(0, 0, 255), 2);
	imshow("source", input->source_image);
	
	double u = output.at<double>(0,0);
	double v = output.at<double>(0,1);
	double w = output.at<double>(0,2);
	Point target_point(u/w, v/w);

	circle(input->target_image, target_point, 2, Scalar(0, 0, 255), 2);
	cout << "target point : " << target_point << endl;
	imshow("target", input->target_image);
}

void solve_homography(){

}