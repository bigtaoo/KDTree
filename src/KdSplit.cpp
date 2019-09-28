#include "../include/KdSplit.h"

KdSplit::KdSplit(int cutDim, int64_t cutVal, int64_t boundLow, int64_t boundHigh, KdNode* childLow, KdNode* childHigh) :
	m_cutDim(cutDim), m_cutVal(cutVal)
{
	m_bounds[BI_LOW] = boundLow;
	m_bounds[BI_HIGH] = boundHigh;
	m_child[BI_LOW] = childLow;
	m_child[BI_HIGH] = childHigh;
}

KdSplit::~KdSplit()
{
	for (int i = 0; i < BI_MAX; ++i)
	{
		if (m_child[i] != nullptr)
		{
			delete m_child[i];
			m_child[i] = nullptr;
		}
	}
}

void KdSplit::Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex)
{
	// get query point is in the left node or in the right node.
	int64_t cutDiff = (m_cutDim == 1) ? (int64_t(q.m_y) - m_cutVal) : (int64_t(q.m_x) - m_cutVal);

	// left node
	if (cutDiff < 0)
	{
		m_child[BI_LOW]->Search(q, distance, smallDistance, smallPointIndex);

		// determine whether to check the node of the other side by check the overlap of the distance and the bounds of the other side.
		int64_t boxDiff = (m_cutDim == 1) ? (m_bounds[BI_LOW] - int64_t(q.m_y)) : (m_bounds[BI_LOW] - int64_t(q.m_x));
		if (boxDiff < 0)
		{
			boxDiff = 0;
		}
		distance += (cutDiff * cutDiff - boxDiff * boxDiff);
		if (distance < smallDistance)
		{
			m_child[BI_HIGH]->Search(q, distance, smallDistance, smallPointIndex);
		}
	}
	// right node
	else
	{
		m_child[BI_HIGH]->Search(q, distance, smallDistance, smallPointIndex);

		// determine whether to check the node of the other side.
		int64_t boxDiff = (m_cutDim == 1) ? (int64_t(q.m_y) - m_bounds[BI_HIGH]) : (int64_t(q.m_x) - m_bounds[BI_HIGH]);
		if (boxDiff < 0)
		{
			boxDiff = 0;
		}
		distance += (cutDiff * cutDiff - boxDiff * boxDiff);
		if (distance < smallDistance)
		{
			m_child[BI_LOW]->Search(q, distance, smallDistance, smallPointIndex);
		}
	}
}