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
	Mat output = homography_matrix * ((Mat)input_point_3D);
	double u = output.at<double>(0, 0);
	double v = output.at<double>(0, 1);
	double w = output.at<double>(0, 2);
	return Point(u / w, v / w);
}

void draw_homographic_pair(int x, int y, void *data)
{
	Homography_transformation *input = (Homography_transformation *)data;
	cout << "Applying homography" << endl;

	// cout << "drawing a circle" << endl;
	circle(input->source_image, Point(x, y), 2, Scalar(0, 0, 255), 2);
	imshow("source", input->source_image);

	Point target_point = homographic_transformation(input->homography_matrix, Point(x, y));

	circle(input->target_image, target_point, 2, Scalar(0, 0, 255), 2);
	cout << "target point : " << target_point << endl;
	imshow("target", input->target_image);
}

void video_homography(string video_file_path, vector<vector<Rect>> &tracking_rectangles, void *data)
{	
	cout << "vector has "<< tracking_rectangles.size()<< " elements" << endl;


	cout << "Loading file"<< video_file_path << endl;
	VideoCapture video(video_file_path);
	cout << "Recovering homography data"<< endl;
	Homography_transformation *homography_data = (Homography_transformation *)data;
	Image<Vec3b> target_image = homography_data->target_image;
	cout << "Iterator"<< endl;
	auto tracking_rectangles_iterator = tracking_rectangles.begin();
	cout << "deference"<< endl;
	vector<Rect> frame_tracking_rectangles = *(tracking_rectangles_iterator);
	cout << "first frame has "<< frame_tracking_rectangles.size()<< "rectangles" << endl;

	// Check if camera opened successfully
	if (!video.isOpened())
		cout << "Error opening video stream or file" << endl;
	cout << "Video loaded" << endl;
	
	Mat frame;
	video >> frame;

	int frame_index = 0;

	while (1)
	{

		cout << "Reading frame "<<frame_index << endl;	
		frame_index += 1;
		video.read(frame);
		// If the frame is empty, break immediately
		if (frame.empty())
		{
			cout << "problem" << endl;
			break;
		};
		homography_data->source_image = Image<Vec3b>(frame);
		Image<Vec3b> pitch_from_above(target_image);
		homography_data->target_image = pitch_from_above;

		vector<Rect> frame_tracking_rectangles = *(tracking_rectangles_iterator);
		cout << "Frame tracking vector has "<< frame_tracking_rectangles.size() << " rectangles" << endl;	


		for (int rectangle_index = 0; rectangle_index < frame_tracking_rectangles.size(); rectangle_index++)
		{
			Rect player_rectangle = frame_tracking_rectangles[rectangle_index];
			float x = player_rectangle.x + player_rectangle.width / 2;
			float y = player_rectangle.y + player_rectangle.height / 2;
			cout << "rectangle index = " << rectangle_index <<" x = "<< x << " y = " << y << endl;	
			draw_homographic_pair(x, y, homography_data);
		}
		
		frame_index++;
		tracking_rectangles_iterator++;

		// Press ESC to stop
		if (waitKey(1) == 27)
			break;
		
		waitKey();
	}
	video.release();
}
