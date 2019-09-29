#pragma once

#include <vector>

struct Point;
class Compute
{
public:
	static void ComputeResult(const std::vector<Point>& points, std::vector<std::pair<Point, Point>>& pointPairs);
};