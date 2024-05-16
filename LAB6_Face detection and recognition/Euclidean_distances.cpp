#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/version.hpp>
#include <opencv2/core/base.hpp>

#include "opencv2/objdetect/charuco_detector.hpp"
#include "opencv2/imgcodecs.hpp"


#define OPENCV_VERSION CVAUX_STR(CV_VERSION_MAJOR) CVAUX_STR(CV_VERSION_MINOR) CVAUX_STR(CV_VERSION_REVISION)
#define LINK_TO_OPENCV(libname) __pragma(comment(lib, "opencv_" #libname OPENCV_VERSION) ".lib")
#define LINK_TO_OPENCV_DEBUG(libname) __pragma(comment(lib, "opencv_" #libname OPENCV_VERSION "d.lib"))
#ifdef _DEBUG
LINK_TO_OPENCV_DEBUG(world)
#else
LINK_TO_OPENCV(world)
#endif;

using namespace cv;
using namespace std;

void Compute_stereo_matching()
{
	//我的opencv沒有sfm
	//fundamentalfromprojection
}

void Generate3DPoints(vector<cv::Point3f> &output)
{
	std::vector<cv::Point3f> points;


	float x, y, z;

	x = .5; y = .5; z = -.5;
	points.push_back(cv::Point3f(x, y, z));

	x = .5; y = .5; z = .5;
	points.push_back(cv::Point3f(x, y, z));

	x = -.5; y = .5; z = .5;
	points.push_back(cv::Point3f(x, y, z));

	x = -.5; y = .5; z = -.5;
	points.push_back(cv::Point3f(x, y, z));


	for (unsigned int i = 0; i < points.size(); ++i)
	{
		std::cout << points[i] << std::endl;
	}

	output = points;

}

void pnp(vector<Point2f> point4, Mat cameramatrix, Mat distcoeffs, Mat & rvec, Mat& tvec)
{
	Mat cameraMatrix = cameramatrix;
	Mat distCoeffs = distcoeffs;
	Mat rvec_tmp;
	Mat tvec_tmp;


	vector<cv::Point2f> imagePoints = point4;
	vector<cv::Point3f> objectPoints ;
	Generate3DPoints(objectPoints);

	for (const auto& point : imagePoints) {
		cout << "(" << point.x << ", " << point.y << ")" << endl;
	}
	cout << "Vector Point2f size: " << distCoeffs.size() << endl;

	//計算出相機旋轉的參數和相機平移的參數
	solvePnP(objectPoints, point4, cameramatrix, distcoeffs, rvec_tmp, tvec_tmp);

	rvec = rvec_tmp.clone();
	tvec = tvec_tmp.clone();

	// cout << "rrrrrrrrrr=======>" << rvec.size() << endl;	
}

void compute_transformation_matrix(Mat rvec_camera1, Mat tvec_camera1, Mat rvec_camera2, Mat tvec_camera2)
{
	cout << "transformation matrix" << endl;
	Mat R_one, R_two;
	Rodrigues(rvec_camera1, R_one);
	Rodrigues(rvec_camera2, R_two);

	Mat R_1_to_2 = R_one * R_two.t(); // 旋轉矩陣的乘積
	Mat tvec_1_to_2 = R_one * (-R_two.t() * tvec_camera1) + tvec_camera2; // 平移矢量的計算

	// 打印坐標變換矩陣
	cout << "Coordinate transformation matrix from left CCS to right CCS:" << endl;
	cout << "Rotation matrix:" << endl << R_1_to_2 << endl << R_1_to_2.size() << endl;
	cout << "Translation vector:" << endl << tvec_1_to_2 << endl << tvec_1_to_2.size() << endl;


}


void find_4_point(Mat img, vector<Point2f> &output)
{
	Mat binary_image;
	cvtColor(img, binary_image, COLOR_BGR2GRAY);
	adaptiveThreshold(binary_image, binary_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 25);

	Mat labelImage(img.size(), CV_32S), stats, centroids;
	int nlabels = connectedComponentsWithStats(binary_image, labelImage, stats, centroids);
	// 尋找梯形模式
	//cout << nlabels << endl;
	cv::Mat whiteBackground(img.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	vector<Point2f> DistortCenterPnt;
	for (int i = 1; i < nlabels; ++i) {
		// 獲取連通組件的左上角坐標和寬高
		int x = stats.at<int>(i, cv::CC_STAT_LEFT);
		int y = stats.at<int>(i, cv::CC_STAT_TOP);
		int width = stats.at<int>(i, cv::CC_STAT_WIDTH);
		int height = stats.at<int>(i, cv::CC_STAT_HEIGHT);
		
		// 假設梯形滿足一定的寬高比和面積閾值
		double aspectRatio = static_cast<double>(width) / height;
		double area = stats.at<int>(i, cv::CC_STAT_AREA);
		if (aspectRatio > 0.3 && aspectRatio < 15 && area > 1000 && area < 7000) {
			// 在原始影像中繪製矩形
			//cout << "ratio : " << aspectRatio << endl;
			//cout << "x: " << x << ", y: " << y << ", width: " << width << ", height: " << height << std::endl;
			cv::rectangle(whiteBackground, cv::Rect(x, y, width, height), cv::Scalar(0, 0, 255), -1);
			//cout << x + (width / 2) <<" " << y + (height / 2)<<endl;
			Point tmp(x + (width / 2), y + (height / 2));
			DistortCenterPnt.push_back(tmp);
		}
		
		
	}
	Mat opening;
	Mat element = getStructuringElement(MORPH_CROSS, Size(10, 2));
	output = DistortCenterPnt;
	morphologyEx(whiteBackground, opening, MORPH_CLOSE, element);

	//undistortPoints();

	//imshow("binary_image", opening);

	
}
void find_chessboard(Mat chessboard)
{
	Mat chessboard_gray;
	cvtColor(chessboard, chessboard_gray, COLOR_BGR2GRAY);

	vector<Point2f> corners;

	bool ret = findChessboardCorners(chessboard_gray,
		Size(6, 9),
		corners,
		CALIB_CB_ADAPTIVE_THRESH |
		CALIB_CB_NORMALIZE_IMAGE);

	TermCriteria criteria = TermCriteria(
		TermCriteria::MAX_ITER + TermCriteria::EPS,
		40,
		0.1);

	cornerSubPix(chessboard_gray, corners, Size(5, 5), Size(-1, -1), criteria);

	drawChessboardCorners(chessboard, Size(6, 9), corners, ret);

	//imshow("chessboard corners", chessboard);
}

int main(int argc, char** argv)
{
	//read image
	Mat chessboard;
	chessboard = imread(samples::findFile("./HW6/Camera1/StereoImages/WIN_20240509_11_12_27_Pro.jpg"), IMREAD_COLOR); 

	Mat find4point_camera1;
	find4point_camera1 = imread(samples::findFile("./HW6/Camera1/StereoImages/point.jpg"), IMREAD_COLOR);
	vector<Point2f> four_ponit_camera1;

	Mat find4point_camera2;
	find4point_camera2 = imread(samples::findFile("./HW6/Camera2/StereoImages/point.jpg"), IMREAD_COLOR);
	vector<Point2f> four_ponit_camera2;

	FileStorage fs("camera_parameter.xml", FileStorage::READ);
	if (!fs.isOpened()) {
		cerr << "Failed to open file!" << endl;
		return -1;
	}

	// 讀取相機內部參數
	Mat camera1Matrix;
	fs["camera1_matrix"] >> camera1Matrix;

	Mat distCoeffs1;
	fs["distortion1_coefficients"] >> distCoeffs1;

	Mat camera2Matrix;
	fs["camera2_matrix"] >> camera2Matrix;

	Mat distCoeffs2;
	fs["distortion2_coefficients"] >> distCoeffs2;

	// 關閉檔案
	fs.release();

	find_chessboard(chessboard);
	find_4_point(find4point_camera1, four_ponit_camera1);
	find_4_point(find4point_camera2, four_ponit_camera2);

	Mat rvec_camera1, tvec_camera1, rvec_camera2, tvec_camera2;
	pnp(four_ponit_camera1, camera1Matrix, distCoeffs1, rvec_camera1, tvec_camera1);
	pnp(four_ponit_camera2, camera2Matrix, distCoeffs2, rvec_camera2, tvec_camera2);
	compute_transformation_matrix(rvec_camera1, tvec_camera1, rvec_camera2, tvec_camera2);

	


	//esc to escape
	char key = waitKey(0);
	if (key == 27) destroyAllWindows();

	return 0;
}