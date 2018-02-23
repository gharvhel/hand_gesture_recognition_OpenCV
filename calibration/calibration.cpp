#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace cv;

// Code for trackbarks is taken from openCV tutorial webpage
// https://docs.opencv.org/trunk/da/d97/tutorial_threshold_inRange.html
void on_low_r_thresh_trackbar(int, void *);
void on_high_r_thresh_trackbar(int, void *);
void on_low_g_thresh_trackbar(int, void *);
void on_high_g_thresh_trackbar(int, void *);
void on_low_b_thresh_trackbar(int, void *);
void on_high_b_thresh_trackbar(int, void *);

int low_r=80, low_g=40, low_b=0;
int high_r=255, high_g=255, high_b=20;

RNG rng(12345);

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage: %s <Image_Path>\n", argv[0]);
		return -1;
	}
    
    Mat image, thresh_image, hsv_image, blur_image, img_threshold;
    char *image_file_name = argv[1];
    image = imread(image_file_name, IMREAD_COLOR);

	if (image.empty())
	{
		printf("error opening file \n");
		return -1;
	}
    
    // convert to HSV 
	cvtColor(image, hsv_image, COLOR_BGR2HSV);
    namedWindow("Calibration", WINDOW_NORMAL);

    //-- Trackbars to set thresholds for RGB values
    createTrackbar("Low R","Calibration", &low_r, 255, on_low_r_thresh_trackbar);
    createTrackbar("High R","Calibration", &high_r, 255, on_high_r_thresh_trackbar);
    createTrackbar("Low G","Calibration", &low_g, 255, on_low_g_thresh_trackbar);
    createTrackbar("High G","Calibration", &high_g, 255, on_high_g_thresh_trackbar);
    createTrackbar("Low B","Calibration", &low_b, 255, on_low_b_thresh_trackbar);
    createTrackbar("High B","Calibration", &high_b, 255, on_high_b_thresh_trackbar);

    while((char)waitKey(1)!='q')
    {
        inRange(hsv_image,Scalar(low_b,low_g,low_r), Scalar(high_b,high_g,high_r),img_threshold);
        erode(img_threshold, img_threshold, MORPH_ELLIPSE);
        dilate(img_threshold, img_threshold, MORPH_ELLIPSE);
        GaussianBlur(img_threshold, img_threshold, Size(19, 19), 0.0, 0);
        imshow("Calibration",img_threshold);

    }
    return 0;
}
void on_low_r_thresh_trackbar(int, void *)
{
    low_r = min(high_r-1, low_r);
    setTrackbarPos("Low R","Calibration", low_r);
}
void on_high_r_thresh_trackbar(int, void *)
{
    high_r = max(high_r, low_r+1);
    setTrackbarPos("High R", "Calibration", high_r);
}
void on_low_g_thresh_trackbar(int, void *)
{
    low_g = min(high_g-1, low_g);
    setTrackbarPos("Low G","Calibration", low_g);
}
void on_high_g_thresh_trackbar(int, void *)
{
    high_g = max(high_g, low_g+1);
    setTrackbarPos("High G", "Calibration", high_g);
}
void on_low_b_thresh_trackbar(int, void *)
{
    low_b= min(high_b-1, low_b);
    setTrackbarPos("Low B","Calibration", low_b);
}
void on_high_b_thresh_trackbar(int, void *)
{
    high_b = max(high_b, low_b+1);
    setTrackbarPos("High B", "Calibration", high_b);
}