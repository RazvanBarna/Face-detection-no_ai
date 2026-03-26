// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <queue>
#include <opencv2/core/utils/logger.hpp>


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
	Mat contour;
	int length;
}contour_info;

typedef struct {
	int c_min, c_max, r_min, r_max;
}rectangle_coord;

Mat get_object_instance(const Mat& src, const Mat& org) {
	int height = src.rows;
	int width = src.cols;
	Mat dst = Mat(height, width, CV_8UC1, Scalar(0));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vec3b val = src.at<Vec3b>(i, j);
			Vec3b val_org = org.at<Vec3b>(i, j);
			uchar b = val_org[0];
			uchar g = val_org[1];
			uchar r = val_org[2];

			bool bgr_skin = r > 95 && g > 40 && b > 20 &&
				r > g && r > b &&
				abs((int)r - (int)g) > 15 &&
				r > 100;

			if (
				val[1] >= 100 && val[1] <= 150 &&
				val[2] >= 140 && val[2] <= 168) {
				dst.at<uchar>(i, j) = 255;
			}
		}
	}
	return dst;
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

contour_info parameter(const Mat& src) {
	int height = src.rows;
	int width = src.cols;
	Mat cont = Mat(height, width, CV_8UC1, Scalar(0));
	int length = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			uchar pixel = src.at<uchar>(i, j);
			if (pixel == 255 && is_contour(src, i, j, width, height)) {
				length++;
				cont.at<uchar>(i, j) = 255;
			}
		}
	}
	return contour_info{ cont, length };
}

float thinness_ratio(const Mat& src) {
	int area = calculate_area(src);
	int per = parameter(src).length;

	return (4.0f * CV_PI * (float)area) / ((float)per * (float)per);
}

rectangle_coord find_rect(const Mat& src) {
	int height = src.rows;
	int width = src.cols;
	int c_min = INT_MAX;
	int c_max = 0;
	int r_min = INT_MAX;
	int r_max = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			uchar pixel = src.at<uchar>(i, j);
			if (pixel == 255) {
				if (i < r_min)
					r_min = i;
				if (i > r_max)
					r_max = i;
				if (j > c_max)
					c_max = j;
				if (j < c_min)
					c_min = j;
			}
		}
	}
	return rectangle_coord{ c_min, c_max, r_min,r_max };
}

Mat draw_rectangle(rectangle_coord r, const Mat& src) {

	Mat dst = src.clone();
	int height = src.rows;
	int width = src.cols;
	for (int i = r.c_min; i <= r.c_max; i++) {
		dst.at<Vec3b>(r.r_min, i) = (0, 126, 126);
		dst.at<Vec3b>(r.r_max, i) = (0, 126, 126);
	}

	for (int i = r.r_min; i <= r.r_max; i++) {
		dst.at<Vec3b>(i, r.c_min) = (0, 126, 126);
		dst.at<Vec3b>(i, r.c_max) = (0, 126, 126);
	}

	return dst;
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

			dst.at<Vec3b>(i, j) = Vec3b(y, cb, cr);
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

Mat bfs(const Mat& src, int& label) {
	label = 0;
	int height = src.rows;
	int width = src.cols;
	Mat labels = Mat(height, width, CV_32SC1, Scalar(0));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 255 && labels.at<int>(i, j) == 0) {
				label++;
				std::queue<Point> q;
				labels.at<int>(i, j) = label;
				q.push(Point{ j,i });
				while (!q.empty()) {
					Point p = q.front();
					std::vector<Point> neighbor = {
				   Point(p.x - 1, p.y - 1), Point(p.x,   p.y - 1), Point(p.x + 1, p.y - 1),
				   Point(p.x - 1, p.y),                        Point(p.x + 1, p.y),
				   Point(p.x - 1, p.y + 1), Point(p.x,   p.y + 1), Point(p.x + 1, p.y + 1)
					};
					q.pop();
					for (auto point : neighbor) {
						if (inside(point.y, point.x, width, height) && src.at<uchar>(point.y, point.x) == 255 && labels.at<int>(point.y, point.x) == 0) {
							labels.at<int>(point.y, point.x) = label;
							q.push(point);
						}
					}
				}
			}
		}
	}

	return labels;
}

void draw_with_thiness(const Mat& src, int labels) {
	int height = src.rows;
	int width = src.cols;

	Mat dst;
	for (int l = 1; l <= labels; l++) {
		dst = Mat(height, width, CV_8UC1, Scalar(0));
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (src.at<int>(i, j) == l) {
					dst.at<uchar>(i, j) = 255;
				}
			}
		}
		float thiness = thinness_ratio(dst);
		int area = calculate_area(dst);
		if (thiness > 0.2f && thiness < 0.9f && area > 3000) {
			imshow("face", dst);
		}
	}
}
void cam() {
	int Port = 0;
	Mat img;
	VideoCapture cap(Port);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	if (!cap.isOpened()) {
		std::cout << "Could not open the camera" << std::endl;
		return;
	}

	while (cap.isOpened()) {
		cap >> img;
		//Mat blur = gaussian_blur(img);
		Mat dst1 = transfor_Ycbcr(img);
		//normalize_Y(dst1);
		draw_hists(dst1);
		Mat obj = get_object_instance(dst1, img);
		int l = 0;
		Mat b = bfs(obj, l);
		if (!img.empty()) {
			draw_with_thiness(b, l);
			imshow("Obj", obj);
			imshow("Original", img);
			imshow("Video camera", dst1);
		}
		waitKey(1);
	}
}

int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
	projectPath = _wgetcwd(0, 0);
	cam();
	return 0;
}