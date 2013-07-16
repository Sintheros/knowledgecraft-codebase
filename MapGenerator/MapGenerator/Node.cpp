#include "Node.h"


Node::Node(char value[], char nm[])
{
	strcpy(id, value);
	strcpy(name, nm);
	numChild = 0;
	angle = 0;
	level = 20;
	distance = 0;
	angleMax = 360;
	angleMin = 0;
	levelDiff = 1000;
}


Node::~Node(void)
{
}

void Node::setID(char value[])
{
	strcpy(id, value);
}

char* Node::getID(void)
{
	return id;
}

void Node::setName(char value[])
{
	strcpy(name, value);
}

char* Node::getName(void)
{
	return name;
}

int Node::getLevel(void)
{
	return level;
}

void Node::setLevel(int value)
{
	level = value;
}

int Node::getNumChild(void)
{
	return numChild;
}

void Node::setNumChild(int value)
{
	numChild = value;
}

float Node::getAngleMin(void)
{
	return angleMin;
}

void Node::setAngleMin(float value)
{
	angleMin = value;
}

float Node::getAngleMax(void)
{
	return angleMax;
}

void Node::setAngleMax(float value)
{
	angleMax = value;
}

float Node::getAngle(void)
{
	return angle;
}

void Node::setAngle(float value)
{
	angle = value;
}

float Node::getDistance(void)
{
	return distance;
}

void Node::setDistance(float value)
{
	distance = value;
}

float Node::getLevelDiff(void)
{
	return levelDiff;
}

void Node::setLevelDiff(float value)
{
	levelDiff = value;
}

string Node::getURL(void)
{
	return url;
}

void Node::setURL(string value)
{
	url = value;
}

void Node::addChild(Node* child)
{
	children.push_back(child);
}

void Node::addChildIDS(string value)
{
	childIDs.push_back(value);
	numChild++;
}

void Node::setChildStuff(float mHeight)
{

	//get the range of the min to max angles for this node
	float range = abs(angleMax - angleMin);

	//divide it up based on number of children
	if(numChild == 1){
		range = range/2;
	}else{
		range = range/numChild;
	}
	if (range <= 1)
		range = 1;

	//set the min/max angles of the children based on new range
	for(int i = 0; i < numChild; i++)
	{
		//check the difference in height, the child will only be modified next to
		//a parent that it is close to in height
		if(children[i]->getLevelDiff() > (getLevel() - children[i]->getLevel()))
		{
			children[i]->setLevelDiff(getLevel() - children[i]->getLevel());

			float tempA = abs(children[i]->getAngleMax() - children[i]->getAngleMin());

			//will only change the child's angles if it can be put into a smaller range
			if(tempA >= range)
			{
				if(i == 0)
				{

					//for the first child
					//set the child's minimum angle using the parents angleMin
					children[i]->setAngleMin(angleMin);
					//the max is based on the min + the range
					children[i]->setAngleMax(children[i]->getAngleMin() + range);

					//get random angle between its min/max to set for the child
					//children[i]->setAngle( (rand() % (int)(range * 1000.0f)) / 1000.0f + children[i]->getAngleMin() );
					children[i]->setAngle( children[i]->getAngleMin() + range/2);
				}
				else
				{
					//for subsequent children, start angles from the previous child's max
					children[i]->setAngleMin(children[i - 1]->getAngleMax());
					children[i]->setAngleMax(children[i]->getAngleMin() + range);

					//get random angle between its min/max to set for the child
					//children[i]->setAngle( (rand() % (int)(range * 1000.0f)) / 1000.0f + children[i]->getAngleMin() );
					children[i]->setAngle( children[i]->getAngleMin() + range/2);
				}
			}


			//get distance from top node, every level is another 1
			float dist = (mHeight - children[i]->y) * DISTANCE;
			children[i]->setDistance(dist);
		}
	}
	for(int i = 0; i < numChild; i++)
	{
		if(children[i]->getLevelDiff() >= (getLevel() - children[i]->getLevel()))
		{
			//get the x- and z-positions for the child based on the angle and distance
			//children's x = parent's x + cos(children's angle) * distance
			children[i]->x = 0 + cos( (children[i]->getAngle()) * (3.14159265359 / 180) ) *  children[i]->getDistance();
			children[i]->z = 0 - sin( (children[i]->getAngle()) * (3.14159265359 / 180) ) *  children[i]->getDistance();


			children[i]->x = children[i]->x * XMULTI;
			children[i]->z = children[i]->z * ZMULTI;
		}
	}
}

//operator overload, to compare Nodes in Tree.cpp
bool Node::operator<(Node& other)
{
	if(y >= other.y)
		return true;
	return false;
}