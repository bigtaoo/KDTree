#include "../include/KdLeaf.h"
#include "../include/GlobalData.h"

KdLeaf::KdLeaf(int count, int start) : m_elementCount(count), m_startPos(start)
{
}

KdLeaf::~KdLeaf() 
{
}

void KdLeaf::Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex)
{
	int64_t dis = 0;

	for (int i = 0; i < m_elementCount; ++i)
	{
		const Point& pp = GlobalData::OriginalPoints[GlobalData::SortedIndices[m_startPos + i]];
		dis = 0;

		int64_t x = q.m_x - pp.m_x;
		int64_t y = q.m_y - pp.m_y;

		dis += (x * x + y * y);
		if (dis > smallDistance)
		{
			break;
		}

		if (dis != 0 && dis < smallDistance)
		{
			smallDistance = dis;
			smallPointIndex = GlobalData::SortedIndices[m_startPos + i];
		}
	}
}