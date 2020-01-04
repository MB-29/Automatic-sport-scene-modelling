
#include "calibration.h"
#include "tracking.h"
#include "detection.h"
#include "output.h"

int main(int argc, char** argv)
{	
	// Parse arguments
    const String keys = "{@source | | source video file path }"
						"{@target | | top view image}";

	CommandLineParser parser(argc, argv, keys);
	String source_path = parser.get<String>(0);
	//String source_path = "../input/videos/ShortBasket.mp4";

	String target_path = parser.get<String>(1);
	//String target_path = "../input/images/pitch_resized.png";

	// Load video
	VideoCapture video(source_path);
	if (!video.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	};
	int frame_count = video.get(CAP_PROP_FRAME_COUNT);
	cout << "Video of " << frame_count << " frames loaded" << endl;

	// Initialize calibration
	Input input;
	DetectionParam param;
	Image<Vec3b> first_frame;
	video >> first_frame;
	Image<Vec3b> source_image = first_frame ;
	Image<Vec3b> target_image = Image<Vec3b>(imread(target_path));
	input.source_image = source_image;
	input.target_image = target_image;
	imshow("source", source_image);
	imshow("target", target_image);

	// Find camera view to top view homography.
	cout << "Point and click to set homographic pairs, then press any key to proceed." << endl;
	setMouseCallback("source", add_point_source, &input);
	setMouseCallback("target", add_point_target, &input);
	waitKey();
	destroyWindow("target");
	cout << "Computing homography" << endl;
	input.homography_matrix = findHomography(input.source_points, input.target_points);
	input.target_image = Image<Vec3b>(imread(target_path));

	// Delimit pitch area by pointing and clicking
	cout << "Point and click to delimit the pitch, press ENTER to validate." << endl;
	input.pitch_points_count = 0;
	setMouseCallback("source", add_pitch_point, &input);
	if (waitKey()==32){
		if (input.pitch_points_count < 4){
			cout << "Error : pitch must be delimited by exactly 4 points" << endl;
			return 1;
		}
	}

	// Select a player	
	cout << "Click and drag to delimit a player" << endl;
	input.player = selectROI("source", source_image);
	int typical_height = input.player.height;

	// Select colours
	cout << "Point and click to select a jersey colour, press SPACE to add, press any key to validate." << endl;
	setMouseCallback("source", select_colour, &input);
	waitKey();
	cout << "Number of colours : " << input.colours.size() << endl;

	// Player detection
	vector<vector<ColoredRectangle>> detected_colored_rectangles;
	vector<vector<Rect>> detected_rectangles;

	param.history = 5;
	param.sizeMinRect = 0.4*typical_height;
	param.sizeMaxRect = 1.5*typical_height;
	param.gaussianSize = 5;
	param.sizeMinBlob = 300;
	param.blobFlag = false;
	param.threshold = 0.5;// En HSV, la distance entre 2 couleurs varie plut�t entre 50000 et 100000. En BGR, entre 0 et 1
	param.technic = "a";
	param.proportioncolour = 100;
	cout << "Detecting rectangles" << endl;
	record_backgroundsubstract_rectangles(source_path, detected_colored_rectangles, param, input);
	// record_detection_rectangles(VIDEO_FILE_PATH, detected_rectangles);
	cout << "Detection complete" << endl;
	cout << "Detecting rectangles for " << detected_rectangles.size() << " frames" << endl;
	detected_rectangles  = get_rectangles(detected_colored_rectangles);

	// Player tracking
	vector<vector<Rect>> matched_rectangles;
	record_tracking_rectangles(source_path, detected_rectangles, matched_rectangles);
	cout << "Tracking complete" << endl;
	cout << "Tracking vector has "<< matched_rectangles.size()<< " elements" << endl;


	// Plot points on the top view 
	cout << "Displaying output. Press any key to go forward." << endl;
	Image<Vec3b> cumulated_positions = video_homography(source_path, matched_rectangles, &input, param);
	imshow ("cumulated positions", cumulated_positions);

	waitKey();
	return 0;
}