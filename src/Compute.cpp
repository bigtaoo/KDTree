#include "../include/Compute.h"
#include "../include/Timer.h"
#include "../include/KdTree.h"
#include <algorithm>
#include <thread>
#include <math.h>

void computeResult_Thread(const KdTree* kdTree, const std::vector<Point>& points, std::vector<std::pair<Point, Point>>& pointPairs, uint32_t offset, uint32_t size)
{
	size = std::min<uint32_t>(size, static_cast<uint32_t>(points.size()));

	for (uint32_t i = offset; i < size; ++i)
	{
		Point outPoint;
		Point inPoint = points[i];
		kdTree->Search(inPoint, outPoint);

		// reset the output value. each thread uses its own data segment so don't need to lock it.
		pointPairs[i] = std::make_pair(inPoint, outPoint);
	}
}

void Compute::ComputeResult(const std::vector<Point>& points, std::vector<std::pair<Point, Point>>& pointPairs)
{
	KdTree* kdTree = nullptr;
	{
		Timer timer("BuildTree");
		kdTree = new KdTree(points);
	}
	if (kdTree == nullptr)
	{
		std::cout << "Build KdTree failed!" << std::endl;
		return;
	}

	// resize output vector to set value easier.
	pointPairs.resize(points.size());

	// use multiple threads to speed up the calculating.
	const uint32_t THREAD_NUMBER = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(THREAD_NUMBER);

	{
		Timer timer("ComputeResult");
		const uint32_t pointsPerThread = static_cast<uint32_t>(ceil(points.size() / float(THREAD_NUMBER)));
		for (uint32_t i = 0; i < THREAD_NUMBER; i++)
		{
			threads[i] = std::thread(&computeResult_Thread, kdTree, points, std::ref(pointPairs), i * pointsPerThread, (i + 1) * pointsPerThread);
		}
		for (uint32_t i = 0; i < THREAD_NUMBER; i++)
		{
			threads[i].join();
		}
	}

	if (kdTree != nullptr)
	{
		delete kdTree;
		kdTree = nullptr;
	}
}
