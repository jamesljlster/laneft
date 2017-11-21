#include <iostream>
#include <opencv2/opencv.hpp>

#include <laneft_ocv.hpp>

using namespace std;
using namespace cv;

void draw_ctrl_simulation(Mat& img, int x, int y);

int main(int argc, char* argv[])
{
	// Checking
	if(argc <= 1)
	{
		cout << "Pass image to run the test" << endl;
		return -1;
	}

	// Loop for testing
	for(int i = 1; i < argc; i++)
	{
		// Load image
		Mat src = imread(argv[i], CV_LOAD_IMAGE_COLOR);
		if(src.empty())
		{
			cout << "Failed to load " << argv[i] << endl;
			return -1;
		}

		// Process canny
		Mat canny;
		Canny(src, canny, 255, 255);

		// Draw control simulation
		laneft_ocv laneFt;
        double ctrlFeature = laneFt.get_feature(canny);
        laneFt.draw_line_onto(src);
        draw_ctrl_simulation(src, src.cols / 2 - ctrlFeature, src.rows / 4 * 3);

		// Show result
		imshow("Result", src);
		waitKey(0);
	}

	return 0;
}

void draw_ctrl_simulation(Mat& img, int x, int y)
{
    // Draw vertical line
    line(img, Point(img.cols / 2, 0), Point(img.cols / 2, img.rows), Scalar(0, 0, 255), 2);

    // Draw control point
	if(x >= img.cols)
	{
		x = img.cols - 1;
	}
	if(x < 0)
	{
		x = 0;
	}

    circle(img, Point(x, y), 5, Scalar(0, 0, 255), 2);
}

