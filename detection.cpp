#include "detection.h"
#include "calibration.h"
#include "polygon.cpp"



// Computes the mean colour of each pixel during the video
// Returns the mean of these means
Scalar moyenneMask(Mat &Moy, string filename)
{
	VideoCapture capInit(filename);
	Image<Vec3b> img;
	int i = 0;
	bool ok = capInit.grab();
	int frame_count = capInit.get(CAP_PROP_FRAME_COUNT);
	std::cout << "Video of " << frame_count << " frames loaded" << endl;
	capInit.retrieve(img, CAP_OPENNI_BGR_IMAGE);

	vector<Scalar> meanColour(frame_count, 1);


	int n = img.cols;
	int m = img.rows;
	Image<Vec3f> Interm, Interm2;
	img.convertTo(Moy, CV_32F, 1 / (255.0 * frame_count));


	for (;;)
	{

		bool ok = capInit.grab();
		if (ok == false)
		{

			std::cout << "Video Capture Fail Init 1" << std::endl;
			break;
		}
		else
		{

			capInit.retrieve(img, CAP_OPENNI_BGR_IMAGE);
			img.convertTo(Interm, CV_32F, 1 / (255.0 * frame_count));
			img.convertTo(Interm2, CV_32F);
			add(Moy, Interm, Moy);
			meanColour.push_back(mean(Interm2));
			i++;
			
		}
	}
	Scalar globalMeanColour = mean(meanColour);
	cout << "Mean colour of the background" << globalMeanColour << endl;
	destroyAllWindows();
	capInit.release();
	return globalMeanColour;
}

// Computes a foregroundMask by comparing the frame and the Mean image computed by moyenneMask()
void initializeMask(Mat &foregroundMask, const Mat &frame, const Mat &Moy, float seuil)
{
	Image<Vec3f> imgFloat;
	frame.convertTo(imgFloat, CV_32F, 1 / 255.0);
	int n = frame.cols;
	int m = frame.rows;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (norm(imgFloat.at<Vec3f>(i, j), Moy.at<Vec3f>(i, j), NORM_L2) > seuil)
			{
				foregroundMask.at<uchar>(i, j) = 255;
			}
			else
			{
				foregroundMask.at<uchar>(i, j) = 0;
			}
		}
	}
	imshow("maskMoy", foregroundMask);
}

// Computes the connected components of the foregroundMask
void labelBlobs(const cv::Mat &binary, const Mat &frame, std::vector <ColoredRectangle> &rectangles, DetectionParam param, vector<Vec3b> colorsJerseys, Point pitch[])
{
	rectangles.clear();
	int c = colorsJerseys.size();


	// Using labels from 2+ for each blob
	cv::Mat label_image, label_image_old, mask;
	binary.convertTo(label_image, CV_32FC1);
	for (int i = 0; i < label_image.rows; i++)
	{
		for (int j = 0; j < label_image.cols; j++)
		{
			if ((int)label_image.at<int>(i, j) > 0)
			{
				label_image.at<int>(i, j) = 0;
			}
			else
			{
				label_image.at<int>(i, j) = 255;
			}
		}
	}
	label_image.copyTo(label_image_old);


	int label_count = 2; // starts at 2 because 0,1 are used already
	int m = label_image.rows;
	int n = label_image.cols;
	mask = Mat::zeros(Size(n + 2, m + 2), CV_8U);
	for (int y = 0; y < m; y++)
	{
		for (int x = 0; x < n; x++)
		{
			if (((int)label_image.at<int>(y, x) == 0))
			{
				ColoredRectangle rectangle;
				rectangle = rectangle.create_Colored_Rectangle(c);

				Rect rectToFill;

				cv::floodFill(label_image, mask, cv::Point(x, y), cv::Scalar(label_count), &rectToFill, cv::Scalar(0), cv::Scalar(1), 8);
				// loDiff (maximal lower diff to connect) = 0
				// upDiff (maximal upper diff to connect) = 1
				// last arguments : 4 if only 4 neighbours are being checked ; 8 is 8 neighbours are being checked
				std::vector<Point> blob;
				rectangle.rect = rectToFill;

				
				if (filter_rectangles(rectangle.rect, pitch)==true) {// The rectangle is kept only if it has a corner inside the pitch 
					// Creates a blob (list of Point) with all the points of the connected component
					for (int i = rectangle.rect.y; i < (rectangle.rect.y + rectangle.rect.height); i++)
					{
						for (int j = rectangle.rect.x; j < (rectangle.rect.x + rectangle.rect.width); j++)
						{
							if ((int)label_image.at<int>(i, j) != (int)label_image_old.at<int>(i, j))
							{

								blob.push_back(cv::Point(j, i));
							}
						}
					}

					// Detects the colour of the player's jersey
					// This colour will be computed again after the tracking, and this time we will suppress the rectangles with colour "-1"
					if (((param.blobFlag) && (blob.size() > param.sizeMinBlob)) || (!param.blobFlag)) {
						if ((rectangle.rect.height > param.sizeMinRect) && (rectangle.rect.height < param.sizeMaxRect)) {
							rectangle.blob = blob;
							int icolour = detect_colour(frame, rectangle.rect, colorsJerseys, param); // We use the version of detect_colour taking "Rect" as arguments because its parameters are more adapted
							if (icolour != (-1)) {
								rectangle.colors[icolour] = 1; // We save the colour of the rectangle, to show it with the right colour after
							}
							rectangles.push_back(rectangle);
						}
					}
				}

				label_count++;
				label_image.copyTo(label_image_old);
			}
		}
	}
}

void record_backgroundsubstract_rectangles(string video_file_path, vector<vector<ColoredRectangle>> &frame_rectangles, DetectionParam param, Input &input)
{

	// Init background substractor
	Ptr<BackgroundSubtractorKNN> bg_model = createBackgroundSubtractorKNN(param.history, 400, true).dynamicCast<BackgroundSubtractorKNN>();

	// Create empty input img, foreground and background image and foreground mask.
	Mat img, foregroundMask, backgroundImage, foregroundImg;

	Image<Vec3f> Moy;

	Scalar meanColor = moyenneMask(Moy, video_file_path);

	Vec3b meanColorVec3b = Vec3b(meanColor[0], meanColor[1], meanColor[2]); // In HSV !

	cout << "Mean colour of the background" << meanColorVec3b << endl;

	// The last color of this list is the mean color of the videp
	input.colours.push_back(meanColorVec3b);

	// Load the video
	VideoCapture cap(video_file_path);
	int frame_count = cap.get(CAP_PROP_FRAME_COUNT);
	cout << "Video of " << frame_count << " frames loaded" << endl;

	int frame_index = 1;

	// main loop to grab sequence of input files
	for (;;)
	{

		bool ok = cap.grab();

		if (ok == false)
		{

			std::cout << "Video Capture Fail" << std::endl;
			break;
		}
		frame_index++;

		// obtain input image from source
		cap.retrieve(img, CAP_OPENNI_BGR_IMAGE);

		if (foregroundMask.empty())
		{
			foregroundMask.create(img.size(), CV_8U);
		}

		// compute foreground mask 8 bit image
		// -1 is parameter that chose automatically your learning rate
		bg_model->apply(img, foregroundMask, true ? -1 : 0);

		// For the 10 first frames, we use an alternative method to compute the foregroundMask
		if ((frame_index < 10) && (param.threshold > 0))
		{
			foregroundMask.create(img.size(), CV_8U);
			initializeMask(foregroundMask, img, Moy, param.threshold);
		}

	
		// smooth the mask to reduce noise in image
		GaussianBlur(foregroundMask, foregroundMask, Size(param.gaussianSize, param.gaussianSize), 3.5, 3.5);

	
		// threshold mask to saturate at black and white values
		threshold(foregroundMask, foregroundMask, 10, 255, THRESH_BINARY);

		// create black foreground image
		foregroundImg = Scalar::all(0);
	
		// Copy source image to foreground image only in area with white mask
		img.copyTo(foregroundImg, foregroundMask);

		//Get background image
		bg_model->getBackgroundImage(backgroundImage);


		std::vector< ColoredRectangle > rects;
		labelBlobs(foregroundMask, img, rects, param, input.colours, input.pitch);

		
		// Save the list of rectangles of this frame
		frame_rectangles.push_back(rects);

		// Add the rectangles to the foreground Image
		Mat foregroundImgWithRect;
		foregroundImg.copyTo(foregroundImgWithRect);
		for (int k = 0; k < rects.size(); k++)
		{	
			Vec3b colorOfTheRectangle;
			for (int l = 0; l < input.colours.size(); l++) {
				if (rects[k].colors[l] == 1) {
					colorOfTheRectangle = input.colours[l];

				}
			}
			rectangle(foregroundImgWithRect, rects[k].rect, colorOfTheRectangle, 3);

		}

		// Show the results
		imshow("real image", img);
		imshow("foreground mask", foregroundMask);
		imshow("foreground image", foregroundImgWithRect);

		if (waitKey(25) == 27)
			break;

		if (!backgroundImage.empty())
		{
			imshow("mean background image", backgroundImage);
		}
	}
	destroyWindow("foreground image");
	destroyWindow("mean background image");
	destroyWindow("foreground mask");
	destroyWindow("real image");
}

// Checks if there is a corner of the rectangle inside the pitch
bool filter_rectangles(Rect rectangle, Point pitch[])
{
			float x = rectangle.x, y = rectangle.y, w = rectangle.width, h = rectangle.height;
			Point corners[4] = {Point(x, y), Point(x + w, y), Point(x, y + h), Point(x + w, y + h)};
			bool out = true;
			for (int corner_index = 0; corner_index < 4; corner_index++)
			{
				Point point = corners[corner_index];
				if (isInside(pitch, 4, point))
					out = false;
			}

	return !out;
}

//Detect the colour of a rectangle, if we know the "blob" (list of Point) corresponding
int detect_colour(const Mat &frame, const ColoredRectangle &rectangle, const vector<Vec3b> &colorsJersey, DetectionParam param)// N'est pas utilisée directement sur les blobs finalement, mais pourrait l'^^etre si les seuils étaient adaptés
{
	if (rectangle.colors.size() != colorsJersey.size()) {
		cout << "Error : different numbers of jersey colours" << endl;
	}
	if (rectangle.blob.size() == 0) {
		int iColour = detect_colour(frame, rectangle, colorsJersey, param);
		return iColour;
	}
	else {

		Image<Vec3b> image(frame);
		Image<Vec3b> imgHSV;
		int c = colorsJersey.size();
		vector<Point> points = rectangle.blob;
		int t = points.size();
		Mat_<Vec3b> matColorBGR(1, c);
		Mat_<Vec3b> matColorHSV(1, c);
		Mat_<int> distColor(c, 1, CV_32FC1);
		for (int l = 0; l < c; l++) {
			distColor.at<int>(l, 0) = 0;
		}
		for (int r = 0; r < c; r++) {
			matColorBGR.at<Vec3b>(0, r) = colorsJersey[r];
		}

		cvtColor(matColorBGR, matColorHSV, COLOR_BGR2HSV);
		cvtColor(image, imgHSV, COLOR_BGR2HSV);
		int n = imgHSV.cols;
		int m = imgHSV.rows;

		// Computes the distColor matrix by looking, for each pixel, to the colour of "colorsJersey" that is the closest to that pixel
		for (int k = 0; k < t; k++) {
			int iColorMaj = c-1;

			for (int r = 0; r < c-1; r++) {
				int norm1 = (int)(norm(imgHSV.at<Vec3b>(points[k].y, points[k].x), matColorHSV.at<Vec3b>(0, r), NORM_L2));
				int norm2 = (int)(norm(imgHSV.at<Vec3b>(points[k].y, points[k].x), matColorHSV.at<Vec3b>(0, iColorMaj), NORM_L2));
				int threshold = (int)(norm(matColorHSV.at<Vec3b>(0, r), matColorHSV.at<Vec3b>(0, c-1), NORM_L2));
				if ((norm1 < norm2) && (norm1*norm1 < threshold*threshold/25))
				{
					iColorMaj = r; 
				}
			}
			distColor.at<int>(iColorMaj, 0) += 1; // The colours n° iColourMaj scores 1 point
		}

		// Searches which colour of coloursJersey scores more points in "distColor"
		int iColorMaj = 0;
		for (int l = 0; l < c-1; l++) {
			if (distColor.at<int>(l, 0) > distColor.at<int>(iColorMaj, 0)) {
				iColorMaj = l;
			}
		}
		if (distColor.at<int>(iColorMaj, 0) < (t / param.proportioncolour)) { 
			iColorMaj =  - 1; // If the number of pixels (score) attributed to the colour is yet too low, we can't attribute a colour to the rectangle
		}
		return iColorMaj;
	}
}

//Detect the colour of a rectangle, if we don't know the "blob" (list of Point) corresponding
int detect_colour(const Mat &frame, const Rect &rectangle, const vector<Vec3b> &colorsJersey, DetectionParam param)
{
	int c = colorsJersey.size();
	vector<Vec3b> colors(colorsJersey);
	ColoredRectangle colored;
	colored = colored.create_Colored_Rectangle(c);
	// Computes the mean colour of the rectangle and creates a blob with all the points of the rectangle
	Mat Mean, CroppedMean;
	frame.convertTo(Mean, CV_32F);
	CroppedMean = Mean(rectangle);
	Scalar meanColor = mean(CroppedMean);
	for (int x = rectangle.x; x < rectangle.x + rectangle.width ; x++) {
		for (int y = rectangle.y; y < rectangle.y + rectangle.height; y++) {
			colored.blob.push_back(Point(x, y));

		}
	}
	Vec3b meanColorVec3b = Vec3b(meanColor[0], meanColor[1], meanColor[2]);
	colors[c - 1] = meanColorVec3b;
	// Calls detect_colour on the blob created, with appropriate parameters
	param.proportioncolour = 1000;
	int iColorMaj = detect_colour(frame, colored, colors, param);
	return iColorMaj;
}

// Transforms a list of list of ColoredRectangle in a list of list containing only the field "rect" of these ColoredRectangle
vector<vector<Rect>> get_rectangles(vector<vector<ColoredRectangle>> &colored){
	vector<vector<Rect>> res;
	for (auto frame : colored){
		vector<Rect> frame_rectangles;
		for (auto rectangle : frame){
			frame_rectangles.push_back(rectangle.rect);
		}
		res.push_back(frame_rectangles);
	}
	return res;
}


// void record_hog_rectangles(string video_file_path, vector<vector<Rect>> &hog_frame_rectangles, int percent)
// {

// 	float weight_threshold = 0.0;

// 	int frame_index = 0;
// 	VideoCapture video(video_file_path);

// 	// Check if camera opened successfully
// 	if (!video.isOpened())
// 	{
// 		cout << "Error opening video stream or file" << endl;
// 	};
// 	Mat frame;
// 	video >> frame;

// 	// Pedestrian recordor
// 	HOGDescriptor hog;
// 	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

// 	while (1)
// 	{

// 		// Capture frame-by-frame
// 		video.read(frame);
// 		// cout << "frame index : " << frame_index << endl;

// 		// If the frame is empty, break immediately
// 		if (frame.empty())
// 		{
// 			cout << "Coudl not read frame " << frame_index << endl;
// 			break;
// 		}

// 		vector<Rect> recordion_rectangles;
// 		vector<double> weights;

// 		Mat gray_frame, gray_frame_preprocessed;
// 		cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
// 		BrightnessAndContrastAuto(gray_frame, gray_frame_preprocessed, percent);
// 		hog.detectMultiScale(gray_frame_preprocessed, recordion_rectangles, weights);
// 		int hog_rectangle_count = recordion_rectangles.size();
// 		// cout << "recordion complete, number of reactangles deteceted : " << hog_rectangle_count << endl;

// 		hog_frame_rectangles.push_back(recordion_rectangles);
// 		/// draw recordions
// 		for (size_t i = 0; i < hog_rectangle_count; i++)
// 		{
// 			if (weights[i] < weight_threshold)
// 				continue;
// 			rectangle(gray_frame, recordion_rectangles[i], cv::Scalar(0, 0, 255), 3);
// 		}

// 		// Display the resulting frame
// 		imshow("Frame ", gray_frame);

// 		// Press ESC to stop
// 		if (waitKey(1) == 27)
// 			break;

// 		frame_index++;
// 	}
// }

// void BrightnessAndContrastAuto(const cv::Mat &src, cv::Mat &dst, float clipHistPercent = 0)
// {

// 	Assert(clipHistPercent >= 0);
// 	Assert((src.type() == CV_8UC1) || (src.type() == CV_8UC3) || (src.type() == CV_8UC4));

// 	int histSize = 256;
// 	float alpha, beta;
// 	double minGray = 0, maxGray = 0;

// 	//to calculate grayscale histogram
// 	cv::Mat gray;
// 	if (src.type() == CV_8UC1) gray = src;
// 	else if (src.type() == CV_8UC3) cvtColor(src, gray, COLOR_BGR2GRAY);
// 	else if (src.type() == CV_8UC4) cvtColor(src, gray, COLORBGRA2GRAY);
// 	if (clipHistPercent == 0)
// 	{
// 		// keep full available range
// 		cv::minMaxLoc(gray, &minGray, &maxGray);
// 	}
// 	else
// 	{
// 		cv::Mat hist; //the grayscale histogram

// 		float range[] = { 0, 256 };
// 		const float* histRange = { range };
// 		bool uniform = true;
// 		bool accumulate = false;
// 		calcHist(&gray, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

// 		// calculate cumulative distribution from the histogram
// 		std::vector<float> accumulator(histSize);
// 		accumulator[0] = hist.at<float>(0);
// 		for (int i = 1; i < histSize; i++)
// 		{
// 			accumulator[i] = accumulator[i - 1] + hist.at<float>(i);
// 		}

// 		// locate points that cuts at required value
// 		float max = accumulator.back();
// 		clipHistPercent *= (max / 100.0); //make percent as absolute
// 		clipHistPercent /= 2.0; // left and right wings
// 		// locate left cut
// 		minGray = 0;
// 		while (accumulator[minGray] < clipHistPercent)
// 			minGray++;

// 		// locate right cut
// 		maxGray = histSize - 1;
// 		while (accumulator[maxGray] >= (max - clipHistPercent))
// 			maxGray--;
// 	}

// 	// current range
// 	float inputRange = maxGray - minGray;

// 	alpha = (histSize - 1) / inputRange;   // alpha expands current range to histsize range
// 	beta = -minGray * alpha;             // beta shifts current range so that minGray will go to 0

// 	// Apply brightness and contrast normalization
// 	// convertTo operates with saurate_cast
// 	src.convertTo(dst, -1, alpha, beta);

// 	// restore alpha channel from source
// 	if (dst.type() == CV_8UC4)
// 	{
// 		int from_to[] = { 3, 3 };
// 		cv::mixChannels(&src, 4, &dst, 1, from_to, 1);
// 	}
// 	return;
// }

//void add_trackers(vector<Rect> &detected_rectangles, vector<Rect> &matched_rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame)
//{
//for (auto iterator = detected_rectangles.begin(); iterator != detected_rectangles.end(); iterator++)
//{
//Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
//tracker->init(frame, *(iterator));
//if ( player_trackers.size() == 0)
//{
//cout << "no matched rectangles" << endl;
//player_trackers.push_back(tracker);
//matched_rectangles.push_back(*(iterator));
//cout << "Matched rectangles count : " << matched_rectangles.size() << endl;
//cout << "detected rectangles count : " << detected_rectangles.size() << endl;			return;
//}
//if (!overlap(*(iterator), matched_rectangles))
//{
//Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
//tracker->init(frame, *(iterator));
//player_trackers.push_back(tracker);
//matched_rectangles.push_back(*(iterator));
//}

//}
//}


