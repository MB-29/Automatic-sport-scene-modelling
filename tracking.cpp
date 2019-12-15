#include "tracking.h"

// string VIDEO_FILE_PATH = "/Users/matthieu/Movies/tracking/Basket.mp4";
float AREA_OVERLAP_THRESHOLD = 0.5;
float AREA_INCLUSION_THRESHOLD = 0.8;

bool overlap(Rect new_rectangle, vector<Rect> tracking_rectangles)
{
	int tracked_players_count = tracking_rectangles.size();
	for (int player_index = 0; player_index < tracked_players_count; player_index++)
	{
		Rect player_rectangle = tracking_rectangles[player_index];
		int min_area = min(new_rectangle.area(), player_rectangle.area());
		int intersection_area = (player_rectangle & new_rectangle).area();
		if (intersection_area > min_area * AREA_OVERLAP_THRESHOLD)
			return true;
	}
	return false;
}

vector<Rect> choose_rectangles(vector<Rect> &frame_detected_rectangles, vector<Rect> &frame_tracking_rectangles)
{
	vector<Rect> matched_rectangles = frame_tracking_rectangles;
	vector<Rect> new_rectangles;

	for (int detected_index = 0; detected_index < frame_detected_rectangles.size(); detected_index++)
	{
		Rect detected_rectangle = frame_detected_rectangles[detected_index];
		bool new_player = true;
		for (int tracking_index = 0; tracking_index < frame_tracking_rectangles.size(); tracking_index++)
		{
			Rect tracking_rectangle = frame_tracking_rectangles[tracking_index];
			Rect intersection = tracking_rectangle & detected_rectangle;
			if (intersection.area() < AREA_OVERLAP_THRESHOLD * min(detected_rectangle.area(), tracking_rectangle.area() )) continue;
			new_player = false;
			if (intersection.area() > AREA_INCLUSION_THRESHOLD * detected_rectangle.area()){
				matched_rectangles[tracking_index] = detected_rectangle;
			}
		}
		if (new_player)
			new_rectangles.push_back(detected_rectangle);
	}
	matched_rectangles.insert(matched_rectangles.end(), new_rectangles.begin(), new_rectangles.end());
	return matched_rectangles;
}

vector<Ptr<TrackerCSRT>> initialize_trackers(vector<Rect> &matched_rectangles, Mat &frame)
{
	vector<Ptr<TrackerCSRT>> player_trackers;
	for (auto iterator = matched_rectangles.begin(); iterator != matched_rectangles.end(); iterator++)
	{
		Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
		tracker->init(frame, *(iterator));
		player_trackers.push_back(tracker);
	}
	return player_trackers;
}

// void add_trackers(vector<Rect> &frame_detected_rectangles, vector<Rect> &matched_rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame)
// {
// 	for (auto iterator = frame_detected_rectangles.begin(); iterator != frame_detected_rectangles.end(); iterator++)
// 	{
// 		Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
// 		tracker->init(frame, *(iterator));
// 		if (player_trackers.size() == 0)
// 		{
// 			cout << "no matched rectangles" << endl;
// 			player_trackers.push_back(tracker);
// 			matched_rectangles.push_back(*(iterator));
// 			cout << "Matched rectangles count : " << matched_rectangles.size() << endl;
// 			cout << "detected rectangles count : " << frame_detected_rectangles.size() << endl;
// 			return;
// 		}
// 		if (!overlap(*(iterator), matched_rectangles))
// 		{
// 			Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
// 			tracker->init(frame, *(iterator));
// 			player_trackers.push_back(tracker);
// 			matched_rectangles.push_back(*(iterator));
// 		}
// 	}
// }

void record_tracking_rectangles(string VIDEO_FILE_PATH, vector<vector<Rect>> &detected_rectangles, vector<vector<Rect>> &matched_rectangles)
{
	// Load video
	VideoCapture video(VIDEO_FILE_PATH);
	if (!video.isOpened())
		cout << "Error opening video stream or file" << endl;
	Mat frame;
	video >> frame;
	int frame_index = 0;

	vector<Rect> frame_detected_rectangles = detected_rectangles[0];
	// if (frame_detected_rectangles.size() == 0)
	// {
	// 	cout << "No rectangles for initital tracking, please select one." << endl;
	// 	Rect window = selectROI("select tracker", frame);
	// 	frame_detected_rectangles.push_back(window);
	// 	destroyWindow("select tracker");
	// }

	// Initialize trackers from detected rectangles

	vector<Rect> frame_matched_rectangles = frame_detected_rectangles;
	matched_rectangles.push_back(frame_matched_rectangles);
	vector<Ptr<TrackerCSRT>> player_trackers = initialize_trackers(frame_matched_rectangles, frame);
	// add_trackers(frame_detected_rectangles, frame_tracking_rectangles, player_trackers, frame);

	while (1)
	{
		cout << "Reading frame " << frame_index << endl;
		// If the frame is empty, break immediately
		if (frame.empty())
		{
			cout << "problem" << endl;
			break;
		}

		vector<Rect> frame_detected_rectangles = detected_rectangles[frame_index];
		cout << "detected rectangles count : " << frame_detected_rectangles.size() << endl;
		// Show detection rectangles
		// for (int rectangle_index = 0; rectangle_index < frame_detected_rectangles.size(); rectangle_index++)
		// {
		// 	Rect2d window = frame_detected_rectangles[rectangle_index];
		// 	rectangle(frame, window, Scalar(0, 0, 255), 2, 1);
		// }
		imshow("Tracking", frame);

		assert(player_trackers.size() == frame_matched_rectangles.size());

		// Show previously matched rectangles
		// for (int rectangle_index = 0; rectangle_index < frame_matched_rectangles.size(); rectangle_index++)
		// {
		// 	Rect2d window = frame_matched_rectangles[rectangle_index];
		// 	rectangle(frame, window, Scalar(0, 0, 0), 2, 1);
		// }
		imshow("Tracking", frame);

		// Update trackers initialized from matched rectangles of the previous frame
		vector<Rect> frame_tracking_rectangles;
		for (int rectangle_index = 0; rectangle_index < player_trackers.size(); rectangle_index++)
		{
			Ptr<TrackerCSRT> tracker = player_trackers[rectangle_index];
			Rect2d window = (Rect2d)frame_matched_rectangles[rectangle_index];
			tracker->update(frame, window);
			frame_tracking_rectangles.push_back((Rect)window);
			// rectangle(frame, frame_tracking_rectangles[rectangle_index], Scalar(255, 0, 0), 2, 1);
		}
		imshow("Tracking", frame);

		// Matched rectangles are chosen among tracking rectangles and detected rectangles
		cout << "Choosing matched rectangles " << endl;
		frame_matched_rectangles = choose_rectangles(frame_detected_rectangles, frame_tracking_rectangles);
		cout << "matched rectangles count : " << frame_matched_rectangles.size() << endl;

		// Add matched rectangles to the output vector
		// vector<Rect> detected_rectangles = detected_rectangles_iterator[frame_index];
		// add_trackers(detected_rectangles, frame_matched_rectangles, player_trackers, frame);
		matched_rectangles.push_back(frame_matched_rectangles);
		// Show newly matched rectangles
		for (int rectangle_index = 0; rectangle_index < frame_matched_rectangles.size(); rectangle_index++)
		{
			Rect2d window = frame_matched_rectangles[rectangle_index];
			rectangle(frame, window, Scalar(255, 255, 255), 2, 1);
			putText(frame, to_string(rectangle_index), Point(window.x, window.y), FONT_HERSHEY_PLAIN, 2, 2);

		}
		imshow("Tracking", frame);
		// Initialize trackers from matched rectangles
		player_trackers = initialize_trackers(frame_matched_rectangles, frame);

		imshow("Tracking", frame);

		// Next frame
		video.read(frame);
		frame_index++;
		// waitKey();

		// Press ESC to stop
		if (waitKey(1) == 27)
			break;
	}
	video.release();
	destroyWindow("Tracking");
}

void record_detection_rectangles(string video_file_path, vector<vector<Rect>> &detected_rectangles)
{
	float weight_threshold = 0.0;
	int frame_index = 0;
	VideoCapture video(video_file_path);

	// Check if camera opened successfully
	if (!video.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
	};
	Mat frame;
	video >> frame;

	// Pedestrian recordor
	HOGDescriptor detection;
	detection.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	while (1)
	{

		// Capture frame-by-frame
		video.read(frame);
		// cout << "frame index : " << frame_index << endl;

		// If the frame is empty, break immediately
		if (frame.empty())
		{
			cout << "Coudl not read frame " << frame_index << endl;
			break;
		}

		vector<Rect> recordion_rectangles;
		vector<double> weights;

		Mat gray_frame;
		cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
		detection.detectMultiScale(gray_frame, recordion_rectangles, weights);
		int detection_rectangle_count = recordion_rectangles.size();
		// cout << "recordion complete, number of reactangles deteceted : " << detection_rectangle_count << endl;

		detected_rectangles.push_back(recordion_rectangles);
		/// draw recordions
		for (size_t i = 0; i < detection_rectangle_count; i++)
		{
			if (weights[i] < weight_threshold)
				continue;
			rectangle(gray_frame, recordion_rectangles[i], cv::Scalar(0, 0, 255), 3);
		}

		// Display the resulting frame
		imshow("HOG detection", gray_frame);

		// Press ESC to stop
		if (waitKey(1) == 27)
			break;

		frame_index++;
	}
	video.release();
	destroyWindow("HOG detection");
}

// int main()
// {
// 	VideoCapture video(VIDEO_FILE_PATH);

// 	// Check if camera opened successfully
// 	if (!video.isOpened())
// 	{
// 		cout << "Error opening video stream or file" << endl;
// 		return -1;
// 	};

// 	int frame_count = video.get(CAP_PROP_FRAME_COUNT);
// 	cout << "Video of " << frame_count << " frames loaded" << endl;
// 	vector<vector<Rect>> detected_rectangles;
// 	vector<vector<Rect>> tracking_rectangles;

// 	record_detected_rectangles(VIDEO_FILE_PATH, detected_rectangles);
// 	int rectangle_count = detected_rectangles[10].size();

// 	record_tracking_rectangles(VIDEO_FILE_PATH, detected_rectangles, tracking_rectangles);

// 	// cout << "number of rectangles for frame " << 10 << " : " << rectangle_count << endl;
// }