#include "../include/GlobalData.h"
#include "../include/Point.h"

KdLeaf* GlobalData::TrivialLeaf = nullptr;
std::vector<Point> GlobalData::OriginalPoints;
int* GlobalData::SortedIndices = nullptr;