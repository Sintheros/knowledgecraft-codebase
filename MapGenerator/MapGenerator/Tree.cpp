#include "Tree.h"

//initialize root
Node* rootPtr = NULL;



Tree::Tree(void)
{
}


Tree::~Tree(void)
{
}

//used to compare Nodes based on height (operator overloaded in Node.cpp)
bool compareNodes2 (Node a, Node b) { return a.y > b.y; }
bool compareNodes3 (Node a, Node b) { return a.y < b.y; }

int main(int argc, char** argv)
{
	vector<Node> nodes;

	srand (time(NULL));
	
	//get filename from user
	cout << "Enter file name for input:\n";
	char filename[100];
	cin >> filename;

	ifstream dFile;
	dFile.open(filename);

	//char to store the lines
	char line[2000];
	int i = 0;

	while(true)
	{
		dFile.getline(line, 2000);

		//break when we achieve end of file
		if(dFile.eof()) 
			break;
			
		string temp(line);
		
		//read in and store info from txt file
		//check if current line contains DisplayName, the starting point
		if(temp.find("DisplayName") != temp.npos) {
			//get the position of the first quotation mark
			int pos = temp.find_first_of("\"") + 1;
			//use that position to delimit the line and store it
			string toPushName(temp.substr(pos, temp.find_first_of("\"", pos) - pos));

			//go to next line, which should be the filename
			dFile.getline(line, 2000);
			string fileName(line);

			//do the same as above to store it
			int filePos = fileName.find_first_of("\"") + 1;
			string toPushFile(fileName.substr(filePos, fileName.find_first_of("\"", filePos) - filePos));

			//next line is the URL
			dFile.getline(line, 2000);
			string earl(line);

			//done a little differently, but basically the same as above
			//use positions in the string to delimit
			int urlPos = earl.find_first_of("\"");
			urlPos++;
			int urlPos2 = earl.find_first_of("\"", urlPos);

			//get the prereqs line for later
			dFile.getline(line, 2000);
			string prereqs(line);
			
			//get coordinates, not actually necessary anymore
			//delete if you wish
			/*
			dFile.getline(line, 2000);
			string coordinates(line);
			int parenPos = coordinates.find_first_of("(", 11) + 1;
			coordinates = coordinates.substr(parenPos, coordinates.find_first_of(")", parenPos) - parenPos);
			*/
			/*
			int commaPos = coordinates.find_first_of(",");
			float xp = atof(coordinates.substr(0, commaPos).c_str());
			float zp = atof(coordinates.substr(commaPos + 1, 12).c_str());
			*/
			
			//initialize a new Node with the displayname and filename
			Node n(const_cast<char*>(toPushFile.c_str()), const_cast<char*>(toPushName.c_str()));
			nodes.push_back(n);

			//initial position
			nodes[i].x = 0;
			nodes[i].z = 0;

			//get and set the Node's URL
			string url(earl.substr(urlPos, urlPos2 - urlPos));
			nodes[i].setURL(url);

			//use a loop to get all the prereqs separatly 
			while(true) {
				
				//same as above, search for the first and then second set
				//of quotation marks
				int reqPos = prereqs.find_first_of("\"");
				//break if there are no more
				if(reqPos == prereqs.npos)
					break;
				++reqPos;
				int reqPos2 = prereqs.find_first_of("\"", reqPos);
				string child(prereqs.substr(reqPos, reqPos2 - reqPos));
				
				//add the child's ID to the current Node
				nodes[i].addChildIDS(child);
				//start the prereqs string after this prereq so that we don't run into it again
				prereqs = prereqs.substr(reqPos2 + 1, prereqs.npos);
			}

			++i;
		}
	}

	vector<string> nodeIDs;

	//loop through the nodes and create an array of all their id's
	for( int j = 0; j < nodes.size(); j++)
	{
		nodeIDs.push_back(nodes[j].getID());
	}

	//loop through all nodes
	for( int k = 0; k < nodes.size(); k++)
	{
		
		//loop through the nodes' children
		for ( int l = 0; l < nodes[k].getNumChild(); l++)
		{
			//store the child Id
			string guy = nodes[k].childIDs[l];
			
			bool foundGuy = false;

			//loop through all the node Id's...
			for(i = 0; i < nodeIDs.size(); ++i) {
				//and compare their id's with the child Id's
				if(guy.compare(nodeIDs[i]) == 0) {
					//if you find one break, this keeps the 'i' variable, which keeps the position in the array
					foundGuy = true;
					break;
				}
			}

			if(foundGuy) {
				//add the corresponding child's reference with the 'i' value above
				nodes[k].addChild(&(nodes[i]));
			}
		}
		//if the node has children
		if(nodes[k].children.size() != 0)
		{
			//loop through children
			for(i = 0; i < nodes[k].children.size(); ++i) {
				//increase the level of the current node based on the child's level
				int level = nodes[k].children[i]->getLevel() + 1;
				//this checks the current level above with the stored node level
				//it then stores the bigger one
				if(level > nodes[k].getLevel())
					nodes[k].setLevel(level);
			}
		}

		//get y value
		nodes[k].y = nodes[k].getLevel() * HEIGHTMULTI;
	}

	dFile.close();

	//sort nodes from highest to smallest using overloaded operator from Node.h
	std::sort(nodes.begin(), nodes.end(), compareNodes2);


	//the sort causes the child pointers to be wrong, so we have to re-do them
	nodeIDs.clear();

	//loop through the nodes and create an array of all their id's
	for( int j = 0; j < nodes.size(); j++)
	{
		nodeIDs.push_back(nodes[j].getID());
	}




	//loop through all nodes
	for( int k = 0; k < nodes.size(); k++)
	{
		nodes[k].children.clear();
		//loop through the nodes' children
		for ( int l = 0; l < nodes[k].getNumChild(); l++)
		{
			//store the child Id
			string guy = nodes[k].childIDs[l];
			
			bool foundGuy = false;

			//loop through all the node Id's...
			for(i = 0; i < nodeIDs.size(); ++i) {
				//and compare their id's with the child Id's
				if(!guy.compare(nodeIDs[i])) {
					//if you find one break, this keeps the 'i' variable, which keeps the position in the array
					foundGuy = true;
					break;
				}
			}

			if(foundGuy) {
				//add the corresponding child's reference with the 'i' value above
				nodes[k].addChild(&(nodes[i]));
			}
		}
		

	}





	nodes[0].x = 0;
	nodes[0].z = 0;

	for( int k = 0; k < nodes.size(); k++)
	{
		//call the function to set children positions and stuff
		nodes[k].setChildStuff(nodes[0].y);
		
		//cout << k << ": " << nodes[k].getAngleMin() << ", " << nodes[k].getAngleMax() << ", " << nodes[k].x << ", " 
		//	<< nodes[k].z << ", " << nodes[k].y << ", " << nodes[k].getURL().c_str() << endl;
	}

	//std::sort(nodes.begin(), nodes.end(), compareNodes3);
	/*
	vector<Point> po;
	for(i = 0; i < nodes.size(); ++i) {
		//create a point for each node, store into the vector of Points
		Point p(nodes[i].x, nodes[i].y, nodes[i].z, nodes[i].getURL(), nodes[i].getName(), nodes[i].getID());

		if(i < 25)
		cout << "Node " << nodes[i].getID() << " has coords of " << nodes[i].x << "," << nodes[i].y << "," << nodes[i].z << endl;
		for(int j = 0; j < nodes[i].children.size(); ++j) {

			Coord c;
			if(i < 25)
			cout << "\tChild node " << nodes[i].children[j]->getID() << " has coords of " << nodes[i].children[j]->x << "," << nodes[i].children[j]->y << "," << nodes[i].children[j]->z << endl;
			c.x = nodes[i].children[j]->x;
			c.y = nodes[i].children[j]->y;
			c.z = nodes[i].children[j]->z;
			c.fileName = nodes[i].children[j]->getID();
			p.children.push_back(c);
		}

		po.push_back(p);
	}
	*/
	cout << "File read!" << endl;

	//get output filename from user
	cout << "Enter file name for output:\n";
	char filenm[100];
	cin >> filenm;

	ofstream oFile;
	oFile.open(filenm);

	cout << "Writing..." << endl;

	//more simple I/O, just writes out every Node's info into the output text file
	for( int l = 0; l < nodes.size(); l++ )
	{
		oFile << "DisplayName: \"" << nodes[l].getName() << "\"" << endl;
		oFile << "FileName: \"" << nodes[l].getID() << "\"" << endl;
		oFile << "URL: \"" << nodes[l].getURL().c_str() << "\"" << endl;
		oFile << "Coordinates: (" << nodes[l].x << "," << nodes[l].y << "," << nodes[l].z << ")" << endl;

		for(int j = 0; j < nodes[l].children.size(); ++j) 
		{
			oFile << "Child Coordinates: (" << nodes[l].children[j]->x << "," << nodes[l].children[j]->y
				<< "," << nodes[l].children[j]->z << ")" << endl;
		}

		if(nodes[l].children.size() == 0)
		{
			oFile << "Child Coordinates: (N/A)" << endl;
		}

		oFile << "\n";
	}

	cout << "Done writing..." << endl;
	oFile.close();

	return 0;
}
