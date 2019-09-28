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
	if (m_child[BI_LOW] != nullptr)
	{
		delete m_child[BI_LOW];
		m_child[BI_LOW] = nullptr;
	}
	if (m_child[BI_HIGH] != nullptr)
	{
		delete m_child[BI_HIGH];
		m_child[BI_HIGH] = nullptr;
	}
}

void KdSplit::Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex)
{
	int64_t cutDiff = (m_cutDim == 1) ? (int64_t(q.m_y) - m_cutVal) : (int64_t(q.m_x) - m_cutVal);

	if (cutDiff < 0)
	{
		m_child[BI_LOW]->Search(q, distance, smallDistance, smallPointIndex);

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
	else
	{
		m_child[BI_HIGH]->Search(q, distance, smallDistance, smallPointIndex);

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