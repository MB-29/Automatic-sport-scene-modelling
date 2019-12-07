#include "homographie.h"
#include "tracking.h"

string VIDEO_FILE_PATH = "/Users/matthieu/Movies/tracking/short.mp4";
string above_image_path = "../input/images/pitch_resized.png";
string photo_path = "../input/images/photo.jpg";


int main()
{	
	// Find a homography from the live match camera angle to the top view of the pitch.
	VideoCapture video(VIDEO_FILE_PATH);
	// Check if camera opened successfully
	if (!video.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	};
	Matches matches;
	Image<Vec3b> first_frame;
	video >> first_frame;
	Image<Vec3b> source_image = first_frame ;
	Image<Vec3b> target_image = Image<Vec3b>(imread(above_image_path));
	matches.source_image = source_image;
	matches.target_image = target_image;
	imshow("source", source_image);
	imshow("target", target_image);
	setMouseCallback("source", add_point_source, &matches);
	setMouseCallback("target", add_point_target, &matches);

	waitKey();
	destroyWindow("source");
	destroyWindow("target");

	cout << "Building homography" << endl;
	Mat homography = findHomography(matches.source_points, matches.target_points);


	int frame_count = video.get(CAP_PROP_FRAME_COUNT);
	cout << "Video of " << frame_count << " frames loaded" << endl;
	// Check if camera opened successfully
	if (!video.isOpened())
		cout << "Error opening video stream or file" << endl;

	vector<vector<Rect>> hog_frame_rectangles;
	vector<vector<Rect>> matched_rectangles;

	record_hog_rectangles(VIDEO_FILE_PATH, hog_frame_rectangles);
	int rectangle_count = hog_frame_rectangles[10].size();

	record_tracking_rectangles(VIDEO_FILE_PATH, hog_frame_rectangles, matched_rectangles);
	cout << "Tracking complete" << endl;
	cout << "Tracking vector has "<< matched_rectangles.size()<< " elements" << endl;

	
	Homography_transformation input;
	input.source_image = first_frame;
	input.target_image = Image<Vec3b>(imread(above_image_path));
	input.homography_matrix = homography;
	cout << "Starting video homography" << endl;
	video_homography(VIDEO_FILE_PATH, matched_rectangles, &input);
	waitKey();
	return 0;
}