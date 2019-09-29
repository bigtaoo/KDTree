#include "../include/KdSplit.h"
#include "../include/GlobalData.h"
#include "../include/KdLeaf.h"

KdSplit::KdSplit(int cutDimension, int64_t cutValue, int64_t boundLow, int64_t boundHigh, KdNode* childLow, KdNode* childHigh) :
	m_cutDimension(cutDimension), m_cutValue(cutValue)
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
		if (m_child[i] != nullptr && m_child[i] != dynamic_cast<KdNode*>(GlobalData::TrivialLeaf))
		{
			delete m_child[i];
			m_child[i] = nullptr;
		}
	}
}

void KdSplit::Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex) const
{
	// get the distance to the cutting node.
	int64_t cutDistance = (m_cutDimension == 1) ? (int64_t(q.m_y) - m_cutValue) : (int64_t(q.m_x) - m_cutValue);

	// left node
	if (cutDistance < 0)
	{
		m_child[BI_LOW]->Search(q, distance, smallDistance, smallPointIndex);

		// determine whether to check the node of the other side by check the overlap of the distance and the bounds of the other side.
		int64_t boundDistance = (m_cutDimension == 1) ? (m_bounds[BI_LOW] - int64_t(q.m_y)) : (m_bounds[BI_LOW] - int64_t(q.m_x));
		if (boundDistance < 0)
		{
			boundDistance = 0;
		}
		distance += (cutDistance * cutDistance - boundDistance * boundDistance);
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
		int64_t boundDistance = (m_cutDimension == 1) ? (int64_t(q.m_y) - m_bounds[BI_HIGH]) : (int64_t(q.m_x) - m_bounds[BI_HIGH]);
		if (boundDistance < 0)
		{
			boundDistance = 0;
		}
		distance += (cutDistance * cutDistance - boundDistance * boundDistance);
		if (distance < smallDistance)
		{
			m_child[BI_LOW]->Search(q, distance, smallDistance, smallPointIndex);
		}
	}
}