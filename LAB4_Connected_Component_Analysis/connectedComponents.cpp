#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

#include <opencv2/calib3d.hpp>


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

Mat src;
Mat src_binary;
int threshval = 50;
vector<Point2f> approx_all;

Mat GetARTag(Mat in_img, int input_p_0, int input_p_1, int input_p_2, int input_p_3)
{
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	obj.push_back(approx_all.at(input_p_0));
	obj.push_back(approx_all.at(input_p_1));
	obj.push_back(approx_all.at(input_p_2));
	obj.push_back(approx_all.at(input_p_3));

	scene.push_back(Point2f(0, 0));
	scene.push_back(Point2f(47, 0));
	scene.push_back(Point2f(0, 47));
	scene.push_back(Point2f(47, 47));
	Mat H = findHomography(obj, scene);

	Mat warpedImage;
	warpPerspective(in_img, warpedImage, H, Size(48, 48));



	return warpedImage;
}
static void on_trackbar(int, void*)
{
	Mat bw;
	threshold(src, bw, threshval, 255, cv::THRESH_BINARY_INV);
	Mat labelImage(src.size(), CV_32S);
	int nLabels = connectedComponents(bw, labelImage, 8);

	std::vector<Vec3b> colors(nLabels);
	colors[0] = Vec3b(0, 0, 0);//background
	for (int label = 1; label < nLabels; ++label) {
		colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}
	Mat dst(src.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; ++r) {
		for (int c = 0; c < dst.cols; ++c) {
			int label = labelImage.	at<int>(r, c);
			Vec3b& pixel = dst.at<Vec3b>(r, c);
			//cout << label << endl;
			pixel = colors[label];
			
		}
	}
	
	//imshow("Connected Components", dst);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(bw, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// Iterate through each contour

	Mat out(src.size(), CV_8UC3);
	 //Iterate through each contour

	// 遍歷每個輪廓
	int pointCounter = 0;
	for (const auto& contour : contours) {
		// 找到輪廓的頂點
		std::vector<Point2f> approx;
		approxPolyDP(contour, approx, 0.01 * arcLength(contour, true), true);
		approx_all.insert(approx_all.end(), approx.begin(), approx.end());
		// 繪製頂點
		for (const auto& point : approx) {
			circle(out, point, 5, Scalar(0, 255, 0), -1);

			// 在頂點上標記數字
			std::stringstream ss;
			ss << pointCounter++;
			putText(out, ss.str(), point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
		}
	}

	//imshow("Result", out);
	Mat o0, o1, o2, o3, o4, o5, o6, o7, o8, o9, o10, o11;
	o0 = GetARTag(dst, 116, 115, 117, 118);
	o1 = GetARTag(dst, 128, 133, 129, 130);
	o2 = GetARTag(dst, 136, 139, 137, 138);
	o3 = GetARTag(dst, 86, 85, 87, 88);
	o4 = GetARTag(dst, 89, 92, 90, 91);
	o5 = GetARTag(dst, 93, 96, 94, 95);
	o6 = GetARTag(dst, 50, 49, 51, 52);
	o7 = GetARTag(dst, 53, 56, 54, 55);
	o8 = GetARTag(dst, 58, 57, 59, 60);
	o9 = GetARTag(dst, 19, 22, 20, 21);
	o10 = GetARTag(dst, 24, 23, 25, 26);
	o11 = GetARTag(dst, 28, 27, 29, 30);

	imshow("Warped Image_0", o0);
	imshow("Warped Image_1", o1);
	imshow("Warped Image_2", o2);
	imshow("Warped Image_3", o3);
	imshow("Warped Image_4", o4);
	imshow("Warped Image_5", o5);
	imshow("Warped Image_6", o6);
	imshow("Warped Image_07", o7);
	imshow("Warped Image_08", o8);
	imshow("Warped Image_09", o9);
	imshow("Warped Image_010", o10);
	imshow("Warped Image_011", o11);

}


int main(int argc, char** argv)
{
	//read image
	CommandLineParser parser(argc, argv, "{@input | 240225_21.jpg | input image}");
	src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_GRAYSCALE); // Load an image

	//check
	if (src.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
		return -1;
	}
	

	//namedWindow("Connected Components", WINDOW_AUTOSIZE);
	//createTrackbar("Threshold", "Connected Components", &threshval, 255, on_trackbar);
	on_trackbar(threshval, 0);
	
	//show originalimage
	//imshow("original", src);

	//esc to escape
	char key = waitKey(0);
	if (key == 27) destroyAllWindows();

	return 0;
}
