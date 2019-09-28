#pragma once

#include "KdNode.h"

class KdLeaf : public KdNode
{
public:
	KdLeaf(int count, int start);
	~KdLeaf();

	void Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex) override;

private:
	int m_elementsCount;
	int m_startPos;
};