#include "calibration.h"
#include "detection.h"
#include "image.h"

// Plot player points on top view and export 
Image<Vec3b> video_homography(string video_file_path, vector<vector<Rect>> &tracking_rectangles, void *data, DetectionParam param)
{
	Input *input = (Input *)data;
	Mat homography_matrix = input->homography_matrix;
	Image<Vec3b> target_image = input->target_image;
	Image<Vec3b> cumulated_positions (input->target_image.clone());

	
	// Load video and initialize
	VideoCapture video(video_file_path);
	auto tracking_rectangles_iterator = tracking_rectangles.begin();
	vector<Rect> frame_tracking_rectangles = *(tracking_rectangles_iterator);
	// Check if camera opened successfully
	if (!video.isOpened())
		cout << "Error opening video stream or file" << endl;
	cout << "Video loaded" << endl;
	Mat frame;
	video >> frame;
	int frame_index = 0;

	// Main loop : iterate over the input video
	while (1)
	{
		if (frame.empty())
		{
			cout << "Could not read frame " << frame_index << endl;
			break;
		};

		Image<Vec3b> source_image(frame);
		Image<Vec3b> frame_target_image = (Image<Vec3b>)target_image.clone();
		vector<Rect> frame_tracking_rectangles = *(tracking_rectangles_iterator);

		// Plot points on both source and target images
		for (int rectangle_index = 0; rectangle_index < frame_tracking_rectangles.size(); rectangle_index++)
		{	
			// Rectangle bottom coordinates
			Rect player_rectangle = frame_tracking_rectangles[rectangle_index];
			float x = player_rectangle.x + player_rectangle.width / 2;
			float y = player_rectangle.y + player_rectangle.height;
			Point point(x, y);

			int colour_index = detect_colour(source_image, player_rectangle, input->colours, param);

            // If detected colour does not match any of the selected colours, -1 and returned and no point is plotted
            if (colour_index < 0) continue; 
            Vec3b colour = input->colours[colour_index   ];
			circle(frame, point, 2, colour, 2);
			Point target_point = homographic_transformation(homography_matrix, point);
			circle(frame_target_image, target_point, 2, colour, 2);
			circle(cumulated_positions, target_point, 2, colour, 2);
		}
		imshow("top view", frame_target_image);

		// Increment
		tracking_rectangles_iterator++;
		frame_index += 1;
		video.read(frame);
		
		waitKey();
		// Press ESC to stop
		if (waitKey(1) == 27)
			break;

	}
	video.release();
    return cumulated_positions;
}
