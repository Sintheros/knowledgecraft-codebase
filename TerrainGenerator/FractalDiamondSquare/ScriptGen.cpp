//ScriptGen.cpp by Ian Siczewicz
#include "ScriptGen.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

void Scriptifier();
void generateYML(string dispName, string fileName, string url, double x, double y, double z, string subject, bool isMath);
void generateMasterSentry(int entry, string fileNames[], string displayNames[], double highestX, double highestY, double highestZ, string subject);

ScriptGen::ScriptGen(void)
{
	Scriptifier();
}


ScriptGen::~ScriptGen(void)
{
}

void Scriptifier()
//int main()
{
	string subject;

	cout << "Enter subject (Math, Biology, etc. Don't put in spaces.): " << endl;
	cin >> subject;

	string buffer;
	

	bool isMath = false; //Math sentires use Khan Academy to quiz and check for completion rather than through question scripts.

	if(subject=="math" || subject=="Math" || subject=="MATH")
		isMath = true;

	string dispName;
	string fileName;
	string fileNames[1000];
	string displayNames[1000];
	string url = "PLACEHOLDER_URL";
	double x;
	double y;
	double z;
	
	double highestX;
	double highestY;
	double highestZ;

	int entry = 0;
	int ticker = 1;

	for(int i=0; i<1000; i++)
	{
		fileNames[i] = "BLANK";
		displayNames[i] = "BLANK";
	}

	ifstream input;
	input.open("Ignore.txt");
	if(input.fail())
	{
		cout << "Could not open data." << endl;
		exit(1);
	}

	while(!input.eof()) //Parse through the file created by the Terrain Generator and sort the data so it can be written into Sentires.
	{
		input >> buffer;
		//cout << buffer << endl;
		if(ticker==1 && buffer.find(",") == -1)
		{
			dispName = dispName+buffer+" ";
		}
		else if(ticker==1 && buffer.find(",") != -1)
		{
			buffer.replace(buffer.find(","), 1, "");
			dispName = dispName+buffer;
			ticker++;
		}
		else if(ticker==2)
		{
			displayNames[entry] = dispName;
			buffer.replace(buffer.find(","), 1, "");
			fileName = buffer;
			fileNames[entry] = fileName;
			ticker++;
		}
		else if(ticker==3)
		{
			buffer.replace(buffer.find(","), 1, "");
			x = atof(buffer.c_str());
			ticker++;
			
			if(entry==0)
				highestX = x;
		}
		else if(ticker==4)
		{
			buffer.replace(buffer.find(","), 1, "");
			y = atof(buffer.c_str());
			ticker++;

			if(entry==0)
				highestY = y;
		}
		else if(ticker==5)
		{
			buffer.replace(buffer.find(","), 1, "");
			z = atof(buffer.c_str());
			ticker++;

			if(entry==0)
				highestZ = z;
		}
		else if(ticker==6)
		{
			url = buffer;
			generateYML(dispName,fileName,url,x,y,z,subject,isMath);

			ticker = 1;
			dispName = "";
			entry++;
		}
	}

	input.close();
	generateMasterSentry(entry, fileNames, displayNames, highestX, highestY, highestZ, subject);
	cout << "Sentry Generation COMPLETE" << endl;

}
//Create standard sentries here. Some code is commented out temporarily until further Learning Landscape development occurs. This includes code such as preventing the player from leaving a node until they are done.
void generateYML(string dispName, string fileName, string url, double x, double y, double z, string subject, bool isMath)
{
	ofstream output;

	output.open("Sentry_"+fileName+".yml");

	output << "#This Sentry will appear at node: " << dispName << endl;
	output << "\"AssignmentSentry_" << fileName << "\":" << endl;
	output << "  Type: Assignment" << endl;
	output << "  Actions:" << endl;
	output << "    On Assignment:" << endl;
	output << "      - FLAG NPC SentryX:<FLAG.G:MasterX_" << subject << ">" << endl;
	output << "      - FLAG NPC SentryY:<FLAG.G:MasterY_" << subject << ">" << endl;
	output << "      - FLAG NPC SentryZ:<FLAG.G:MasterZ_" << subject << ">" << endl;
	output << "      - FLAG NPC \"SentryX:+:"<<x<<"\"" << endl;
	output << "      - FLAG NPC \"SentryY:+:"<<y<<"\"" << endl;
	output << "      - FLAG NPC \"SentryZ:+:"<<z<<"\"" << endl;
	output << "      - TELEPORT NPC \"location:<FLAG.NPC:SentryX>,<FLAG.NPC:SentryY>,<FLAG.NPC:SentryZ>,world\"" << endl;
	output << "      - TRIGGER NAME:Proximity TOGGLE:True RADIUS:8" << endl;
	output << "      - execute as_server \"npc select <npc.id>\"" << endl;
	output << "      - execute as_server \"npc lookclose\"" << endl;
	output << "  Interact Scripts:" << endl;
	output << "  - 1 Greeting_" << fileName << endl;
	output << "'Greeting_" << fileName << "':" << endl;
	output << "  Type: Interact" << endl;
	output << "  Requirements:" << endl;
	output << "    Mode: All" << endl;
	output << "  Steps:" << endl;
	output << "    1:" << endl;
	output << "      Chat Trigger:" << endl;
	output << "        1:" << endl;
	output << "            Trigger: /Regex:Hi|hi|Hello|hello|Hey|hey/, where am I?" << endl;
	output << "            Script:" << endl;
	output << "              - CHAT \"This peak is " << dispName << "!\"" << endl;
	output << "              - WAIT 3" << endl;
	if(isMath){
	output << "              - CHAT \"Try some practice at " << url << "\"" << endl;
	}
	if(!isMath){
	output << "              - CHAT \"You can learn more at " << url << "\"" << endl;
	output << "              - WAIT 3" << endl;
	output << "              - CHAT \"Say 'quiz' to begin!\"" << endl;
	output << "        2:" << endl;
	output << "            Trigger: /Regex:Quiz|quiz/ me!" << endl;
	output << "            Script:" << endl;
	output << "               - execute as_server \"npc select <npc.id>\"" << endl;
	output << "               - execute as_server \"npc assignment --set questions_" << fileName << endl;
	output << "               - CHAT \"Right click me for a question!\"" << endl;
	}
	output << "      Proximity Trigger:" << endl;
	output << "        entry:" << endl;
	output << "          Script:" << endl;
	output << "            - CHAT \"This peak is " << dispName << "!\"" << endl;
	output << "            - WAIT 3" << endl;
	if(isMath){
	output << "            - CHAT \"Try some practice at " << url << "\"" << endl;
	}
	if(!isMath){
	output << "            - CHAT \"You can learn more at " << url << "\"" << endl;
	output << "            - WAIT 3" << endl;
	output << "            - CHAT \"Say 'quiz' to begin!\"" << endl;
	}
  /*output << "            - IF \"<flag:Correct_" << fileName << " || 0>\" == \"0\" FLAG \"Correct_" << fileName << ":0\"" << endl;
  /*output << "        exit:" << endl;
	output << "          Script:" << endl;
	output << "            - IF \"<flag:Correct_" << fileName << " || 0>\" < \"10\" RUNTASK \"Sentry_" << fileName << "\"" << endl;
  /*output << "'Sentry_" << fileName << "':" << endl;
	output << "  Type: Task" << endl;
	output << "  Script:" << endl;
	output << "    - CHAT \"You haven't finished this area yet!\"" << endl;
	output << "    - TELEPORT \"location:<FLAG.NPC:SentryX>,<FLAG.NPC:SentryY>,<FLAG.NPC:SentryZ>,world\"" << endl;
	output << "'Yes_" << fileName << "':" << endl;
	output << "  Type: Task" << endl;
	output << "  Script:" << endl;
	output << "    - CHAT \"Correct!\"" << endl;
	output << "    - FLAG \"Correct_" << fileName << ":+:1\"" << endl;
	output << "    - CHAT \"You now have <flag:Correct_" << fileName << "> correct at this peak.\"" << endl;
	output << "'No_" << fileName << "':" << endl;
	output << "  Type: Task" << endl;
	output << "  Script:" << endl;
	output << "    - CHAT \"Sorry, that's wrong.\"" << endl;
	output << "    - WAIT 2" << endl;
	output << "    - \"Remember, you can learn more at " << url << "\"" << endl;*/

	output.close();
}
//Create the Master Sentry here.
void generateMasterSentry(int entry, string fileNames[], string displayNames[], double highestX, double highestY, double highestZ, string subject)
{
	ofstream output;

	output.open("MASTER_Sentry_"+subject+".yml");

	output << "#This is the MASTER SENTRY for " << entry << " different Sentries." << endl;
	output << "\"AssignmentSentry_MASTER_" << subject << "\":" << endl;
	output << "  Type: Assignment" << endl;
	output << "  Interact Scripts:" << endl;
	output << "  - 1 CreateSentries_" << subject << "" << endl;
	output << "'CreateSentries_" << subject << "':" << endl;
	output << "  Type: Interact" << endl;
	output << "  Requirements:" << endl;
	output << "    Mode: All" << endl;
	output << "  Steps:" << endl;
	output << "    1:" << endl;
	output << "        Chat Trigger:" << endl;
	output << "            1:" << endl;
	output << "                Trigger: /Create/ the sentries you've been assigned to create." << endl;
	output << "                Script:" << endl;
	output << "                - FLAG GLOBAL MasterX_" << subject << ":<npc.location.x>" << endl;
	output << "                - FLAG GLOBAL MasterY_" << subject << ":<npc.location.y>" << endl;
	output << "                - FLAG GLOBAL MasterZ_" << subject << ":<npc.location.z>" << endl;
	output << "                - FLAG GLOBAL \"MasterX_" << subject << ":-:"<<highestX<<"\"" << endl;
	output << "                - FLAG GLOBAL \"MasterY_" << subject << ":-:"<<highestY<<"\"" << endl;
	output << "                - FLAG GLOBAL \"MasterZ_" << subject << ":-:"<<highestZ<<"\"" << endl;
	output << "                - CHAT \"Now spawning and assigning " << entry << " sentries!\"" << endl;
	output << "                - TELEPORT NPC \"location:<FLAG.NPC:MasterX_" << subject << ">,<FLAG.NPC:MasterY_" << subject << ">,<FLAG.NPC:MasterZ_" << subject << ">,world\"" << endl;

	for(int i=0; i<1000; i++)
		if(fileNames[i]!="BLANK")
		{
			output << "                - execute as_npc \"npc create " << displayNames[i] << "\"" << endl;
			output << "                - execute as_npc \"npc assignment --set AssignmentSentry_" << fileNames[i] << "\"" << endl;
		}
	output << "                - FINISH" << endl;

	output.close();
}
