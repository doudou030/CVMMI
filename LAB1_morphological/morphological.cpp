
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

Mat src, src_gray;
Mat dst, detected_edges;
Mat dst_closing, dst_tmp, dilation_dst;
const char* window_name = "Connect Map";


int main(int argc, char** argv)
{ 
	CommandLineParser parser(argc, argv, "{@input | adaptiveth.png | input image}");
	src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_COLOR); // Load an image

	if (src.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
		return -1;
	}

	dst.create(src.size(), src.type());

	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	namedWindow(window_name, WINDOW_AUTOSIZE);

	adaptiveThreshold(src_gray, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 25, 10);
	
	Mat element = getStructuringElement(MORPH_CROSS, Size(10, 2));
	
	int iterations = 2;
	morphologyEx(dst, dst_closing, MORPH_OPEN, element, Point(-1, -1), iterations);
	
	imshow(window_name, dst_closing);

	waitKey(0);

	return 0;
}