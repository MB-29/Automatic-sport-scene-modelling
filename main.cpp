
#include "homography.h"
//#include "tracking.h"
#include "detection.h"


string VIDEO_FILE_PATH = "../input/videos/ShortBasket.mp4";

//string VIDEO_FILE_PATH = "/Users/matthieu/Movies/tracking/short.mp4";
// string VIDEO_FILE_PATH = "/Users/matthieu/Movies/tracking/tennis2.mp4";
// string VIDEO_FILE_PATH = "/Users/matthieu/Movies/tracking/tennis_short.mp4";

string above_image_path = "../input/images/pitch_resized.png";
// string above_image_path = "../input/images/tennis_top.jpg";

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

	// Initialize calibration
	Matches matches;
	DetectionParam param;
	Image<Vec3b> first_frame;
	video >> first_frame;
	Image<Vec3b> source_image = first_frame ;
	Image<Vec3b> target_image = Image<Vec3b>(imread(above_image_path));
	matches.source_image = source_image;
	matches.target_image = target_image;
	imshow("source", source_image);
	// imshow("target", target_image);

	// Find camera view to top view homography.
	// cout << "Point and click to set homographic pairs, then press any key to proceed." << endl;
	// setMouseCallback("source", add_point_source, &matches);
	// setMouseCallback("target", add_point_target, &matches);
	// waitKey();
	// destroyWindow("source");
	// destroyWindow("target");
	// cout << "Building homography" << endl;
	// Mat homography = findHomography(matches.source_points, matches.target_points);
	Mat homography;

	// Delimit pitch area by pointing and clicking
	cout << "Point and click to delimit the pitch, press ENTER to validate." << endl;
	matches.pitch_points_count = 0;
	setMouseCallback("source", add_pitch_point, &matches);
	if (waitKey()==32){
		if (matches.pitch_points_count < 4){
			cout << "Error : pitch must be delimited by exactly 4 points" << endl;
			return 1;
		}
	};

	// Select a player	
	cout << "Select a player" << endl;
	matches.player = selectROI("select tracker", source_image);
	int typical_height = matches.player.height;
	//int typical_height = 100;


	// Select colours
	cout << "Point and click to select a color, press space to add, press a key validate." << endl;
	setMouseCallback("source", select_colour, &matches);
	waitKey();
	cout << "Number of colours : " << matches.colours.size() << endl;

	// Player detection
	vector<vector<ColoredRectangle>> detected_rectangles;

	param.history = 5;
	param.sizeMinRect = 0.5*typical_height;
	param.sizeMaxRect = 1.5*typical_height;
	param.gaussianSize = 5;
	param.sizeMinBlob = 300;
	param.blobFlag = false;
	param.threshold = 0.5;
	param.technic = "a";
	cout << "Detecting rectangles" << endl;
	record_backgroundsubstract_rectangles(VIDEO_FILE_PATH, detected_rectangles, param, matches.colours, matches.pitch);
	// record_detection_rectangles(VIDEO_FILE_PATH, detected_rectangles);
	cout << "Detection complete" << endl;
	cout << "Detecting rectangles for " << detected_rectangles.size() << " frames" << endl;

	// Filter rectangles
	//detected_rectangles = filter_rectangles(detected_rectangles, matches.pitch);

	// Player tracking

	vector<vector<ColoredRectangle>> matched_rectangles;
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