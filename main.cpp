#include "homographie.h"
//#include "tracking.h"
#include "detection.h"

string VIDEO_FILE_PATH = "../input/videos/ShortBasket.mp4";
string above_image_path = "../input/images/pitch_resized.png";
string photo_path = "../input/images/photo.jpg";


int main()
{	

	// Load video
	VideoCapture video(VIDEO_FILE_PATH);
	if (!video.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	};
	int frame_count = video.get(CAP_PROP_FRAME_COUNT);
	cout << "Video of " << frame_count << " frames loaded" << endl;

	// Find camera view to top view homography.
	Matches matches;
	Image<Vec3b> first_frame;
	video >> first_frame;
	Image<Vec3b> source_image = first_frame ;
	Image<Vec3b> target_image = Image<Vec3b>(imread(above_image_path));
	matches.source_image = source_image;
	matches.target_image = target_image;
	imshow("source", source_image);
	imshow("target", target_image);
	cout << "Point and click to set homographic pairs, then press any key to proceed." << endl;
	setMouseCallback("source", add_point_source, &matches);
	setMouseCallback("target", add_point_target, &matches);
	waitKey();
	destroyWindow("source");
	destroyWindow("target");
	cout << "Building homography" << endl;
	Mat homography = findHomography(matches.source_points, matches.target_points);
	// Mat homography;

	// Player detection
	vector<vector<Rect>> detected_rectangles;
	vector<vector<Rect>> matched_rectangles;
	int history = 30, sizeMinRect = 60, sizeMaxRect = 150, gaussianSize = 7, sizeBlobMin = 300, blobInt = 0;
	float threshold = 0.5;
	string technic = "a";
	cout << "Detecting rectangles" << endl;
	record_backgroundsubstract_rectangles(VIDEO_FILE_PATH, detected_rectangles, technic, history, sizeMinRect, sizeMaxRect, sizeBlobMin, blobInt, gaussianSize, threshold);
	// record_detection_rectangles(VIDEO_FILE_PATH, detected_rectangles);
	cout << "Detection complete" << endl;
	cout << "Detecting rectangles for " << detected_rectangles.size() << " frames" << endl;

	// Player tracking
	//record_tracking_rectangles(VIDEO_FILE_PATH, detected_rectangles, matched_rectangles);
	//cout << "Tracking complete" << endl;
	//cout << "Tracking vector has "<< matched_rectangles.size()<< " elements" << endl;

	// Plot points on the top view
	cout << "Starting video homography" << endl;
	video_homography(VIDEO_FILE_PATH, matched_rectangles, homography, Image<Vec3b>(imread(above_image_path)));
	cout << "Finished" << endl;

	waitKey();
	return 0;
}