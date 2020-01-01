#include "tracking.h"

float AREA_OVERLAP_THRESHOLD = 0.2;
float AREA_INCLUSION_THRESHOLD = 0.8;

// Determine weather two rectangles overlap or not, according to arbitrary constants
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

// 
vector<Rect> choose_rectangles(vector<Rect> &frame_detected_rectangles, vector<Rect> &frame_tracking_rectangles)
{
	vector<Rect> matched_rectangles = frame_tracking_rectangles;
	vector<Rect> new_rectangles;

	for (int detected_index = 0; detected_index < frame_detected_rectangles.size(); detected_index++)
	{
		Rect detected_rectangle = frame_detected_rectangles[detected_index];
		// A priori, newly detected rectangles matches a new player  
		bool new_player = true;
		// For each detected rectangle, test if it overlaps with an already matched rectangle
		for (int tracking_index = 0; tracking_index < frame_tracking_rectangles.size(); tracking_index++)
		{
			Rect tracking_rectangle = frame_tracking_rectangles[tracking_index];
			Rect intersection = tracking_rectangle & detected_rectangle;
			if (intersection.area() < AREA_OVERLAP_THRESHOLD * min(detected_rectangle.area(), tracking_rectangle.area() )) continue;
			new_player = false;
			// If it's included in an already matched rectangle, replace the latter
			if (intersection.area() > AREA_INCLUSION_THRESHOLD * detected_rectangle.area()){
				matched_rectangles[tracking_index] = detected_rectangle;
			}
		}
		if (new_player)
			new_rectangles.push_back(detected_rectangle);
	}
	// newly matched rectangles = new player rectangles + updated previously matched rectangles
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
	vector<Rect> frame_matched_rectangles = frame_detected_rectangles;
	matched_rectangles.push_back(frame_matched_rectangles);
	vector<Ptr<TrackerCSRT>> player_trackers = initialize_trackers(frame_matched_rectangles, frame);

	while (1)
	{
		// If the frame is empty, break 
		if (frame.empty())
		{
			cout << "problem" << endl;
			break;
		}

		vector<Rect> frame_detected_rectangles = detected_rectangles[frame_index];
		imshow("Tracking", frame);
		assert(player_trackers.size() == frame_matched_rectangles.size());
		imshow("Tracking", frame);

		// Update trackers initialized from matched rectangles of the previous frame
		vector<Rect> frame_tracking_rectangles;
		for (int rectangle_index = 0; rectangle_index < player_trackers.size(); rectangle_index++)
		{
			Ptr<TrackerCSRT> tracker = player_trackers[rectangle_index];
			Rect2d window = (Rect2d)frame_matched_rectangles[rectangle_index];
			tracker->update(frame, window);
			frame_tracking_rectangles.push_back((Rect)window);
		}
		imshow("Tracking", frame);

		// Matched rectangles are chosen among tracking rectangles and detected rectangles
		frame_matched_rectangles = choose_rectangles(frame_detected_rectangles, frame_tracking_rectangles);
		matched_rectangles.push_back(frame_matched_rectangles);

		// Show newly matched rectangles
		for (int rectangle_index = 0; rectangle_index < frame_matched_rectangles.size(); rectangle_index++)
		{
			Rect2d window = frame_matched_rectangles[rectangle_index];
			Vec3b color(255, 255, 255);
			rectangle(frame, window, color, 2, 1);
			putText(frame, to_string(rectangle_index), Point(window.x, window.y), FONT_HERSHEY_PLAIN, 2, 2);

		}
		imshow("Tracking", frame);

		// Initialize trackers from matched rectangles
		player_trackers = initialize_trackers(frame_matched_rectangles, frame);

		imshow("Tracking", frame);

		// Next frame
		video.read(frame);
		frame_index++;

		// Press ESC to stop
		if (waitKey(1) == 27)
			break;
	}
	video.release();
	destroyWindow("Tracking");
}

