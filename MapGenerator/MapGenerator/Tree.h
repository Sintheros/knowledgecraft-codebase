#pragma once
#include "Node.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm> 

using namespace std;

#define HEIGHTMULTI 0.025;
//#define HEIGHTMULTI 0.05;
#define XMULTI .005;
#define ZMULTI .005;

#define DISTANCE 1;
/*
struct Coord {
	float x;
	float y;
	float z;
	string url;
	string disName;
	string fileName;

	Coord() : x(0.0f), y(0.0f), z(0.0f) { }
};

struct Point {
	float _x;
	float _y;
	float _z;
	string _url;
	string _disName;
	string _fileName;
	//string name; //TODO
	vector<Coord> children;

	Point(float x, float y, float z, string url, string disName, string fileName) : 
		_x(x), _y(y), _z(z), _url(url), _disName(disName), _fileName(fileName) { }
};
*/
class Tree
{
public:
	Tree(void);
	~Tree(void);

	//vector<Point> setUp(void);
	int main();
};