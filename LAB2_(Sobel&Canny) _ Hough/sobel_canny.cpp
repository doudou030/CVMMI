
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

Mat src, src_gray, src_clone, src_clone_gray;
Mat src_tmp, tmp;
Mat cannyedges, sobeledges;
Mat grad_x, grad_y;
Mat abs_grad_x, abs_grad_y;
vector<Vec4i> cannylines;
vector<Vec4i> sobellines;

const char* window_name_original = "HoughLines_Canny";
const char* window_name_canny = "Canny";
const char* window_name_sobel = "Sobel";
const char* window_name_original_two = "HoughLines_Sobel";

int main(int argc, char** argv)
{
	CommandLineParser parser(argc, argv, "{@input | wdg3.png | input image}");
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
	namedWindow(window_name_sobel, WINDOW_AUTOSIZE);
	namedWindow(window_name_original_two, WINDOW_AUTOSIZE);
	
	//Sobel
	Sobel(src_gray, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(src_gray, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT);

	// converting back to CV_8U
	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);

	addWeighted(abs_grad_x, 1, abs_grad_y, 1, 0, sobeledges);
	threshold(sobeledges, tmp, 60, 255, THRESH_BINARY);

	HoughLinesP(tmp, sobellines, 1, CV_PI / 180, 30, 50, 30);
	for (size_t i = 0; i < sobellines.size(); i++)
	{
		Vec4i l = sobellines[i];
		line(src_clone, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
	}

	//Canny
	Canny(src_gray, cannyedges, 40, 50);
	HoughLinesP(cannyedges, cannylines, 1, CV_PI / 180, 30, 20, 50);

	for (size_t i = 0; i < cannylines.size(); i++)
	{
		Vec4i l = cannylines[i];
		line(src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
	}

	imshow(window_name_original, src);
	imshow(window_name_canny, cannyedges);
	imshow(window_name_original_two, src_clone);
	imshow(window_name_sobel, sobeledges);
	imshow("threshold", tmp);


	waitKey(0);

	return 0;
}
