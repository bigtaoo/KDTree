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
	void EnclosingRectangle(std::vector<Point>& p, KdBounds& bound);
	void Split(const std::vector<Point>& p, int* pIndices, int start, int n, int Dimension, const KdBounds& bound, int& cutDim, int& cutValue, int& lowPointsNumber);
	int Spread(const std::vector<Point>& p, int* pIndices, int start, int n, int d);
	void MinMax(const std::vector<Point>& p, int* pIndices, int start, int n, int d, int& min, int& max);
	void PlaneSplit(const std::vector<Point>& p, int* pIndices, int start, int n, int d, int cv, int& br1, int& br2);
	int64_t BoxDistance(const Point& q, const Point& low, const Point& high) const;
	KdNode* RecursiveBuildKdTree(const std::vector<Point>& p, int start, int n, int Dimension, int bucketSpace, KdBounds& bound);
};