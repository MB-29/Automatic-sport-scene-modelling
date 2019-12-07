#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>

#include <iostream>
#include "image.h"


using namespace std;
using namespace cv;


void initialize_trackers(vector<Rect2d> rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame);
void record_hog_rectangles(string video_file_path, vector<vector<Rect>> &frame_rectangles);
void record_tracking_rectangles(string video_file_path, vector<vector<Rect>> &hog_frame_rectangles, vector<vector<Rect>> &matched_rectangles);
bool overlap(Rect new_rectangle, vector<Rect> tracking_rectangles);
void add_trackers(vector<Rect> &detected_rectangles, vector<Rect> &matched_rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame);