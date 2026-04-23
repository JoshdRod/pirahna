#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <map>

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
				Vec3b centrePixel = frame.at<Vec3b>(Point(x, y));
				map<string, Vec3b> surroundingPixels = generateSurroundingPixelMap(frame, centrePixel);

				// 2. Calculate value of new pixels
				generateBottomRightPixels(upscaledImage, surroundingPixels, Point(x, y), 3);
	    }

	    // Break if key pressed (??)
	    if (waitKey(5) >= 0)
		    break;
    }
    return 0;
}

// Generates a map of pixels surrounding the centre.
// If pixel does not exist (e.g a surrounding pixel on the corner), Map stores null in that place
// INPUTS: Frame to take pixels from, Point location of centre pixel
// RETURNS: map<string, Pixel> of surrounding pixels ({"left": Vec3b, ..})
map<string, Vec3b> generateSurroundingPixelMap(Mat frame, Point centrePixel)
{
	map<string, Vec3b> surroundingPixelMap;
	surroundingPixelMap.insert("centre", frame.at<Vec3b>(centrePixel));

	map<string, int[2]> directions = {{"left", [-1, 0]}, {"right", [1, 0]}, {"up", [0, -1]}, {"down", [0, 1]}};
	for (auto direction : directions)
	{
		string name = direction.first;
		int offset[2] = direction.second;
		try
		{
			surroundingPixelMap.insert(name, frame.at<Vec3b>((centrePixel.x + offset[0], centrePixel.y + offset[1])));
		}
		catch
		{
			surroundingPixelMap.insert(name, null);
		}
	}
	return surroundingPixelMap;
}


// Generates pixels to fill the bottom right of the centre pixel, then places them on the upscaled image frame
// Generates enough pixels to satify the given scale factor
// INPUTS: frame of upscaled image, map<string, Vec3b> of surrounding pixels,Point where centre pixel lies on original image, int scale factor
void generateBottomRightPixels(Mat upscaledFrame, map<string, Vec3b> surroundingPixelMap, Point originalCentrePoint, int scaleFactor) // TODO: Switch to centrePoint on UPSCALED image
{
	// Calculate position of where generated pixel will go
	for (int i = 0; i < scaleFactor; i++)
	{
		for (int j = 0; j < scaleFactor; j++)
		{
			// Skip 0,0 (that's the centre pixel!)
			if (i == 0 && j == 0)
				continue;
			// Calculate position old pixel will be on new image
			map<string, int> pixelPositionMap;
			for (auto pixel : surroundingPixelMap)
			{
				// Calculate pixel's position relative to centre pixel on upscaled image
				Point pixelRelativePosition = Point(0,0);
				if (pixel.first.contains("top"))
					pixelRelativePosition.y = scaleFactor;
				else if (pixel.first.contains("bottom"))
					pixelRelativePosition.y = -scaleFactor;

				if (pixel.first.contains("right"))
					pixelRelativePosition.x = scaleFactor;
				else if (pixel.first.contains("left"))
					pixelRelativePosition.x = -scaleFactor;
				pixelPositionMap.insert(pixel.first, pixelRelativePosition);
			}
			// Calculate distance between (pythag)
			// Do Ae^k(dist) here = weight
			// (Figure out A and k via ML later)
}

//// Calculates the weighted average of a pixel map
//// By default, each pixel has a weighting of 1. To modify that weighting, enter a record into the weighting map (e.g: {"right": 2} will weight the right pixel by 2x)
//// INPUTS: map<Vec3b> pixels, map<float> weightings
//// RETURNS: Vec3b average pixel
//Vec3b generateWeightedAverage(map<string, Vec3b> pixelMap, map<string, float> weights)
//{
//	Vec3b sigmaPixel;
//	float sigmaWeight = 0;
//	for (auto pixel : pixelMap)
//	{
//		string name = pixel.first;
//		Vec3b data = pixel.second;
//		// If pixel is null, skip it
//		if (data == null)
//			continue;
//
//		float weight = weights.contains(name) ? weights.at(name) : 1;
//		for (int i = 0; i < 3; i++)
//			sigmaPixel[i] += data[i] * weight;
//		sigmaWeight += weight;
//	}
//	// Return avg : sigma pixel / sigma weight
//	Vec3b averagePixel;
//	for (int i = 0; i < 3; i++)
//		averagePixel[i] = std::round(sigmaPixel[i] / sigmaWeight);
//
//	return averagePixel;
//}
