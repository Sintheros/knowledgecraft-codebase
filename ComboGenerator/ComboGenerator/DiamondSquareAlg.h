#pragma once

#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm> 

#define HEIGHTMULTI 0.025;
//#define HEIGHTMULTI 0.05;
#define XMULTI .005;
#define ZMULTI .005;

#define DISTANCE 1;

using namespace std;

class DiamondSquareAlg
{
public:
	DiamondSquareAlg(void);
	~DiamondSquareAlg(void);
	int main();
	void treeBuilder();
};

