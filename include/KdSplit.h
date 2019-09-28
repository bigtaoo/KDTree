#pragma once

#include "KdNode.h"
#include "KdBounds.h"

class KdSplit : public KdNode
{
public:
	KdSplit(int cutDim, int64_t cutVal, int64_t boundLow, int64_t boundHigh, KdNode* childLow = nullptr, KdNode* childHigh = nullptr);
	~KdSplit();

	void Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex) override;

private:
	int m_cutDim;	// cut dimensionality of current node cut dimensionality (x or y).
	int64_t m_cutVal; // cut value of current node. 
	int64_t m_bounds[BI_MAX];
	KdNode* m_child[BI_MAX];
};