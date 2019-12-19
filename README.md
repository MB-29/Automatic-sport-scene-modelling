# Automatic sport scene modelling

## Input 
Sport scene video, with fixed camera
![input](demo/input.jpg)
## Pipeline
* Calibrating : compute homography
* Detect players by foreground detection
* Track players choosing the best rectangles
* Project players on top view image by applying homography

## Output
![ouput](demo/output.jpg)