#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "image.h"
#include "detection.h"

using namespace std;
using namespace cv;

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

void add_point_source(int event, int x, int y, int foo, void *data);
void add_point_target(int event, int x, int y, int foo, void *data);
Point homographic_transformation(const Mat &homography_matrix, Point input_point);
void draw_homographic_pair(Point point, Mat homography_matrix, Image<Vec3b> source_image, Image<Vec3b> target_image, Vec3b colors);
void video_homography(string video_file_path, vector<vector<Rect>> &tracking_rectangles, void *data);
void select_colour(int event, int x, int y, int foo, void *data);
void add_pitch_point(int event, int x, int y, int foo, void *data);
double norm(Vec3b vector);
int get_jersey_colour(Mat &frame, Rect rectangle, Vec3b jersey_colour_1, Vec3b jersey_colour_2);
