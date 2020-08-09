#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/filesystem.hpp>


int grayImage(cv::Mat& img, cv::Mat& result)
{

	int h = img.rows;
	int w = img.cols;
	int c = img.channels();

	if ((c != 3) or h != result.rows or w != result.cols)
	{
		std::cout << "invalid parameters" << std::endl;
		return -1;
	}

	uchar* im_out = result.ptr<uchar>(0);
	uchar* im_in = img.ptr<uchar>(0);

	for (int i=0, p_i=0; i < h * w * c; i += c, p_i++)
	{
		im_out[p_i] =  0.299 * im_in[i] + 0.587 * im_in[i+1] + 0.114 * im_in[i+2];
	}

	return 0;
}


int iterateImages(std::string folderName)
{
	int i = 0;
    for (auto f = boost::filesystem::directory_iterator(folderName); f != boost::filesystem::directory_iterator(); f++)
    {
        if (!boost::filesystem::is_directory(f->path())) // eliminate directories
        {
            std::string fn = f->path().filename().string();
        	std::cout << fn << std::endl;

            cv::Mat image;
            image = cv::imread(f->path().string(), cv::IMREAD_COLOR);
            if ( !image.data )
            {
                printf("No image data \n");
                return -1;
            }
            cv::Mat result(image.rows, image.cols, CV_8UC1);
			grayImage(image, result);
			cv::imwrite("image_" + std::to_string(++i) + ".jpg", result);
        }
        else
            continue;
    }
}



int main(int argc, char** argv)
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
//    std::string folder = argv[1];
    std::string folder = "/home/ian/Documents/Shelly/opencv/DisplayImage/images/";
    iterateImages(folder);

    return 0;
}
