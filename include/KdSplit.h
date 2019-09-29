#pragma once

#include "KdNode.h"
#include "KdBounds.h"

class KdSplit : public KdNode
{
public:
	KdSplit(int cutDimension, int64_t cutValue, int64_t boundLow, int64_t boundHigh, KdNode* childLow = nullptr, KdNode* childHigh = nullptr);
	~KdSplit();

	void Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex) const override;

private:
	int m_cutDimension;	// cut dimension of current node (x or y).
	int64_t m_cutValue; // cut value of current node. 
	int64_t m_bounds[BI_MAX];
	KdNode* m_child[BI_MAX];
};