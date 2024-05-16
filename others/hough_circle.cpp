
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

#include <opencv2/core/version.hpp>
#define OPENCV_VERSION CVAUX_STR(CV_VERSION_MAJOR) CVAUX_STR(CV_VERSION_MINOR) CVAUX_STR(CV_VERSION_REVISION)
#define LINK_TO_OPENCV(libname) __pragma(comment(lib, "opencv_" #libname OPENCV_VERSION) ".lib")
#define LINK_TO_OPENCV_DEBUG(libname) __pragma(comment(lib, "opencv_" #libname OPENCV_VERSION "d.lib"))
#ifdef _DEBUG
LINK_TO_OPENCV_DEBUG(world)
#else
LINK_TO_OPENCV(world)
#endif


using namespace cv;
using namespace std;

Mat src, src_gray;
Mat cannyedges;
vector<Vec3f> circles;


const char* window_name_original = "HoughLines_Canny";
const char* window_name_canny = "Canny";


int main(int argc, char** argv)
{
	CommandLineParser parser(argc, argv, "{@input | HoughCircles.jpg | input image}");
	src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_COLOR); // Load an image

	if (src.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
		return -1;
	}

	src_clone = src.clone();

	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	
	namedWindow(window_name_original, WINDOW_AUTOSIZE);
	namedWindow(window_name_canny, WINDOW_AUTOSIZE);

	
	
	//Canny
	Canny(src_gray, cannyedges, 40, 50);
	
	HoughCircles(src_gray, circles, HOUGH_GRADIENT, 2, src_gray.rows / 4, 200, 100);

	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// draw the circle center
		circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// draw the circle outline
		circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}

	imshow(window_name_original, src);
	imshow(window_name_canny, cannyedges);



	waitKey(0);

	return 0;
}