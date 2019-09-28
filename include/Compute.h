#pragma once

#include <vector>
#include "Point.h"
#include "KdTree.h"

class Compute
{
public:
	static void ComputeResult(const std::vector<Point>& points, std::vector<std::pair<Point, Point>>& pointPairs);
};