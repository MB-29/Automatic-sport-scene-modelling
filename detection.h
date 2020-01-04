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

struct DetectionParam {
	int sizeMinRect;
	int sizeMaxRect;
	int sizeMinBlob;
	bool blobFlag;
	string technic;
	int history;
	int gaussianSize;
	float threshold;
	int proportioncolour;
	int thresholdcolour;
};

//Listes de listes de rectangles avec couleurs associ�es
struct ColoredRectangle {
	Rect rect;
	vector<int> colors;
	vector<Point> blob;
	ColoredRectangle create_Colored_Rectangle(int sizeColorsJerseys){
		auto colored_rectangle = ColoredRectangle{};
		Rect rect_created(0, 0, 0, 0);
		colored_rectangle.rect = rect_created;
		vector<int> colors_created;
		for (int i = 0; i < sizeColorsJerseys; i++) {
			colors_created.push_back(0);
		}
		colored_rectangle.colors = colors_created;
		return colored_rectangle;
	}
};

struct Input
{
	Image<Vec3b> source_image;
	vector<Point2f> source_points;
	Image<Vec3b> target_image;
	vector<Point2f> target_points;
	Mat homography_matrix;
	vector<Vec3b> colours;
	Rect player;
	Point pitch[4];
	int pitch_points_count;
};

struct pitch
{
	Image<Vec3b> source_image;
	vector<Point> polygon_points;
};


//void initialize_trackers(vector<Rect2d> rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame);
// void record_hog_rectangles(string video_file_path, vector<vector<Rect>> &frame_rectangles);
//void record_tracking_rectangles(string video_file_path, vector<vector<Rect>> hog_frame_rectangles, vector<vector<Rect>> tracking_rectangles);

//void add_trackers(vector<Rect> &detected_rectangles, vector<Rect> &matched_rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame);

Scalar moyenneMask(Mat &Moy, string filename);
void initializeMask(Mat &foregroundMask, const Mat &frame, const Mat &Moy, float seuil);
bool filter_rectangles(Rect rectangle, Point pitch[]);
void colorMask(const Mat &img, const Mat&foreground, std::vector<Mat> &rst, vector<Vec3b> colors);
void labelBlobs(const cv::Mat &binary, const Mat &frame, std::vector <ColoredRectangle> &rectangles, DetectionParam param, vector<Vec3b> colorsJerseys, Point pitch[]);
void record_backgroundsubstract_rectangles(string video_file_path, vector<vector<ColoredRectangle>> &frame_rectangles, DetectionParam param, Input &input);
int detect_colour(const Mat &frame,const ColoredRectangle &rectangle, const vector<Vec3b> &colorsJersey, DetectionParam param);
int detect_colour(const Mat &frame, const Rect &rectangle, const vector<Vec3b> &colorsJersey, DetectionParam param);
vector<vector<Rect>> get_rectangles(vector<vector<ColoredRectangle>> &colored);
