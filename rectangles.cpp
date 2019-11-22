#include "rectangles.h"

float AREA_OVERLAP_THRESHOLD = 0.5;

bool overlap(Rect new_rectangle, vector<Rect> tracking_rectangles){

  	int tracked_players_count = tracking_rectangles.size();
    for (int player_index = 0; player_index < tracked_players_count; player_index++)
		{
        Rect player_rectangle = tracking_rectangles[player_index];
        int min_area = min(new_rectangle.area(), player_rectangle.area());
        int intersection_area = (player_rectangle & new_rectangle).area();
        if  (intersection_area > min_area *  AREA_OVERLAP_THRESHOLD) return true;			
		}
    return false;
}

