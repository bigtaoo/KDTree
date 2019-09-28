#pragma once

#include <vector>

class KdLeaf;
struct Point;
class GlobalData
{
public:
	static KdLeaf* TrivialLeaf;
	static std::vector<Point> OriginalPoints;
	static int* SortedIndices;
};