#pragma once

/*
use static to store global data that never can be changed after the tree build finished.
this way can speed up the recursive function deal with stacking and unstacking instead of pass them as a parameter.
*/

#include <vector>

class KdLeaf;
struct Point;
class GlobalData
{
public:
	// the invalid leaf that has no elements.
	static KdLeaf* TrivialLeaf;
	// all original input points.
	static std::vector<Point> OriginalPoints;
	// indicate the point index that used for easier sort when building the tree.
	static int* SortedIndices;
};