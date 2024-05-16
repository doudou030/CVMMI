#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
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

void main()
{
	Mat img(480, 640, CV_8UC1);
	img.setTo(128);
	RNG rng;

	for (int r = 0; r < img.rows; r++) {
		uchar* row = img.ptr<uchar>(r);
		for (int c = 0; c < img.cols; c++) {
			row[c] = uchar(int(row[c]) + int(rng.gaussian(16)));
		}
	}

	imshow("noise", img);
	waitKey(0);

	Mat simg, kernel;

	for (int s = 1; s < 32; s++) {
		kernel = cv::getGaussianKernel(s * 3+1, s);
		cv::sepFilter2D(img, simg, -1, kernel.t(), kernel);

		imshow("smoothed", simg);
		waitKey(100);
	}
}