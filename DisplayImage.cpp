#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>


cv::Mat grayImage(cv::Mat& img)
{
	int h = img.rows;
	int w = img.cols;
	int c = img.channels();

	assert(c==3);

	uchar* im_out = (uchar*)malloc(h * w * sizeof(uchar));
	uchar* im_in = img.ptr<uchar>(0);

	for (int i=0, p_i=0; i < h * w * c; i += c, p_i++) {
		im_out[p_i] =  0.299 * im_in[i] + 0.587 * im_in[i+1] + 0.114 * im_in[i+2];
	}

	return cv::Mat(h, w, CV_8UC1, im_out);
}


int main(int argc, char** argv)
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    cv::Mat image;
    image = cv::imread( argv[1], cv::IMREAD_COLOR);
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    cv::Mat result = grayImage(image);
    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    cv::imshow("Display Image", result);
    cv::waitKey(0);
    return 0;
}
