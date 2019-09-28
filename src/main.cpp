#include "../include/Parser.h"
#include "../include/Timer.h"
#include "../include/PointValidation.h"
#include <iostream>

int main(int argc, const char **argv)
{
	const std::string pointSetFilename = "../../point_set_small.txt";
	const std::string solutionFilename = "../../solution_small.txt";

	std::vector<Point> points;
	{
		//measures the time in milliseconds
		Timer timer("Point Loading");

		// reads the point data
		points = Parser::readPoints(pointSetFilename);
	}

	//empty vector to put your solution in
	std::vector<std::pair<Point, Point>> pointPairs;

	//TODO: Find the closest neighbor for each point in "points" and save it in "pointPairs"
	//e.g. pointPairs.push_back(std::pair<Point, Point>(point0, closest neighbor of point0));
	
	//validates your solution
	const std::string result = PointValidation::validatePoints(solutionFilename, pointPairs) ? "Correct" : "Wrong";
	std::cout << std::endl;
	std::cout << result << std::endl;

	int dummy;
	std::cin >> dummy;

	return 0;
}