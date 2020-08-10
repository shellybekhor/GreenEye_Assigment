#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <fstream>


typedef std::queue<std::pair<std::string, int>> tasksQue;

tasksQue que;
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


int operateImage(std::string filepath, int num)
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
        	que.push(std::make_pair(f->path().string(), ++i));
        }
        else
            continue;
    }

    return que;
}


void threadFunction()
{
	while (true)
	{
		queMutex.lock();
		if (que.empty())
		{
			queMutex.unlock();
			return;
		}
		std::pair<std::string, int> p = que.front();
		que.pop();
		queMutex.unlock();

		operateImage(p.first, p.second);
		std::cout << "running image: " << p.second << std::endl;
	}
}


int threads(int numThreads)
{
	std::vector<std::thread> threadsPool;

	for (int i=0; i < numThreads-1; i++)
	{
		threadsPool.push_back(std::thread(threadFunction));
	}
	threadFunction(); // this thread is working too!

	for (std::thread &every_thread: threadsPool)
	{
		every_thread.join();
	}
	threadsPool.clear();
	return 0;
}


int main(int argc, char** argv)
{
    if ( argc < 2 )
    {
        printf("usage: DisplayImage <Images_Directory>\n");
        return -1;
    }
    std::string folder = argv[1];
    int numOfThreads = 2;
    if (argc > 2)
    {
    	if (! isdigit(argv[2][0]))
    	{
    		printf("second input is not a number, using defult number of threads: 2\n");
    	}
    	else
    	{
    		numOfThreads = std::stoi(std::string(argv[2]));
    	}
    }
    int maxThreads = numOfThreads;
    if (argc > 3)
    {
    	if (isdigit(argv[3][0])){
    		maxThreads = std::atoi(argv[3]);
    		printf("Will run compression between %d threads to %d\n", numOfThreads, maxThreads);
    	}
    }

    std::ofstream myfile;
    myfile.open ("durations.txt");
    for (int i=numOfThreads; i<maxThreads+1; i++)
    {
    	que = iterateImages(folder); // init task queue
    	printf("Start working with %d threads\n", i);

    	auto t1 = std::chrono::high_resolution_clock::now();
		threads(i);
		auto t2 = std::chrono::high_resolution_clock::now();
		auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

		myfile << duration1 << " " << i << std::endl;
		std::cout << i << " threads took: " << duration1 << std::endl;
    }

    myfile.close();

    return 0;
}
