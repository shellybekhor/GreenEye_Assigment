#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <thread>
#include <mutex>
#include <chrono>



typedef std::queue<std::pair<std::string, int>> tasksQue;

bool done[3] = {true, true, true};
std::mutex queMutex;


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


int operateImage(std::string filepath, int num, int threadid)
{
    cv::Mat image;
    image = cv::imread(filepath, cv::IMREAD_COLOR);
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    cv::Mat result(image.rows, image.cols, CV_8UC1);
	grayImage(image, result);
	cv::imwrite("image_" + std::to_string(num) + ".jpg", result);

	done[threadid] = true;
}


tasksQue iterateImages(std::string folderName)
{
	tasksQue que;

	int i = 0;
    for (auto f = boost::filesystem::directory_iterator(folderName); f != boost::filesystem::directory_iterator(); f++)
    {
        if (!boost::filesystem::is_directory(f->path())) // eliminate directories
        {
            std::string fn = f->path().filename().string();
//        	std::cout << fn << std::endl;
        	que.push(std::make_pair(f->path().string(), ++i));

        }
        else
            continue;
    }

    return que;
}


int threads(std::string folderName)
{
	tasksQue que = iterateImages(folderName);
	std::thread threads[3];

	while (! que.empty())
	{
		for (int i=0; i < 3; i++)
		{
			if (done[i]) {
				queMutex.lock();
				std::pair<std::string, int> p = que.front();
				que.pop();
				queMutex.unlock();

				if (threads[i].joinable())
					threads[i].join();

				threads[i] = std::thread(operateImage, p.first, p.second, i);
				done[i] = false;
				std::cout << "running thread: " << i << std::endl;
			}
		}
	}
	threads[0].join();
	threads[1].join();
	threads[2].join();
	return 0;
}


int singleThread(std::string folderName)
{
	tasksQue que = iterateImages(folderName);
	while (! que.empty())
	{
		std::pair<std::string, int> p = que.front();
		que.pop();
		operateImage(p.first, p.second, 0);
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

    auto t1 = std::chrono::high_resolution_clock::now();
    threads(folder);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();


    t1 = std::chrono::high_resolution_clock::now();
    singleThread(folder);
	t2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

    std::cout << "d1:" << duration1 << std::endl;
    std::cout << "d2:" << duration2 << std::endl;

    return 0;
}
