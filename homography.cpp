#include "homography.h"

// Homographic pairs
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
	Matches *matches = (Matches *)data;
	int count = matches->target_points.size();
	cout << "x = " << x << ", y = " << y << endl;
	Point point = Point(x, y);
	circle(matches->target_image, point, 2, Scalar(0, 255, 0), 2);
	putText(matches->target_image, to_string(count), point, FONT_HERSHEY_PLAIN, 2, 2);
	imshow("target", matches->target_image);
	matches->target_points.push_back(point);
	cout << "size of target points : " << count << endl;
}

Point homographic_transformation(const Mat &homography_matrix, Point input_point)
{
	Vec3d input_point_3D(input_point.x, input_point.y, 1);
	cout << " computing homographic output" << endl;
	cout << (Mat)input_point_3D << homography_matrix << endl;
	Mat output = homography_matrix * ((Mat)input_point_3D);
	double u = output.at<double>(0, 0);
	double v = output.at<double>(0, 1);
	double w = output.at<double>(0, 2);
	return Point(u / w, v / w);
}

void draw_homographic_pair(Point point, Mat homography_matrix, Image<Vec3b> source_image, Image<Vec3b> target_image)
{
	circle(source_image, point, 2, Scalar(0, 0, 255), 2);
	imshow("source", source_image);
	Point target_point = homographic_transformation(homography_matrix, point);
	circle(target_image, target_point, 2, Scalar(0, 0, 255), 2);
	cout << "target point : " << target_point << endl;
	imshow("target", target_image);
}

// Plot player points on top view
void video_homography(string video_file_path, vector<vector<ColoredRectangle>> &tracking_rectangles, Mat homography_matrix, Image<Vec3b> target_image)
{
	// Load video and initialize
	VideoCapture video(video_file_path);
	auto tracking_rectangles_iterator = tracking_rectangles.begin();
	vector<ColoredRectangle> frame_tracking_rectangles = *(tracking_rectangles_iterator);
	// Check if camera opened successfully
	if (!video.isOpened())
		cout << "Error opening video stream or file" << endl;
	cout << "Video loaded" << endl;
	Mat frame;
	video >> frame;
	int frame_index = 0;

	while (1)
	{
		if (frame.empty())
		{
			cout << "Coudl not read frame " << frame_index << endl;
			break;
		};

		Image<Vec3b> source_image(frame);
		Image<Vec3b> frame_target_image = (Image<Vec3b>)target_image.clone();
		vector<ColoredRectangle> frame_tracking_rectangles = *(tracking_rectangles_iterator);
		cout << "Frame tracking vector has " << frame_tracking_rectangles.size() << " rectangles" << endl;

		// Plot points on both source and target images
		for (int rectangle_index = 0; rectangle_index < frame_tracking_rectangles.size(); rectangle_index++)
		{
			Rect player_rectangle = frame_tracking_rectangles[rectangle_index].rect;
			float x = player_rectangle.x + player_rectangle.width / 2;
			float y = player_rectangle.y + player_rectangle.height;
			Point point(x, y);
			draw_homographic_pair(point, homography_matrix, source_image, frame_target_image);
		}

		// Increment
		tracking_rectangles_iterator++;
		frame_index += 1;
		video.read(frame);

		// Press ESC to stop
		if (waitKey(1) == 27)
			break;

		waitKey();
	}
	video.release();
}

// Select jersey colourss
void select_colour(int event, int x, int y, int foo, void *data)
{
    if (event != EVENT_LBUTTONDOWN)
        return;
	
    Matches *matches = (Matches *)data;
    Vec3b colour = matches->source_image(x, y);
	circle(matches->source_image, Point(x,y), 7, (Scalar)colour, 4);
	imshow("source", matches->source_image);
	cout << "selected colour : " << matches->source_image(x, y) << endl;
	if (waitKey() == 32) {
		matches->colours.push_back(colour);
		cout << "Added colour : " << matches->source_image(x, y) << endl;
	}

}

// Delimit the pitch
void add_pitch_point(int event, int x, int y, int foo, void *data)
{
	Matches *matches = (Matches *)data;

	if (event != EVENT_LBUTTONDOWN || matches->pitch_points_count == 4)
		return;
	int count = matches->pitch_points_count;
	Point point = Point(x, y);
	circle(matches->source_image, point, 2, Scalar(255, 255, 255), 2);
	if (count > 0) line(matches->source_image, matches->pitch[count-1], point, Scalar(0,0,255), 2);
	putText(matches->source_image, to_string(count), point, FONT_HERSHEY_PLAIN, 2, 2);
	imshow("source", matches->source_image);
	matches->pitch[count] = point;
	cout << "Added point : " << matches->pitch[count] << endl;
	matches->pitch_points_count = count + 1;

}