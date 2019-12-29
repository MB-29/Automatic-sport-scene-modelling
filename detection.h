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
};

//Listes de listes de rectangles avec couleurs associées
struct ColoredRectangle {
	Rect rect;
	vector<int> colors;
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


//void initialize_trackers(vector<Rect2d> rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame);
// void record_hog_rectangles(string video_file_path, vector<vector<Rect>> &frame_rectangles);
//void record_tracking_rectangles(string video_file_path, vector<vector<Rect>> hog_frame_rectangles, vector<vector<Rect>> tracking_rectangles);

//void add_trackers(vector<Rect> &detected_rectangles, vector<Rect> &matched_rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame);

Scalar moyenneMask(Mat &Moy, string filename);
void initializeMask(Mat &foregroundMask, const Mat &frame, const Mat &Moy, float seuil);
<<<<<<< HEAD
bool filter_rectangles(ColoredRectangle rectangle, Point pitch[]);
void colorMask(const Mat &img, const Mat&foreground, std::vector<Mat> &rst, vector<Vec3b> colors);
void labelBlobs(const cv::Mat &binary, std::vector < std::vector<Point> > &blobs, std::vector <ColoredRectangle> &rectangles, DetectionParam param, vector<Mat> colorMasks, vector<Vec3b> colorsJerseys, Point pitch[]);
void record_backgroundsubstract_rectangles(string video_file_path, vector<vector<ColoredRectangle>> &frame_rectangles, DetectionParam param, vector<Vec3b> colorsJerseys, Point pitch[]);
=======
vector<vector<Rect>> filter_rectangles(vector<vector<Rect>> &detection_rectangles, Point pitch[]);
void colorMask(const Mat &img, const Mat&foreground, Mat &rst, vector<Vec3b> colors, float icolor);
void labelBlobs(const cv::Mat &binary, std::vector < std::vector<Point> > &blobs, std::vector < cv::Rect> &rects, std::vector <Vec3b> &rectsColors, Detection_param param, vector<Mat> colorMasks, vector<Vec3b> colors);
void record_backgroundsubstract_rectangles(string video_file_path, vector<vector<Rect>> &frame_rectangles, vector<vector<Vec3b>> &frame_colors, Detection_param param, vector<Vec3b> colorsJerseys);
vector<vector<Rect>> filter_rectangles(vector<vector<Rect>> &detection_rectangles, Point pitch[]);
int get_jersey_color(Image<Vec3b> frame, Rect rectangle, Vec3b jersey_colour_1, Vec3b jersey_colour_2);

>>>>>>> jerseys
