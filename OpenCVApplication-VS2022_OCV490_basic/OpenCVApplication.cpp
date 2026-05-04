// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include "common.h"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <queue>|
#include <fstream>
#include <filesystem>
#include <opencv2/core/utils/logger.hpp>
#include <chrono>
#include <ctime>
#include <thread>

wchar_t* projectPath;

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height, width, CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				uchar val = src.at<uchar>(i, j);
				uchar neg = 255 - val;
				dst.at<uchar>(i, j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testNegativeImageFast()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// The fastest approach of accessing the pixels -> using pointers
		uchar* lpSrc = src.data;
		uchar* lpDst = dst.data;
		int w = (int)src.step; // no dword alignment is done !!!
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i * w + j];
				lpDst[i * w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height, width, CV_8UC1);

		// Accessing individual pixels in a RGB 24 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i, j) = (r + g + b) / 3;
			}
		}

		imshow("input image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// HSV components
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// Defining pointers to each matrix (8 bits/pixels) of the individual components H, S, V 
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, COLOR_BGR2HSV);

		// Defining the pointer to the HSV image matrix (24 bits/pixel)
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int hi = i * width * 3 + j * 3;
				int gi = i * width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;	// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1, dst2;
		//without interpolation
		resizeImg(src, dst1, 320, false);
		//with interpolation
		resizeImg(src, dst2, 320, true);
		imshow("input image", src);
		imshow("resized image (without interpolation)", dst1);
		imshow("resized image (with interpolation)", dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src, dst, gauss;
		src = imread(fname, IMREAD_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int)k * pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss, dst, pL, pH, 3);
		imshow("input image", src);
		imshow("canny", dst);
		waitKey();
	}
}

void testVideoSequence()
{
	_wchdir(projectPath);

	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}

	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		Canny(grayFrame, edges, 40, 100, 3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = waitKey(100);  // waits 100ms and advances to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n");
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	_wchdir(projectPath);

	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];

	// video resolution
	Size capS = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;

		imshow(WIN_SRC, frame);

		c = waitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115) { //'s' pressed - snap the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess)
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
	{
		printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
			x, y,
			(int)(*src).at<Vec3b>(y, x)[2],
			(int)(*src).at<Vec3b>(y, x)[1],
			(int)(*src).at<Vec3b>(y, x)[0]);
	}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

bool inside(int i, int j, int width, int height) {
	return  (i >= 0 && i < height && j >= 0 && j < width);
}

typedef struct {
	int area;
	int parameter;
	int c_min, c_max, r_min, r_max;
}component_info;

Mat get_object_instance(const Mat& org, const Mat& yCbCr, const Mat& hsv) {
	int height = org.rows;
	int width = org.cols;
	Mat dst = Mat(height, width, CV_8UC1, Scalar(0));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			uchar Y = yCbCr.at<Vec3b>(i, j)[0];
			uchar Cb = yCbCr.at<Vec3b>(i, j)[1];
			uchar Cr = yCbCr.at<Vec3b>(i, j)[2];

			uchar R = org.at<Vec3b>(i, j)[2];
			uchar G = org.at<Vec3b>(i, j)[1];
			uchar B = org.at<Vec3b>(i, j)[0];

			uchar H_norm = hsv.at<Vec3b>(i, j)[0];
			uchar S_norm = hsv.at<Vec3b>(i, j)[1];
			uchar V_norm = hsv.at<Vec3b>(i, j)[2];

			float H = H_norm * 360.0f / 255.0f;
			float S = S_norm / 255.0f;
			//float V = V_norm / 255.0f;
			int A = R - G;

			bool rgb_common = (R > 95) && (G > 40) && (B > 20) &&
				(R > G) && (R > B) &&
				(abs(A) > 15) && (A > 15);

			bool cond1 = rgb_common &&
				(H >= 0.0f && H <= 50.0f) &&
				(S >= 0.23f && S <= 0.68f) ;

			bool cond2 = rgb_common &&
				(Cr > 135) && (Cb > 85) && (Y > 80) &&
				(Cr <= 1.5862f * Cb + 20.0f) &&
				(Cr >= 0.3448f * Cb + 76.2069f) &&
				(Cr >= -4.5652f * Cb + 234.5652f) &&
				(Cr <= -1.15f * Cb + 301.75f) &&
				(Cr <= -2.2857f * Cb + 432.85f);

			if (cond1 || cond2) 
				dst.at<uchar>(i, j) = 255;
		}
	}
	return dst;
}

std::vector<component_info> bfs(const Mat& src, int& label) {
	int height = src.rows;
	int width = src.cols;
	std::vector<component_info> components;
	label = 0;
	Mat visit = Mat(height, width, CV_32SC1, Scalar(0));
	std::queue<Point> q;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 255 && visit.at<int>(i, j) == 0) {
				label++;
				visit.at<int>(i, j) = label;
				component_info c = { 0,0,j,j,i,i };
				q.push(Point(j, i));

				while (!q.empty()) {
					Point p = q.front();
					q.pop();
					c.area++;
					c.c_max = max(c.c_max, p.x);
					c.c_min = min(c.c_min, p.x);
					c.r_max = max(c.r_max, p.y);
					c.r_min = min(c.r_min, p.y);

					if ((inside(p.y - 1, p.x - 1, width, height) && src.at<uchar>(p.y - 1, p.x - 1) == 0) ||
						(inside(p.y - 1, p.x, width, height) && src.at<uchar>(p.y - 1, p.x) == 0) ||
						(inside(p.y - 1, p.x + 1, width, height) && src.at<uchar>(p.y - 1, p.x + 1) == 0) ||
						(inside(p.y, p.x - 1, width, height) && src.at<uchar>(p.y, p.x - 1) == 0) ||
						(inside(p.y, p.x + 1, width, height) && src.at<uchar>(p.y, p.x + 1) == 0) ||
						(inside(p.y + 1, p.x - 1, width, height) && src.at<uchar>(p.y + 1, p.x - 1) == 0) ||
						(inside(p.y + 1, p.x, width, height) && src.at<uchar>(p.y + 1, p.x) == 0) ||
						(inside(p.y + 1, p.x + 1, width, height) && src.at<uchar>(p.y + 1, p.x + 1) == 0)
						) {
						c.parameter++;
					}
					std::vector<Point> neighbor = {
						Point(p.x - 1, p.y - 1), Point(p.x,   p.y - 1), Point(p.x + 1, p.y - 1),
						Point(p.x - 1, p.y),                              Point(p.x + 1, p.y),
						Point(p.x - 1, p.y + 1), Point(p.x,   p.y + 1), Point(p.x + 1, p.y + 1)
					};

					for (auto n : neighbor) {
						if (inside(n.y, n.x, width, height) && src.at<uchar>(n.y, n.x) == 255 && visit.at<int>(n.y, n.x) == 0) {
							q.push(n);
							visit.at<int>(n.y, n.x) = label;
						}
					}
				}
				components.push_back(c);
			}
		}
	}
	return components;
}

Mat gaussian_blur(const Mat& src) {
	int height = src.rows;
	int width = src.cols;
	Mat dst = src.clone();

	float kernel[3][3] = {
		{1 / 16.f, 2 / 16.f, 1 / 16.f},
		{2 / 16.f, 4 / 16.f, 2 / 16.f},
		{1 / 16.f, 2 / 16.f, 1 / 16.f}
	};

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			for (int c = 0; c < 3; c++) {
				float sum = 0;
				for (int ki = -1; ki < 2; ki++) {
					for (int kj = -1; kj < 2; kj++) {
						sum += src.at<Vec3b>(i + ki, j + kj)[c] * kernel[ki + 1][kj + 1];
					}
				}
				dst.at<Vec3b>(i, j)[c] = (uchar)sum;
			}
		}
	}

	return dst;
}

Mat median_filter(const Mat& src)	 {
	int height = src.rows;
	int width = src.cols;
	Mat dst = Mat::zeros(height, width, CV_8UC1);

	for (int i = 2; i < height - 2; i++) {
		for (int j = 2; j < width - 2; j++) {
			int white_count = 0;
			for (int ki = -2; ki <= 2; ki++) {
				for (int kj = -2; kj <= 2; kj++) {
					if (src.at<uchar>(i + ki, j + kj) == 255)
						white_count++;
				}
			}
			dst.at<uchar>(i, j) = (white_count >= 13) ? 255 : 0;
		}
	}
	return dst;
}

int calculate_area(const Mat& src) {
	int height = src.rows;
	int width = src.cols;
	int area = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 255) {
				area++;
			}
		}
	}
	return area;
}

Point2d calculate_center(const Mat& src) {
	int height = src.rows;
	int width = src.cols;
	int r = 0;
	int c = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 255) {
				r += i; // pe randuri, nu pe culori
				c += j;
			}
		}
	}

	r /= calculate_area(src);
	c /= calculate_area(src);

	return Point2d(c, r);
}

float calculte_angle(const Mat& src, Point center) {
	float angle = 0;
	float up = 0;
	float d1 = 0;
	float d2 = 0;
	int height = src.rows;
	int width = src.cols;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 255)
				up += (i - center.y) * (j - center.x);
		}
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 255) {
				d1 += (j - center.x) * (j - center.x);
				d2 += (i - center.y) * (i - center.y);
			}
		}
	}

	return atan2(2 * up, (d1 - d2)) / 2;
}

bool is_contour(const Mat& src, int i, int j, int width, int height) {
	return (
		(inside(i - 1, j - 1, width, height) && src.at<uchar>(i - 1, j - 1) == 0) ||
		(inside(i - 1, j, width, height) && src.at<uchar>(i - 1, j) == 0) ||
		(inside(i - 1, j + 1, width, height) && src.at<uchar>(i - 1, j + 1) == 0) ||
		(inside(i, j - 1, width, height) && src.at<uchar>(i, j - 1) == 0) ||
		(inside(i, j + 1, width, height) && src.at<uchar>(i, j + 1) == 0) ||
		(inside(i + 1, j - 1, width, height) && src.at<uchar>(i + 1, j - 1) == 0) ||
		(inside(i + 1, j, width, height) && src.at<uchar>(i + 1, j) == 0) ||
		(inside(i + 1, j + 1, width, height) && src.at<uchar>(i + 1, j + 1) == 0)
		);
}

Mat transfor_Ycbcr(const Mat& src) {
	int height = src.rows;
	int width = src.cols;

	Mat dst = Mat(height, width, CV_8UC3);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			//BGR
			uchar b = src.at<Vec3b>(i, j)[0];
			uchar g = src.at<Vec3b>(i, j)[1];
			uchar r = src.at<Vec3b>(i, j)[2];
			double y = 0.299 * (double)r + 0.587 * (double)g + 0.114 * (double)b;
			double cb = 128.0 - 0.169 * (double)r - 0.331 * (double)g + 0.5 * (double)b;
			double cr = 128.0 + 0.5 * (double)r - 0.419 * (double)g - 0.081 * (double)b;

			y = (y < 0.0) ? 0.0 : (y > 255.0) ? 255.0 : y;
			cb = (cb < 0.0) ? 0.0 : (cb > 255.0) ? 255.0 : cb;
			cr = (cr < 0.0) ? 0.0 : (cr > 255.0) ? 255.0 : cr;

			dst.at<Vec3b>(i, j) = Vec3b((uchar)y, (uchar)cb, (uchar)cr);
		}
	}
	return dst;
}

void normalize_Y(Mat& src) {
	int height = src.rows;
	int width = src.cols;
	int y_min = 255;
	int y_max = 0;

	//for (int i = 0; i < height; i++) {
	//	for (int j = 0; j < width; j++) {
	//		if (src.at<Vec3b>(i, j)[0] < y_min) {
	//			y_min = src.at<Vec3b>(i, j)[0];
	//		}
	//		if (src.at<Vec3b>(i, j)[0] > y_max) {
	//			y_max = src.at<Vec3b>(i, j)[0];
	//		}
	//	}
	//}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			//float normalized = (float)(src.at<Vec3b>(i, j)[0] - y_min) / (y_max - y_min);
			//src.at<Vec3b>(i, j)[0] = (uchar)(normalized * 255);
			src.at<Vec3b>(i, j)[0] = 128;
		}
	}
}

void draw_hists(const Mat& src) {
	int** hist = (int**)calloc(3, sizeof(int*));
	hist[0] = (int*)calloc(256, sizeof(int));
	hist[1] = (int*)calloc(256, sizeof(int));
	hist[2] = (int*)calloc(256, sizeof(int));

	int height = src.rows;
	int width = src.cols;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vec3b val = src.at<Vec3b>(i, j);
			hist[0][val[0]]++;
			hist[1][val[1]]++;
			hist[2][val[2]]++;
		}
	}
	showHistogram("Hist Y", hist[0], 256, 200);
	showHistogram("Hist cb", hist[1], 256, 200);
	showHistogram("Hist cr", hist[2], 256, 200);

}

Mat transoform_HSV(const Mat& src) {
	int height = src.rows;
	int width = src.cols;

	Mat dst = Mat(height, width,CV_8UC3, Scalar(0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vec3b pixel = src.at<Vec3b>(i, j);
			float b = (float)pixel[0] / 255;
			float g = (float)pixel[1] / 255;
			float r = (float)pixel[2] / 255;
			float M = max(b, max(g, r));
			float m = min(b, min(g, r));
			float C = M - m;
			float V = M;
			float S = 0.0;
			float H = 0.0;

			if (V != 0.0) {
				S = C / V;
			}
			else S = 0;

			if (C != 0.0) {
				if (M == r)      H = 60.0f * (g - b) / C;
				else if (M == g) H = 120.0f + 60.0f * (b - r) / C;
				else if (M == b) H = 240.0f + 60.0f * (r - g) / C;
			}
			else H = 0;

			if (H < 0) {
				H = H + 360;
			}

			uchar H_norm = H * 255 / 360;
			uchar S_norm = S * 255;
			uchar V_norm = V * 255;
			dst.at<Vec3b>(i, j) = Vec3b(H_norm, S_norm, V_norm);

		}
	}

	return dst;
}

std::string get_filename(const char* path) {
	std::string s(path);
	size_t pos = s.find_last_of("/\\");
	if (pos == std::string::npos)
		return s;
	return s.substr(pos + 1);
}

void get_img_resized(const Mat& src, int c_min, int c_max, int r_min, int r_max) {
	int center_i = r_max + r_min / 2;
	int center_j = c_max + c_min / 2;

	int height = r_max - r_min;
	int width = c_max - c_min;
	int lat = max(height, width);


	int new_x = center_i - lat / 2;
	int new_y = center_j - lat / 2;
	Rect dorit(center_i, center_j, lat, lat);

	Rect imagine_limite(0, 0, src.cols, src.rows);

	Rect valid = dorit & imagine_limite;

	if (valid.width <= 0 || valid.height <= 0) {
		return;
	}

	Mat face = src(valid);
	Mat final_100x100;

	cv::resize(face, final_100x100, Size(100, 100));
	imwrite("Razvan.png", final_100x100);
}

typedef struct {
	Mat resized;
	Mat all_face;
}Mat_tuple;

Mat_tuple draw_with_thiness(const Mat& color, const std::vector<component_info>& components, const char* fname) {
	int height = color.rows;
	int width = color.cols;
	Mat resized;
	Mat dst = color.clone();

	for (auto c : components) {
		if (c.area < 4500 || c.area > 200000)
			continue;

		double thiness = (4.0 * CV_PI * (double)c.area) / ((double)c.parameter * (double)c.parameter);

		int box_w = c.c_max - c.c_min;
		int box_h = c.r_max - c.r_min;

		float wth = (float)box_w / (float)box_h;

		if (wth < 0.6 || wth > 1.0)
			continue;


		//if (thiness < 0.1 || thiness > 0.80)
		//	continue;
		printf("Area: %d | Perimeter: %d | Thinness: %.3f | width to height %.3f \n", c.area, c.parameter, thiness, wth);


		int r_min = max(c.r_min, 0);
		int r_max = min(c.r_max, color.rows - 1);
		int c_min = max(c.c_min, 0);
		int c_max = min(c.c_max, color.cols - 1);

		rectangle(dst,
			Point(c_min, r_min),
			Point(c_max, r_max),
			Scalar(0, 255, 0), 2);
		Mat write = Mat(r_max - r_min + 1, c_max - c_min + 1, CV_8UC3);
		for (int i = r_min ; i <= r_max ; i++) {
			for (int j = c_min ; j <= c_max ; j++) {
				write.at<Vec3b>(i - r_min, j - c_min) = color.at<Vec3b>(i, j);
			}
		}
		resize(write, resized, Size(100, 100), 0, 0, INTER_AREA);
		//imwrite("Razvan.png", resized);
		//csv << get_filename(fname) << "," << c_min << "," << c_max << "," << r_min << "," << r_max << "\n";
	}
	return Mat_tuple{ resized, dst };
}

Mat dilatation(const Mat& src) {
	int kernel[5][5] = { {0,0,1,0,0},
							 {0,1,1,1,0},
							 {1,1,1,1,1},
							 {0,1,1,1,0},
							 {0,0,1,0,0} };

	int height = src.rows;
	int width = src.cols;
	Mat dst = Mat(height, width, CV_8UC1, Scalar(0));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			bool white = false;

			for (int ki = 0; ki < 5;ki++) {
				for (int kj = 0; kj < 5; kj++) {
					if (kernel[ki][kj] == 1) {
						int ii = i + (ki - 2);
						int jj = j + (kj - 2);

						if (!inside(ii, jj, width, height))
							continue;
						else {
							if (src.at<uchar>(ii, jj) == 255) {
								white = true;
								break;
							}
						}
					}
				}
				if (white)
					break;
			}

			if (white)
				dst.at<uchar>(i, j) = 255;

		} 
	}
	return dst;
}

Mat erosion(const Mat& src) {
	int kernel[5][5] = { {0,0,1,0,0},
						 {0,1,1,1,0},
						 {1,1,1,1,1},
						 {0,1,1,1,0},
						 {0,0,1,0,0} };

	int height = src.rows;
	int width = src.cols;
	Mat dst = Mat(height, width, CV_8UC1, Scalar(0));


	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			bool white = true;
			for (int ki = 0; ki < 5; ki++) {
				for (int kj = 0; kj < 5; kj++) {
					int ii = i + (ki - 2);
					int jj = j + (kj - 2);

					if (kernel[ki][kj] == 1) {
						if (!inside(ii, jj, width, height))
							continue;
						else {
							if (src.at<uchar>(ii, jj) == 0) {
								white = false;
								break;
							}
						}
					}
				}
				if (!white)
					break;
			}
			if (white)
				dst.at<uchar>(i, j) = 255;
		}
	}
	return dst;
}

void get_photos_with_person(const Mat& src, const std::string name, int i) {
	// src este t.resized
	std::string folder = "../TestPy/MyDataset/Faces/App_Faces/" + name;

	std::filesystem::create_directories(folder);
	std::string img_name = "../TestPy/MyDataset/Faces/App_Faces/" + name + "/" + name + "_" + std::to_string(i) + ".png";
	imwrite(img_name, src);
}


void cam(const std::string& name) {
	//int Port = 0;
	//telefon este 2
	char fname[MAX_PATH];
	Mat img;
	VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);


	if (!cap.isOpened()) {
		std::cout << "Could not open the camera" << std::endl;
		return;
	}
	int count = 0;
	auto last_save = std::chrono::steady_clock::now();
	while (cap.isOpened()) {
		cap >> img;
		//img = imread(fname);
		Mat ycbcr = transfor_Ycbcr(img);
		normalize_Y(ycbcr);
		Mat hsv = transoform_HSV(img);
		Mat obj = get_object_instance(img, ycbcr, hsv);
		Mat median = median_filter(obj);
		Mat dil = dilatation(median);
		Mat ero = erosion(dil);
		int l = 0;


		std::vector<component_info> cs = bfs(ero, l);
		if (!img.empty()) {
			Mat_tuple t = draw_with_thiness(img, cs,NULL);
			auto now = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_save).count();
			if (count < 30 && !t.resized.empty() && elapsed >= 500) {
				get_photos_with_person(t.resized, name, count);
				count++;
				last_save = now;
				std::cout << "Salvat " << count << "/30" << std::endl;
			}
			imshow("t", t.all_face);
			//imwrite("test.jpg",)
			imshow("original camera", img);
			imshow("ycbcr", ycbcr);
			imshow("hsv", hsv);
			imshow("binary", obj);
			//imshow("dilatation", dil);
			imshow("erosion", ero);
		}
		waitKey(1);
	}
}
//void read_argv_img(char* file) {
//	char fname[MAX_PATH];
//	strncpy(fname, file, MAX_PATH - 1);
//	fname[MAX_PATH - 1] = '\0';
//	const char* path = "../CSVs/coords.csv";
//	bool file_exists = std::ifstream(path).good();
//	std::ofstream csv(path, std::ios::app);
//	if (!file_exists)
//		csv << "filename,c_min,c_max,r_min,r_max\n";
//	/*while (openFileDlg(fname)) {*/
//		Mat src = imread(fname);
//		if(src.channels() == 1)
//			cvtColor(src, src, COLOR_GRAY2BGR);
//
//		Mat ycbcr = transfor_Ycbcr(src);
//		Mat hsv = transoform_HSV(src);
//		Mat obj = get_object_instance(src, ycbcr, hsv);
//		Mat median = median_filter(obj);
//		Mat ero = erosion(median);
//		int l = 0;
//		std::vector<component_info> cs = bfs(ero, l);
//		Mat thiness = draw_with_thiness(src, cs, fname, csv);
//		std::string file_output = "../MyDataset/Images/"+ get_filename(fname);
//		imwrite(file_output, thiness);
//		
//	//}
//	csv.close();
//
//}

int main(int argc, char* argv[])
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
	projectPath = _wgetcwd(0, 0);
	std::string name;
	std::cout << "Enter your name: ";
	std::cin >> name;
	cam(name);
	//read_argv_img(argv[1]);
	return 0;
}