#include "../include/Compute.h"
#include "../include/Timer.h"
#include <algorithm>
#include <thread>

KdTree* Compute::pTree = nullptr;
std::vector< std::vector<std::pair<Point, Point>>> outResult;

void computeResult_Thread(const std::vector<Point>& points, uint32_t offset, uint32_t size, uint8_t index)
{
	size = std::min<uint32_t>(size, static_cast<uint32_t>(points.size()));

	for (uint32_t i = offset; i < size; ++i)
	{
		Point outPoint;
		Point inPoint = points[i];
		Compute::pTree->Search(inPoint, outPoint);

		outResult[index].push_back(std::make_pair(inPoint, outPoint));
	}
}

void Compute::ComputeResult(const std::vector<Point>& points, std::vector<std::pair<Point, Point>>& pointPairs)
{
	{
		Timer load("Build Tree");
		Compute::pTree = new KdTree(points);
	}

	pointPairs.reserve(points.size());

	const uint32_t THREAD_NUMBER = std::thread::hardware_concurrency();
	outResult.resize(THREAD_NUMBER);
	std::vector<std::thread> threads(THREAD_NUMBER);
	//std::vector<uint8_t> isCorrect(THREAD_NUMBER, 1);

	Timer* caculate = new Timer("Find Result Time :");

	const uint32_t pointsPerThread = static_cast<uint32_t>(ceil(points.size() / float(THREAD_NUMBER)));

	for (uint32_t i = 0; i < THREAD_NUMBER; i++)
	{
		threads[i] = std::thread(&computeResult_Thread, points, i * pointsPerThread, (i + 1) * pointsPerThread, i);
	}

	for (uint32_t i = 0; i < THREAD_NUMBER; i++)
	{
		threads[i].join();
	}

	delete caculate;

	for (uint32_t i = 0; i < THREAD_NUMBER; i++)
	{
		//if (isCorrect[i] == 0)
		{
			pointPairs.insert(pointPairs.end(), outResult[i].begin(), outResult[i].end());
		}
	}
}