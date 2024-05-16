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

Mat src, src_clone;
Mat M_cropImg;
bool left_flag = 0;
int pointx, pointy;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		left_flag = 1;
		pointx = x;
		pointy = y;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
		Point p1(pointx, pointy);
		Point p2(x, y);
		if (left_flag) {
			src_clone = src.clone();
			rectangle(src_clone, p1, p2, Scalar(255, 255, 255), 2, LINE_8);
		}
		imshow("MouseEventHandler & Image ROI", src_clone);
	}
	else if (event == EVENT_LBUTTONUP)
	{
		cout << "Left button of the mouse is up - position (" << x << ", " << y << ")" << endl;
		/*pointx -> mousedown_x*/
		/*pointy -> mousedown_y*/
		/*x -> mouseup_x*/
		/*y -> mouseup_x*/
		// p1 is leftup & p2 is rightdown
		Point p1(pointx, pointy); // leftup
		Point p2(x, y); // rightdown
		int tmp = 0;
		if (x >= src.cols || x <= 0 || y >= src.rows || y <= 0 || pointx == x || pointy == y)
		{
			//¨¾§b fool proofing
			left_flag = 0;
			return;
		}
		else
		{
			if (pointx > x && pointy > y) // p1 is rightdown & p2 is leftup
			{
				tmp = p1.x;
				p1.x = x;
				p2.x = tmp;

				tmp = p1.y;
				p1.y = y;
				p2.y = tmp;
			}
			else if (pointx > x && pointy < y) // p1 is rightup & p2 is leftdown
			{
				tmp = p1.x;
				p1.x = p2.x;
				p2.x = tmp;
			}
			else if (pointx < x && pointy > y) // p1 is leftdown & p2 is rightup
			{
				tmp = p1.y;
				p1.y = p2.y;
				p2.y = tmp;
			}
			rectangle(src_clone, p1, p2, Scalar(255, 255, 255), 2, LINE_8);
			M_cropImg = src(Range(p1.y, p2.y), Range(p1.x, p2.x));
			//cout << p1 << " " << p2 << endl;
			GaussianBlur(M_cropImg, M_cropImg, Size(3, 3), 0);
			M_cropImg.copyTo(src_clone(Rect(p1.x, p1.y, M_cropImg.cols, M_cropImg.rows)));

			imshow("MouseEventHandler & Image ROI", src_clone);
			left_flag = 0;
		}
	}
}

int main(int argc, char** argv)
{
	//read image
	CommandLineParser parser(argc, argv, "{@input | 1409756677059_wps_58_Man_Standing_in_Front_of_.jpg | input image}");
	src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_COLOR); // Load an image

	//check
	if (src.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
		return -1;
	}
	src_clone = src.clone();
	
	//create window and set mouse
	namedWindow("MouseEventHandler & Image ROI", WINDOW_AUTOSIZE);
	setMouseCallback("MouseEventHandler & Image ROI", CallBackFunc, NULL);
	
	//show image
	imshow("MouseEventHandler & Image ROI", src);
	
	//esc to escape
	char key = waitKey(0);
	if (key == 27) destroyAllWindows();

	return 0;
}