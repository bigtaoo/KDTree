#include "../include/KdTree.h"
#include "../include/GlobalData.h"
#include "../include/KdLeaf.h"
#include "../include/KdSplit.h"

KdTree::KdTree(const std::vector<Point>& p) : m_root(nullptr)
{
	// store original points.
	GlobalData::OriginalPoints = p;

	const int len = static_cast<int>(p.size());

	// initialize the index of the original points.
	GlobalData::SortedIndices = new int[len];
	for (int i = 0; i < len; ++i)
	{
		GlobalData::SortedIndices[i] = i;
	}

	// initialize the trivial leaf.
	if (GlobalData::TrivialLeaf == nullptr)
	{
		GlobalData::TrivialLeaf = new KdLeaf(0, 0);
	}

	// get bound box.
	KdBounds boundBox;
	GetBoundBox(GlobalData::OriginalPoints, boundBox);
	m_low = boundBox.m_low;
	m_high = boundBox.m_high;

	// build the tree.
	m_root = RecursiveBuildKdTree(p, 0, len, boundBox);
}

KdTree::~KdTree() 
{
	if (m_root != nullptr)
	{
		delete m_root;
		m_root = nullptr;
	}

	// clear global data
	GlobalData::OriginalPoints.clear();
	if (GlobalData::SortedIndices != nullptr)
	{
		delete GlobalData::SortedIndices;
		GlobalData::SortedIndices = nullptr;
	}
	if (GlobalData::TrivialLeaf != nullptr)
	{
		delete GlobalData::TrivialLeaf;
		GlobalData::TrivialLeaf = nullptr;
	}
}

void KdTree::Search(const Point& q, Point& out) const
{
	int64_t dis = GetBoundDistance(q, m_low, m_high);
	int64_t smallDistance = std::numeric_limits<int64_t>::max();
	int smallPointIndex = 0;

	// recursive search.
	m_root->Search(q, dis, smallDistance, smallPointIndex);

	// get out point.
	out = GlobalData::OriginalPoints[smallPointIndex];
}

void KdTree::GetBoundBox(const std::vector<Point>& p, KdBounds& bound)
{
	// x dimension
	int32_t low = p[0].m_x;
	int32_t high = p[0].m_x;
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

void KdTree::Split(const std::vector<Point>& p, int offset, int size, const KdBounds& bound, int& cutDimension, int32_t& cutValue, int& lowPointsNumber)
{
	// determine the cut dimension.
	// compare the spread on each dimension and choose the larger one.
	int32_t maxLengthX = bound.m_high.m_x - bound.m_low.m_x;
	int32_t maxLengthY = bound.m_high.m_y - bound.m_low.m_y;
	int32_t maxLength = maxLengthX > maxLengthY ? maxLengthX : maxLengthY;
	int32_t maxSpread = -1;
	if (bound.m_high.m_x - bound.m_low.m_x >= maxLength)
	{
		int32_t spread = GetSpread(p, offset, size, 0);
		if (spread > maxSpread)
		{
			maxSpread = spread;
			cutDimension = 0;
		}
	}
	if (bound.m_high.m_y - bound.m_low.m_y >= maxLength)
	{
		int32_t spread = GetSpread(p, offset, size, 1);
		if (spread > maxSpread)
		{
			maxSpread = spread;
			cutDimension = 1;
		}
	}

	// determine the cut value.
	// the default is evenly divided, but it extends to the edge if there are no elements on one side.
	int32_t idealCutValue = (cutDimension == 1) ? ((bound.m_low.m_y + bound.m_high.m_y) / 2) : ((bound.m_low.m_x + bound.m_high.m_x) / 2);
	int32_t min, max;
	FindMaxRange(p, offset, size, cutDimension, min, max);
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

	int break1, break2;
	// p[offset..break1] < cutValue
	// p[break1..break2-1] == cutValue
	// p[break2..size-1] > cutValue
	SortSplit(p, offset, size, cutDimension, cutValue, break1, break2);

	// there is no element on left, so we choose one put in left.
	if (idealCutValue < min)
	{
		lowPointsNumber = 1;
	}
	// there is no element on right, so we choose one put in right.
	else if (idealCutValue > max)
	{
		lowPointsNumber = size - 1;
	}
	else if (break1 > size / 2)
	{
		lowPointsNumber = break1 - offset;
	}
	else if (break2 < size / 2)
	{
		lowPointsNumber = break2 - offset;
	}
	else
	{
		// all of the elements are the same.
		lowPointsNumber = size / 2;
	}
}

int32_t KdTree::GetSpread(const std::vector<Point>& p, int offset, int size, int dimension)
{
	// x-axis.
	if (dimension == 0)
	{
		int32_t min = p[offset].m_x;
		int32_t max = p[offset].m_x;

		for (int i = 1; i < size; ++i)
		{
			const int32_t& x = p[i].m_x;
			if (x < min)
			{
				min = x;
			}
			else if (x > max)
			{
				max = x;
			}
		}
		return max - min;
	}
	// y-axis.
	else if (dimension == 1)
	{
		int32_t min = p[offset].m_y;
		int32_t max = p[offset].m_y;

		for (int i = 1; i < size; ++i)
		{
			const int32_t& y = p[i].m_y;
			if (y < min)
			{
				min = y;
			}
			else if (y > max)
			{
				max = y;
			}
		}
		return max - min;
	}
	return 0;
}

void KdTree::FindMaxRange(const std::vector<Point>& p, int offset, int size, int dimension, int32_t& min, int32_t& max)
{
	// x-axis.
	if (dimension == 0)
	{
		min = p[GlobalData::SortedIndices[offset]].m_x;
		max = p[GlobalData::SortedIndices[offset]].m_x;

		for (int i = 0; i < size; ++i)
		{
			const int32_t& x = p[GlobalData::SortedIndices[offset + i]].m_x;
			if (x < min)
			{
				min = x;
			}
			else if (x > max)
			{
				max = x;
			}
		}
	}
	// y-axis.
	else if(dimension == 1)
	{
		min = p[GlobalData::SortedIndices[offset]].m_y;
		max = p[GlobalData::SortedIndices[offset]].m_y;

		for (int i = 0; i < size; ++i)
		{
			const int32_t& y = p[GlobalData::SortedIndices[offset + i]].m_y;
			if (y < min)
			{
				min = y;
			}
			else if (y > max)
			{
				max = y;
			}
		}
	}
	else
	{
		min = 0;
		max = 0;
	}
}

void KdTree::SortSplit(const std::vector<Point>& p, int offset, int size, int dimension, int32_t cutValue, int& break1, int& break2)
{
	// x-axis.
	if (dimension == 0)
	{
		// exchange smaller element to left.
		int left = offset;
		int right = offset + size - 1;
		for (;;)
		{
			while (left < (offset + size) && p[GlobalData::SortedIndices[left]].m_x < cutValue)
			{
				left++;
			}
			while (right >= 0 && p[GlobalData::SortedIndices[right]].m_x >= cutValue)
			{
				right--;
			}
			if (left > right)
			{
				break;
			}
			int temp = GlobalData::SortedIndices[left];
			GlobalData::SortedIndices[left] = GlobalData::SortedIndices[right];
			GlobalData::SortedIndices[right] = temp;

			left++;
			right--;
		}
		break1 = left;
		// now p[offset..break1-1] < cutValue <= p[break1..offset+size-1]
		right = offset + size - 1;
		for (;;)
		{
			while (left < (offset + size) && p[GlobalData::SortedIndices[left]].m_x <= cutValue)
			{
				left++;
			}
			while (right >= break1 && p[GlobalData::SortedIndices[right]].m_x > cutValue)
			{
				right--;
			}
			if (left > right)
			{
				break;
			}
			int temp = GlobalData::SortedIndices[left];
			GlobalData::SortedIndices[left] = GlobalData::SortedIndices[right];
			GlobalData::SortedIndices[right] = temp;

			left++;
			right--;
		}
		break2 = left;
		// now p[break1..break2-1] == cutValue < p[break2..offset+size-1]
	}
	// y-axis. the same as x-axis.
	else
	{
		int left = offset;
		int right = offset + size - 1;
		for (;;)
		{
			while (left < (size + offset) && p[GlobalData::SortedIndices[left]].m_y < cutValue)
			{
				left++;
			}
			while (right >= 0 && p[GlobalData::SortedIndices[right]].m_y >= cutValue)
			{
				right--;
			}
			if (left > right)
			{
				break;
			}
			int temp = GlobalData::SortedIndices[left];
			GlobalData::SortedIndices[left] = GlobalData::SortedIndices[right];
			GlobalData::SortedIndices[right] = temp;
			left++;
			right--;
		}
		break1 = left;
		right = offset + size - 1;
		for (;;)
		{
			while (left < (offset + size) && p[GlobalData::SortedIndices[left]].m_y <= cutValue)
			{
				left++;
			}
			while (right >= break1 && p[GlobalData::SortedIndices[right]].m_y > cutValue)
			{
				right--;
			}
			if (left > right)
			{
				break;
			}
			int temp = GlobalData::SortedIndices[left];
			GlobalData::SortedIndices[left] = GlobalData::SortedIndices[right];
			GlobalData::SortedIndices[right] = temp;
			left++;
			right--;
		}
		break2 = left;
	}
}

int64_t KdTree::GetBoundDistance(const Point& q, const Point& low, const Point& high) const
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

KdNode* KdTree::RecursiveBuildKdTree(const std::vector<Point>& p, int offset, int size, KdBounds& bound)
{
	// this split has no elements.
	if (size == 0)
	{
		return GlobalData::TrivialLeaf;	
	}
	// final leaf.
	else if(size == 1)
	{
		return new KdLeaf(size, offset);
	}
	// build split.
	else
	{
		int32_t cutDimension, cutValue, lowPointsNumber;
		KdNode* low, * high;

		Split(p, offset, size, bound, cutDimension, cutValue, lowPointsNumber);

		// split x-axis.
		if (cutDimension == 0)
		{
			int32_t lowValue = bound.m_low.m_x;
			int32_t highValue = bound.m_high.m_x;

			bound.m_high.m_x = cutValue;
			low = RecursiveBuildKdTree(p, offset, lowPointsNumber, bound);
			bound.m_high.m_x = highValue;

			bound.m_low.m_x = cutValue;
			high = RecursiveBuildKdTree(p, offset + lowPointsNumber, size - lowPointsNumber, bound);
			bound.m_low.m_x = lowValue;

			KdSplit *ptr = new KdSplit(cutDimension, cutValue, lowValue, highValue, low, high);
			return ptr;
		}
		// split y-axis.
		else
		{
			int32_t lowValue = bound.m_low.m_y;
			int32_t highValue = bound.m_high.m_y;

			bound.m_high.m_y = cutValue;
			low = RecursiveBuildKdTree(p, offset, lowPointsNumber, bound);
			bound.m_high.m_y = highValue;

			bound.m_low.m_y = cutValue;
			high = RecursiveBuildKdTree(p, offset + lowPointsNumber, size - lowPointsNumber, bound);
			bound.m_low.m_y = lowValue;

			KdSplit *ptr = new KdSplit(cutDimension, cutValue, lowValue, highValue, low, high);
			return ptr;
		}
	}
}