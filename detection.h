#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
//#include <opencv2/tracking.hpp>

#include <iostream>
#include "image.h"


using namespace std;
using namespace cv;


//void initialize_trackers(vector<Rect2d> rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame);
// void record_hog_rectangles(string video_file_path, vector<vector<Rect>> &frame_rectangles);
//void record_tracking_rectangles(string video_file_path, vector<vector<Rect>> hog_frame_rectangles, vector<vector<Rect>> tracking_rectangles);

//void add_trackers(vector<Rect> &detected_rectangles, vector<Rect> &matched_rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame);

void moyenneMask(Mat &Moy, string filename);
void initializeMask(Mat &foregroundMask, const Mat &frame, const Mat &Moy, int seuil);
void colorMask(const Mat &img, const Mat&foreground, Mat &rst, vector<Vec3b> colors, float icolor);
void labelBlobs(const cv::Mat &binary, std::vector < std::vector<Point> > &blobs, std::vector < cv::Rect> &rects, std::vector <Vec3b> &rectsColors, int sizeMinRect, int sizeMaxRect, int sizeMinBlob, bool blobFlag, vector<Mat> colorMasks, vector<Vec3b> colors);
void record_backgroundsubstract_rectangles(string video_file_path, vector<vector<Rect>> &frame_rectangles, vector<vector<Vec3b>> &frame_colors, string technic, int history, int sizeMinRect, int sizeMaxRect, int sizeMinBlob, bool blob, int gaussianSize, float seuil, vector<Vec3b> colorsJerseys);
