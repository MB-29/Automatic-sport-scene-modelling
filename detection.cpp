#include "detection.h"
#include "polygon.cpp"


// ColoredRectangle dans tracking.cpp
// Améliorer détection couleurs
// Taille des joueurs avec perspective
// Statistiques à faire ??

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
	//cvtColor(Moy, Moy, COLOR_BGR2HSV);


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

			// obtain input image from source
			capInit.retrieve(img, CAP_OPENNI_BGR_IMAGE);
			img.convertTo(Interm, CV_32F, 1 / (255.0 * frame_count));
			//cvtColor(Interm, Interm, COLOR_BGR2HSV);
			img.convertTo(Interm2, CV_32F);
			//cvtColor(Interm2, Interm2, COLOR_BGR2HSV);
			//cv::divide(Interm, cv::Scalar((float)frame_count, (float)frame_count, (float)frame_count), Interm, 1, CV_32F);
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

void initializeMask(Mat &foregroundMask, const Mat &frame, const Mat &Moy, float seuil)
{
	Image<Vec3f> imgFloat;
	frame.convertTo(imgFloat, CV_32F, 1 / 255.0);
	//cvtColor(imgFloat, imgFloat, COLOR_BGR2HSV);
	// waitKey();
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
	imshow("maaskMoy", foregroundMask);
}

void colorMask(const Mat &img, const Mat&foreground, std::vector<Mat> &rst, vector<Vec3b> colorsJersey) {
	
	if (!rst.empty()) {
		rst.clear();
	}
	
	Image<Vec3b> imgHSV;
	int c = colorsJersey.size();
	Mat_<Vec3b> matColorBGR(1, colorsJersey.size());
	Mat_<Vec3b> matColorHSV(1, colorsJersey.size());

	for (int r = 0; r < c; r++) {
		matColorBGR.at<Vec3b>(0, r) = colorsJersey[r] ;
	}

	cvtColor(matColorBGR, matColorHSV, COLOR_BGR2HSV);
	cvtColor(img, imgHSV, COLOR_BGR2HSV);
	int n = imgHSV.cols;
	int m = imgHSV.rows;

	for (int r = 0; r < c; r++) {
		Mat maskUnit;
		maskUnit.create(img.size(), CV_8U);
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				if (foreground.at<uchar>(i, j) == 255) {
					
					maskUnit.at<uchar>(i, j) += (int)(norm(imgHSV.at<Vec3b>(i, j), matColorHSV.at<Vec3b>(0, r), NORM_L2));
				}
				else {
					maskUnit.at<uchar>(i, j) = 0;
				}

			}
		}
		rst.push_back(maskUnit);
	}
}

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
				// loDiff (maximal lower diff to connect), upDiff (maximal upper diff to connect), $rect : output minimal bounding rectangle
				// last arguments : 4 if only 4 neighbours checked ; 8 is 8 of them
				std::vector<Point> blob;
				rectangle.rect = rectToFill;

				if (filter_rectangles(rectangle.rect, pitch)==true) {
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


					if (((param.blobFlag) && (blob.size() > param.sizeMinBlob)) || (!param.blobFlag)) {
						if ((rectangle.rect.height > param.sizeMinRect) && (rectangle.rect.height < param.sizeMaxRect)) {
							rectangle.blob = blob;
							int icolour = detect_colour(frame, rectangle.rect, colorsJerseys, param);
							rectangle.colors[icolour] = 1;
							if (icolour == (c - 1)) {
								cout << colorsJerseys[icolour] << endl;
							}
							if (icolour != (c - 1)) { //Cette condition ne marche pas, la couleur bleu sombre est trop proche de la couleur moyenne du background
								rectangles.push_back(rectangle);
							}
						}
					}
				}

				label_count++;
				label_image.copyTo(label_image_old);
			}
		}
	}
}

void record_backgroundsubstract_rectangles(string video_file_path, vector<vector<ColoredRectangle>> &frame_rectangles, DetectionParam param, Input input)
{

	// Init background substractor

	//Ptr<BackgroundSubtractorMOG2> bg_model = createBackgroundSubtractorMOG2(history, 16, true).dynamicCast<BackgroundSubtractorMOG2>();
	// Adjust automatically the number of K gaussian for the background mixture, history of (1) frames, threshold (2), detect Shaddows in grey (3) (mouaif)
	Ptr<BackgroundSubtractorKNN> bg_model = createBackgroundSubtractorKNN(param.history, 400, true).dynamicCast<BackgroundSubtractorKNN>();
	// history of (1) frames (too much is worst), threshold (2), detect Shaddows in grey (3) (mouaif). Better, doesn't get excited that much at frames 40-50

	// Create empty input img, foreground and background image and foreground mask.
	Mat img, foregroundMask, backgroundImage, foregroundImg;
	//vector<Mat> clrMasks;

	Image<Vec3f> Moy;

	Scalar meanColor = moyenneMask(Moy, video_file_path);

	Vec3b meanColorVec3b = Vec3b(meanColor[0], meanColor[1], meanColor[2]); // Attention, c'est en HSV !!

	//Mat_<Vec3b> matColorBGR(1, 1);
	//Mat_<Vec3b> matColorHSV(1, 1);
	//matColorHSV.at<Vec3b>(0, 0) = meanColorVec3b;
	cout << "Mean colour of the background" << meanColorVec3b << endl;
	//cvtColor(matColorHSV, matColorBGR, COLOR_HSV2BGR);

	// The last color of this list is the color "of the background"
	input.colours.push_back(meanColorVec3b);


	// capture video from source 0, which is web camera, If you want capture video from file just replace //by �VideoCapture cap("videoFile.mov")
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
		cout << "frame index = " << frame_index << endl;
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

		//bg_model->apply(img, foregroundMask, 0.1);

		if ((frame_index < 10) && (param.threshold > 0))
		{
			foregroundMask.create(img.size(), CV_8U);
			initializeMask(foregroundMask, img, Moy, param.threshold);
		}


	
		//colorMask(img, foregroundMask, clrMasks, colorsJerseys);

	
		// smooth the mask to reduce noise in image
		//GaussianBlur(foregroundMask, foregroundMask, Size(11, 11), 3.5, 3.5);

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
		

		cv::Mat binary;
		labelBlobs(foregroundMask, img, rects, param, input.colours, input.pitch);


		Mat foregroundImgWithRect;

		foregroundImg.copyTo(foregroundImgWithRect);

		frame_rectangles.push_back(rects);


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
		//cout << "frame number" << endl;
		//cout << cap.get(CAP_PROP_POS_FRAMES) << endl;
		//cout << "Nsamples" << endl;
		//cout << bg_model->getNSamples() <<endl;
		//cout << "k" << endl;
		//cout << bg_model->getkNNSamples() << endl;
		//cout << bg_model->getNMixtures() << endl;;
		imshow("real image", img);
		imshow("foreground mask", foregroundMask);
		imshow("foreground image", foregroundImgWithRect);

		// waitKey();
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

bool filter_rectangles(Rect rectangle, Point pitch[])
{
	//vector<vector<Rect>> filtered;
	//for (int frame_index = 0; frame_index < detection_rectangles.size(); frame_index++)
	//{
		//cout << " frame index = " << frame_index << endl;
		//int in_rectangles_count = 0;
		//vector<Rect> frame_rectangles = detection_rectangles[frame_index], frame_filtered_rectangles;
		//for (int rectangle_index = 0; rectangle_index < frame_rectangles.size(); rectangle_index++)
		//{
			//Rect rectangle = frame_rectangles[rectangle_index];
			float x = rectangle.x, y = rectangle.y, w = rectangle.width, h = rectangle.height;
			Point corners[4] = {Point(x, y), Point(x + w, y), Point(x, y + h), Point(x + w, y + h)};
			bool out = true;
			for (int corner_index = 0; corner_index < 4; corner_index++)
			{
				Point point = corners[corner_index];
				if (isInside(pitch, 4, point))
					out = false;
			}
			//if (!out)
			//{
				//frame_filtered_rectangles.push_back(rectangle);
				//in_rectangles_count ++;
			//}
		//}
		//filtered.push_back(frame_filtered_rectangles);
		//cout << "Kept " << in_rectangles_count << " rectangles out of " << frame_rectangles.size() << endl;
	//}
	return !out;
}


int detect_colour(const Mat &frame, const ColoredRectangle &rectangle, vector<Vec3b> colorsJersey, DetectionParam param)
{
	if (rectangle.colors.size() != colorsJersey.size()) {
		cout << "Error : different numbers of jersey colours" << endl;
	}
	if (rectangle.blob.size() == 0) {
		int iColour = detect_colour(frame, rectangle.rect, colorsJersey, param);
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
		Mat_<int> distColor(t, 1, CV_32FC1);
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

		//for (int r = 0; r < c; r++) {
			//for (int k = 0; k < t; k++) {
				//distColor.at<int>(r, 0) += (int)(norm(imgHSV.at<Vec3b>(points[k].y, points[k].x), matColorHSV.at<Vec3b>(0, r), NORM_L2));
				//distance_1 += norm(pixel_colour - jersey_colour_1);
			//}
		//}
		for (int k = 0; k < t; k++) {
			int iColorMaj = 0;
			for (int r = 0; r < c - 1; r++) {
				int norm1 = (int)(norm(imgHSV.at<Vec3b>(points[k].y, points[k].x), matColorHSV.at<Vec3b>(0, r), NORM_L2));
				int norm2 = (int)(norm(imgHSV.at<Vec3b>(points[k].y, points[k].x), matColorHSV.at<Vec3b>(0, iColorMaj), NORM_L2));
				if ((norm1 < norm2) && (norm1 < param.thresholdcolour))
				{
					iColorMaj = r;
					//distance_1 += norm(pixel_colour - jersey_colour_1);
				}
			}
			distColor.at<int>(iColorMaj, 0) += 1;
		}


		int iColorMaj = c - 1;
		for (int l = 0; l < c; l++) {
			if (distColor.at<int>(l, 0) > distColor.at<int>(iColorMaj, 0)) {
				iColorMaj = l;
			}
		}
		cout << "dist" << distColor.at<int>(iColorMaj, 0) << "et" << t << endl;
		if (distColor.at<int>(iColorMaj, 0) < (t / param.proportioncolour)) { // Mais si ça ne représente pas au moins 1/2 (limite arbitraire aussi...) des points du blob...
			iColorMaj = c - 1;
		}
		return iColorMaj;
	}
}

int detect_colour(const Mat &frame, const Rect &rectangle, vector<Vec3b> colorsJersey, DetectionParam param)
{
	int c = colorsJersey.size();
	ColoredRectangle colored;
	colored = colored.create_Colored_Rectangle(c);
	for (int x = rectangle.x; x < rectangle.x + rectangle.width ; x++) {
		for (int y = rectangle.y; y < rectangle.y + rectangle.height; y++) {
			colored.blob.push_back(Point(x, y));
		}
	}
	param.proportioncolour = 4;
	param.thresholdcolour = 200;
	int iColorMaj = detect_colour(frame, colored, colorsJersey, param);
	return iColorMaj;
}

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

// void add_trackers(vector<Rect> &hog_rectangles, vector<Rect> &tracking_rectangles, vector<Ptr<TrackerCSRT>> player_trackers){
// 	int hog_rectangles_count = hog_rectangles.size();
// 	for (int rectangle_index = 0; rectangle_index < hog_rectangles_count; rectangle_index++)
// 		{
// 			Rect new_rectangle = hog_rectangles[rectangle_index];
// 			if (overlap(new_rectangle, tracking_rectangles) == true) continue;

// 			tracking_rectangles.push_back(new_rectangle);
// 			Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
// 			tracker->init(frame, *(iterator));
// 			player_trackers

// Create a mask image for drawing purposes
//Mat mask = Mat::zeros(old_frame.size(), old_frame.type());

//setMouseCallback("source", add_point_source, &input);
// 		}
// 	return;
// }

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

// int main() {
// 	string method;
// 	vector<vector<Rect>> frame_rectangles;
// 	// // cout << "Method ?";
// 	// cin >> method;
// 	if (method == "HOG") {
// 		// int percent;
// 		// cout << "percent ";
// 		// cin >> percent;
// 		// record_hog_rectangles(VIDEO_FILE_PATH, frame_rectangles, percent);
// 		return 0;
// 	}
// 	else {
// 		int history = 30, sizeMinRect = 10, gaussianSize = 7;
// 		string technic = "a";
// 		// cout << "history ";
// 		// cin >> history;
// 		// cout << "sizeMinRect";
// 		// cin >> sizeMinRect;
// 		// cout << "technic";
// 		// cin >> technic;
// 		// cout << "gaussianSize";
// 		// cin >> gaussianSize;
// 		record_backgroundsubstract_rectangles(VIDEO_FILE_PATH, frame_rectangles, technic, history, sizeMinRect, gaussianSize);
// 	}
// }