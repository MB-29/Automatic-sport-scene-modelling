#include "calibration.h"

// Homographic pairs
void add_point_source(int event, int x, int y, int foo, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	Input *input = (Input *)data;
	int count = input->source_points.size();
	cout << "x = " << x << ", y = " << y << endl;
	Point point = Point(x, y);
	circle(input->source_image, point, 2, Scalar(0, 255, 0), 2);
	putText(input->source_image, to_string(count), point, FONT_HERSHEY_PLAIN, 2, 2);
	imshow("source", input->source_image);
	input->source_points.push_back(point);
	cout << "size of source points : " << count << endl;
}
void add_point_target(int event, int x, int y, int foo, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	Input *input = (Input *)data;
	int count = input->target_points.size();
	cout << "x = " << x << ", y = " << y << endl;
	Point point = Point(x, y);
	circle(input->target_image, point, 2, Scalar(0, 255, 0), 2);
	putText(input->target_image, to_string(count), point, FONT_HERSHEY_PLAIN, 2, 2);
	imshow("target", input->target_image);
	input->target_points.push_back(point);
	cout << "size of target points : " << count << endl;
}

// Homography map
Point homographic_transformation(const Mat &homography_matrix, Point input_point)
{
	Vec3d input_point_3D(input_point.x, input_point.y, 1);
	Mat output = homography_matrix * ((Mat)input_point_3D);
	double u = output.at<double>(0, 0);
	double v = output.at<double>(0, 1);
	double w = output.at<double>(0, 2);
	return Point(u / w, v / w);
}

// Select jersey colours
void select_colour(int event, int x, int y, int foo, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;

	Input *input = (Input *)data;
	Vec3b colour = input->source_image(x, y);
	circle(input->source_image, Point(x, y), 7, (Scalar)colour, 4);
	imshow("source", input->source_image);
	cout << "selected colour : " << input->source_image(x, y) << endl;
	if (waitKey() == 32)
	{
		Vec3b colour_rgb = Vec3b({colour[2], colour[1], colour[0]});
		input->colours.push_back(colour);
		cout << "Added colour : " << input->source_image(x, y) << endl;
	}
}

// Delimit the pitch
void add_pitch_point(int event, int x, int y, int foo, void *data)
{
	Input *input = (Input *)data;

	if (event != EVENT_LBUTTONDOWN || input->pitch_points_count == 4)
		return;
	int count = input->pitch_points_count;
	Point point = Point(x, y);
	circle(input->source_image, point, 2, Scalar(255, 255, 255), 2);
	if (count > 0)
		line(input->source_image, input->pitch[count - 1], point, Scalar(0, 0, 255), 2);
	if (count == 3)
		line(input->source_image, input->pitch[0], point, Scalar(0, 0, 255), 2);
	putText(input->source_image, to_string(count), point, FONT_HERSHEY_PLAIN, 2, 2);
	imshow("source", input->source_image);
	input->pitch[count] = point;
	cout << "Added point : " << input->pitch[count] << endl;
	input->pitch_points_count = count + 1;
}

double norm(Vec3b vector){
	double squared_norm = 0;

	for (int i = 0; i < 3 ; i ++){
		squared_norm += vector[i] * vector[i];
	}
	return sqrt(squared_norm);
}

int get_jersey_colour(Mat &frame, Rect rectangle, Vec3b jersey_colour_1, Vec3b jersey_colour_2)
{
	Image<Vec3b> image(frame);
	double distance_1 = 0, distance_2 = 0;
	for (int y = rectangle.y; y < rectangle.y + rectangle.height; y++)
	{
		for (int x = rectangle.x; x < rectangle.x + rectangle.width; x++)
		{
			Vec3b pixel_colour = image(x, y);
			distance_1 +=  norm(pixel_colour - jersey_colour_1);
			distance_1 +=  norm(pixel_colour - jersey_colour_1);
		}
	}
	if (distance_1 < distance_2) return 0;
	return 1; 
}