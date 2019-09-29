#include "../include/KdLeaf.h"
#include "../include/GlobalData.h"
#include "../include/Point.h"

KdLeaf::KdLeaf(int size, int offset) : m_size(size), m_offset(offset)
{
}

KdLeaf::~KdLeaf() 
{
}

void KdLeaf::Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex) const
{
	int64_t dis = 0;

	for (int i = 0; i < m_size; ++i)
	{
		const Point& p = GlobalData::OriginalPoints[GlobalData::SortedIndices[m_offset + i]];
		dis = 0;

		// get the distance between the query point and the check point.
		int64_t x = int64_t(q.m_x - p.m_x);
		int64_t y = int64_t(q.m_y - p.m_y);
		dis += (x * x + y * y);
		if (dis > smallDistance)
		{
			break;
		}

		// if it is not itself and nearer the query point than the previous point.
		if (dis != 0 && dis < smallDistance)
		{
			smallDistance = dis;
			smallPointIndex = GlobalData::SortedIndices[m_offset + i];
		}
	}
}