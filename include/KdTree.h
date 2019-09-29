#pragma once

#include <vector>
#include "Point.h"
#include "KdBounds.h"

class KdNode;
class KdTree
{
private:
	KdNode* m_root;
	Point m_low;
	Point m_high;

public:
	KdTree(const std::vector<Point>& p);
	~KdTree();
	void Search(const Point& q, Point& out) const;

private:
	// get the largest bound box. used for initializing first bound box.
	void GetBoundBox(const std::vector<Point>& p, KdBounds& bound);
	// split the tree.
	void Split(const std::vector<Point>& p, int offset, int size, int dimension, const KdBounds& bound, int& cutDimension, int32_t& cutValue, int& lowPointsNumber);
	// get spread on the current dimension.
	int32_t GetSpread(const std::vector<Point>& p, int offset, int size, int dimension);
	// get min and max coordinates on the current dimension.
	void FindMaxRange(const std::vector<Point>& p, int offset, int size, int dimension, int32_t& min, int32_t& max);
	// exchange the elements with the cut value.
	void SortSplit(const std::vector<Point>& p, int offset, int size, int dimension, int32_t cutValue, int& break1, int& break2);

	int64_t GetBoundDistance(const Point& q, const Point& low, const Point& high) const;
	KdNode* RecursiveBuildKdTree(const std::vector<Point>& p, int offset, int size, int dimension, KdBounds& bound);
};