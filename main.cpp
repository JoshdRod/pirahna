#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

using namespace cv;
int main()
{
    Mat frame;
    VideoCapture cap;

    int deviceID = 2; // Select the ID of the USB camera we want - 0 = default
    int apiID = CAP_V4L2; // ??

    cap.open(deviceID, apiID); // Opens selected cam using selected API
    
    // Error if fail to open cam
    if (!cap.isOpened()) {
	    std::cerr << "ERROR! Unable to open camera\n";
	    return -1;
    }
    std::cout << "Sleep" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Starting to grab (??)" << std::endl;

    while (true)
    {
	    cap.read(frame); // I think this reads *into* the frame?
	    // Break on err
	    if (frame.empty())
	    {
		    std::cerr << "ERROR! Blank frame grabbed\n";
		    break;
	    }
	    // Show image
	    imshow("Live", frame);

	    // Create array of double size
	    Mat upscaledImage;

	    // For each pixel in original array
	    for (int y; y < frame.rows; y++)
	    {
		    for (int x; x < frame.cols; x++)
         	    {
			    // Add to new array
			    upscaledImage.at<Vec3b>(Point(x*2,y*2)) = frame.at<Vec3b>(Point(x,y));

			    // Generate new pixels from pixels around it
  			    	// 1. Find pixels around new pixel -> nested array (tuple?)
				// above / below
				Vec3b left = frame.at<Vec3b((Point(x,y));
				Vec3b right = frame.at<Vec3b((Point(x+1,y));
				Vec3b up = frame.at<Vec3b((Point(x,y-1));
				Vec3b down = frame.at<Vec3b((Point(x,y+1));

				// 2. Calculate value of new pixels
				// RIGHT
				Vec3b rightPixel;
				for (int i = 0; i < 3; i++)
				{
					// Weight left + right pixels
					rightPixel[i] = std::mean(left[i], left[i], right[i], right[i],  up[i], down[i]); // What's the mean func called??
				}

				// BOTTOM
				Vec3b bottomPixel;
				for (int i = 0; i < 3; i++)
				{
					// Weight left + bottom pixels
					bottomPixel[i] = std::mean(left[i], left[i], right[i],  up[i], down[i], down[i]);
				}

				// BOTTOM RIGHT
				Vec3b bottomRightPixel;
				for (int i = 0; i < 3; i++)
				{
					// No weighting
					bottomRightPixel[i] = std::mean(left[i], right[i],  up[i], down[i]);
				}
				// Add new pixel to arrray
		  }
	    }

	    // Break if key pressed (??)
	    if (waitKey(5) >= 0)
		    break;
    }
    return 0;
}
