#pragma once
#include <cstddef>
#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>  

using namespace std;

#define DISTANCE 1;
#define XMULTI 500;
#define ZMULTI 500;

class Node
{
private:

	char id[500];
	char name[500];
	int level;
	int numChild;

	float levelDiff;

	float angle;

	float distance;

	string url;

public:
	Node(char value[], char nm[]);
	~Node(void);

	int index;

	float angleMin;
	float angleMax;

	float x;
	float y;
	float z;
	vector<Node*> children;
	vector<string> childIDs;

	char* getID(void);
	void setID(char value[]);

	char* getName(void);
	void setName(char value[]);

	int getLevel(void);
	void setLevel(int value);

	int getNumChild(void);
	void setNumChild(int value);

	float getAngle(void);
	void setAngle(float value);

	float getAngleMin(void);
	void setAngleMin(float value);

	float getAngleMax(void);
	void setAngleMax(float value);

	float getDistance(void);
	void setDistance(float value);

	float getLevelDiff(void);
	void setLevelDiff(float value);

	string getURL(void);
	void setURL(string value);

	void addChild(Node* child);
	void addChildIDS(string value);

	void setChildStuff(float mHeight);

	bool operator<(Node& other);
};
