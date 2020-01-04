# Automatic sport scene modelling

## Demo
[Demo video on YouTube](https://www.youtube.com/watch?v=OM7V0shw3fI)

## Input 
Sport scene video, with fixed camera
![input](demo/input.png)
## Output
![ouput](demo/output.png)

## Pipeline
* Calibration, computation of the homography matrix
* Detect players by foreground detection
* Track players choosing the best rectangles
* Project players on top view image by applying homography


## Run the program

```bash
mkdir build
cd build
cmake ..
./main [path to video file] [path to top view image]
```

Once the first frame of the output is displayed, press any key to move on.
