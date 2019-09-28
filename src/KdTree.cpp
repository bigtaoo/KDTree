#include "../include/KdTree.h"
#include "../include/GlobalData.h"
#include "../include/KdLeaf.h"
#include "../include/KdSplit.h"

KdTree::KdTree(const std::vector<Point>& p) : m_root(nullptr)
{
	GlobalData::OriginalPoints = p;
	const int len = static_cast<int>(p.size());

	GlobalData::SortedIndices = new int[len];
	for (int i = 0; i < len; ++i)
	{
		GlobalData::SortedIndices[i] = i;
	}

	if (GlobalData::TrivialLeaf == nullptr)
	{
		GlobalData::TrivialLeaf = new KdLeaf(0, 0);
	}

	KdBounds boundBox;
	EnclosingRectangle(GlobalData::OriginalPoints, boundBox);

	m_low = boundBox.m_low;
	m_high = boundBox.m_high;

	m_root = RecursiveBuildKdTree(p, 0, len, 2, 1, boundBox);
}

KdTree::~KdTree() 
{
	if (m_root != nullptr)
	{
		delete m_root;
		m_root = nullptr;
	}
}

void KdTree::Search(const Point& q, Point& out) const
{
	int64_t dis = BoxDistance(q, m_low, m_high);
	int64_t smallDistance = std::numeric_limits<int64_t>::max();
	int smallPointIndex = 0;

	m_root->Search(q, dis, smallDistance, smallPointIndex);

	out = GlobalData::OriginalPoints[smallPointIndex];
}

void KdTree::EnclosingRectangle(std::vector<Point>& p, KdBounds& bound)
{
	// x dimension
	int low = p[0].m_x;
	int high = p[0].m_x;
	const int len = static_cast<int>(p.size());
	for (int i = 0; i < len; ++i)
	{
		if (p[i].m_x < low)
		{
			low = p[i].m_x;
		}
		else if (p[i].m_x > high)
		{
			high = p[i].m_x;
		}
	}
	bound.m_low.m_x = low;
	bound.m_high.m_x = high;

	// y dimension
	low = p[0].m_y;
	high = p[0].m_y;
	for (int i = 0; i < len; ++i)
	{
		if (p[i].m_y < low)
		{
			low = p[i].m_y;
		}
		else if (p[i].m_y > high)
		{
			high = p[i].m_y;
		}
	}
	bound.m_low.m_y = low;
	bound.m_high.m_y = high;
}

void KdTree::Split(const std::vector<Point>& p, int* pIndices, int start, int n, int Dimension, const KdBounds& bound, int& cutDim, int& cutValue, int& lowPointsNumber)
{
	int maxLengthX = bound.m_high.m_x - bound.m_low.m_x;
	int maxLengthY = bound.m_high.m_y - bound.m_low.m_y;
	int maxLength = maxLengthX > maxLengthY ? maxLengthX : maxLengthY;

	int maxSpread = -1;

	if (bound.m_high.m_x - bound.m_low.m_x >= maxLength)
	{
		int spread = Spread(p, pIndices, start, n, 0);
		if (spread > maxSpread)
		{
			maxSpread = spread;
			cutDim = 0;
		}
	}
	if (bound.m_high.m_y - bound.m_low.m_y >= maxLength)
	{
		int spread = Spread(p, pIndices, start, n, 1);
		if (spread > maxSpread)
		{
			maxSpread = spread;
			cutDim = 1;
		}
	}

	int idealCutValue = (bound.m_low.m_x + bound.m_high.m_x) / 2;
	if (cutDim == 1)
	{
		idealCutValue = (bound.m_low.m_y + bound.m_high.m_y) / 2;
	}

	int min, max;
	MinMax(p, pIndices, start, n, cutDim, min, max);

	if (idealCutValue < min)
	{
		cutValue = min;
	}
	else if (idealCutValue > max)
	{
		cutValue = max;
	}
	else
	{
		cutValue = idealCutValue;
	}

	int br1, br2;
	PlaneSplit(p, pIndices, start, n, cutDim, cutValue, br1, br2);

	if (idealCutValue < min)
	{
		lowPointsNumber = 1;
	}
	else if (idealCutValue > max)
	{
		lowPointsNumber = n - 1;
	}
	else if (br1 > n / 2)
	{
		lowPointsNumber = br1 - start;
	}
	else if (br2 < n / 2)
	{
		lowPointsNumber = br2 - start;
	}
	else
	{
		lowPointsNumber = n / 2;
	}
}

int KdTree::Spread(const std::vector<Point>& p, int* pIndices, int start, int n, int d)
{
	if (d == 0)
	{
		int min = p[start].m_x;
		int max = p[start].m_x;

		for (int i = 1; i < n; ++i)
		{
			int c = p[i].m_x;
			if (c < min)
			{
				min = c;
			}
			else if (c > max)
			{
				max = c;
			}
		}
		return max - min;
	}

	int min = p[start].m_y;
	int max = p[start].m_y;

	for (int i = 1; i < n; ++i)
	{
		int c = p[i].m_y;
		if (c < min)
		{
			min = c;
		}
		else if (c > max)
		{
			max = c;
		}
	}
	return max - min;
}

void KdTree::MinMax(const std::vector<Point>& p, int* pIndices, int start, int n, int d, int& min, int& max)
{
	if (d == 0)
	{
		min = p[pIndices[start]].m_x;
		max = p[pIndices[start]].m_x;

		for (int i = 0; i < n; ++i)
		{
			int c = p[pIndices[start + i]].m_x;
			if (c < min)
			{
				min = c;
			}
			else if (c > max)
			{
				max = c;
			}
		}
	}
	else
	{
		min = p[pIndices[start]].m_y;
		max = p[pIndices[start]].m_y;

		for (int i = 0; i < n; ++i)
		{
			int c = p[pIndices[start + i]].m_y;
			if (c < min)
			{
				min = c;
			}
			else if (c > max)
			{
				max = c;
			}
		}
	}
}

void KdTree::PlaneSplit(const std::vector<Point>& p, int* pIndices, int start, int n, int d, int cv, int& br1, int& br2)
{
	if (d == 0)
	{
		int l = start;
		int r = start + n - 1;
		for (;;)
		{
			while (l < (start + n) && p[pIndices[l]].m_x < cv)
			{
				l++;
			}
			while (r >= 0 && p[pIndices[r]].m_x >= cv)
			{
				r--;
			}
			if (l > r)
			{
				break;
			}
			int temp = pIndices[l];
			pIndices[l] = pIndices[r];
			pIndices[r] = temp;

			l++;
			r--;
		}
		br1 = l;
		r = start + n - 1;
		for (;;)
		{
			while (l < (start + n) && p[pIndices[l]].m_x <= cv)
			{
				l++;
			}
			while (r >= br1 && p[pIndices[r]].m_x > cv)
			{
				r--;
			}
			if (l > r)
			{
				break;
			}
			int temp = pIndices[l];
			pIndices[l] = pIndices[r];
			pIndices[r] = temp;

			l++;
			r--;
		}
		br2 = l;
	}
	else
	{
		int l = start;
		int r = start + n - 1;
		for (;;)
		{
			while (l < (n + start) && p[pIndices[l]].m_y < cv)
			{
				l++;
			}
			while (r >= 0 && p[pIndices[r]].m_y >= cv)
			{
				r--;
			}
			if (l > r)
			{
				break;
			}
			int temp = pIndices[l];
			pIndices[l] = pIndices[r];
			pIndices[r] = temp;
			l++;
			r--;
		}
		br1 = l;
		r = start + n - 1;
		for (;;)
		{
			while (l < (start + n) && p[pIndices[l]].m_y <= cv)
			{
				l++;
			}
			while (r >= br1 && p[pIndices[r]].m_y > cv)
			{
				r--;
			}
			if (l > r)
			{
				break;
			}
			int temp = pIndices[l];
			pIndices[l] = pIndices[r];
			pIndices[r] = temp;
			l++;
			r--;
		}
		br2 = l;
	}
}

int64_t KdTree::BoxDistance(const Point& q, const Point& low, const Point& high) const
{
	int64_t distance = 0;
	int64_t t;

	if (q.m_x < low.m_x)
	{
		t = int64_t(low.m_x) - int64_t(q.m_x);
		distance += (t * t);
	}
	else if (q.m_x > high.m_x)
	{
		t = int64_t(q.m_x) - int64_t(high.m_x);
		distance += (t * t);
	}

	if (q.m_y < low.m_y)
	{
		t = int64_t(low.m_y) - int64_t(q.m_y);
		distance += (t * t);
	}
	else if (q.m_y > high.m_y)
	{
		t = int64_t(q.m_y) - int64_t(high.m_y);
		distance += (t * t);
	}

	return distance;
}

KdNode* KdTree::RecursiveBuildKdTree(const std::vector<Point>& p, int lowPos, int n, int Dimension, int bucketSpace, KdBounds& bound)
{
	if (n <= bucketSpace)
	{
		if (n == 0)
		{
			return GlobalData::TrivialLeaf;
		}
		else
		{
			return new KdLeaf(n, lowPos);
		}
	}
	else
	{
		int cutDimension;
		int cutValue;
		int lowPointsNumber;
		KdNode* low;
		KdNode* high;

		Split(p, GlobalData::SortedIndices, lowPos, n, Dimension, bound, cutDimension, cutValue, lowPointsNumber);

		if (cutDimension == 0)
		{
			int lowValue = bound.m_low.m_x;
			int highValue = bound.m_high.m_x;

			bound.m_high.m_x = cutValue;
			low = RecursiveBuildKdTree(p, lowPos, lowPointsNumber, Dimension, bucketSpace, bound);
			bound.m_high.m_x = highValue;

			bound.m_low.m_x = cutValue;
			high = RecursiveBuildKdTree(p, lowPos + lowPointsNumber, n - lowPointsNumber, Dimension, bucketSpace, bound);
			bound.m_low.m_x = lowValue;

			KdSplit *ptr = new KdSplit(cutDimension, cutValue, lowValue, highValue, low, high);
			return ptr;
		}
		else
		{
			int lowValue = bound.m_low.m_y;
			int highValue = bound.m_high.m_y;

			bound.m_high.m_y = cutValue;
			low = RecursiveBuildKdTree(p, lowPos, lowPointsNumber, Dimension, bucketSpace, bound);
			bound.m_high.m_y = highValue;

			bound.m_low.m_y = cutValue;
			high = RecursiveBuildKdTree(p, lowPos + lowPointsNumber, n - lowPointsNumber, Dimension, bucketSpace, bound);
			bound.m_low.m_y = lowValue;

			KdSplit *ptr = new KdSplit(cutDimension, cutValue, lowValue, highValue, low, high);
			return ptr;
		}
	}
}