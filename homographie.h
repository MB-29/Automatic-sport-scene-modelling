#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "image.h"


using namespace std;
using namespace cv;


struct Matches
{
	Image<Vec3b> source_image;
	vector<Point2f> source_points;
	Image<Vec3b> target_image;
	vector<Point2f> target_points;
};

struct Homography_transformation
{
	Image<Vec3b> source_image;
	vector<Point2f> source_points;
	Image<Vec3b> target_image;
	Mat homography_matrix;
};


void add_point_source(int event, int x, int y, int foo, void *data);
void add_point_target(int event, int x, int y, int foo, void *data);
Point homographic_transformation(const Mat &homography_matrix, Point input_point);
void draw_homographic_pair(int x, int y, void *data);
void video_homography(string video_file_path, vector<vector<Rect>> &tracking_rectangles, void *data);