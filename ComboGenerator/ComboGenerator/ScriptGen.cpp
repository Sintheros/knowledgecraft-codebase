//ScriptGen.cpp by Ian Siczewicz
#include "ScriptGen.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "curl/curl.h"

using namespace std;

void Scriptifier();
void populate(bool isMath);
string truncURL(string url);
void generateYML(string dispName, string fileName, string url, string dispUrl, double x, double y, double z, string subject, bool isMath, bool isReading, int numPreReqs, string preReqs[], string associatedDispNames[], string associatedUrls[]);
void generateGrad(double x, double y,double z, string subject, int numPreReqs, string preReqs[], string associatedDispNames[] , string associatedUrls[]);
void generateMasterSentry(int entry, string displayNamesSmall[], double highestX, double highestY, double highestZ, string subject);

string const KALITEURL = "http://129.21.142.218:8008";
string const URLFUNCTION = "http://129.21.142.218:29876/KALITE?userName=<player.name>&HREF=";
string fileNames[1000];
string displayNames[1000];
string displayNamesSmall[1000];
string urls[1000];

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
	getline(cin,subject);

	string buffer;


	bool isMath = false; //Math sentires use Khan Academy to quiz and check for completion rather than through question scripts.
	bool isReading = false; //Reading sentries don't auto-check submissions or have traditional quizzes.

	if(subject=="math" || subject=="Math" || subject=="MATH")
		isMath = true;
	if(subject=="reading" || subject=="Reading" || subject=="READING")
		isReading = true;

	string dispName;
	string fileName;
	string associatedDispNames[1000];
	string associatedUrls[1000];
	string url = "PLACEHOLDER_URL";
	string dispUrl = "PLACEHOLDER_DISP";
	double x;
	double y;
	double z;
	string preReqs[100];

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
	populate(isMath);
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
			buffer.replace(buffer.find(","), 1, "");
			fileName = buffer;
			while(fileName.find_first_of(".") != string::npos )
				fileName.replace(fileName.find("."), 1, "");
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
			buffer.replace(buffer.find(","), 1, "");
			url = buffer;
			dispUrl = truncURL(url);
			if(isMath)
				if(url.find_first_of(KALITEURL) != string::npos )
					url.replace(url.find(KALITEURL), 27, "");
			ticker++;

		}
		else if(ticker==7)
		{
			for(int i=0; i<1000; i++)
				associatedDispNames[i] = "";
			for(int i=0; i<100; i++)
				preReqs[i] = "";
			bool allPreReqs = false;
			int numPreReqs = 0;
			string preReqNext = buffer;

			if(preReqNext.find("N/A") == string::npos)
				numPreReqs++;
			else
			{
				preReqs[0] = "N/A";
				allPreReqs = true;
			}

			while(!allPreReqs)
			{
				if(numPreReqs >= 1)
				{
					preReqs[numPreReqs-1] = preReqNext;
					preReqs[numPreReqs-1].replace(preReqs[numPreReqs-1].find("\""), 1, "");
					preReqs[numPreReqs-1].replace(preReqs[numPreReqs-1].find("\""), 1, "");
					if(numPreReqs == 1)
						preReqs[numPreReqs-1].replace(preReqs[numPreReqs-1].find("["), 1, "");
					if(preReqs[numPreReqs-1].find(",") != string::npos)
					{
						preReqs[numPreReqs-1].replace(preReqs[numPreReqs-1].find(","), 1, "");
						for(int i=0; i<1000; i++)
							if(fileNames[i] == preReqs[numPreReqs-1])
							{
								associatedDispNames[numPreReqs-1] = displayNamesSmall[i];
								//associatedDispNames[numPreReqs-1] = displayNames[i];
								associatedUrls[numPreReqs-1] = urls[i];
							}
						numPreReqs++;
						input >> preReqNext;
					}
					else
					{
						preReqs[numPreReqs-1].replace(preReqs[numPreReqs-1].find("]"), 1, "");
						for(int i=0; i<1000; i++)
							if(fileNames[i] == preReqs[numPreReqs-1])
							{
								associatedDispNames[numPreReqs-1] = displayNamesSmall[i];
								//associatedDispNames[numPreReqs-1] = displayNames[i];
								associatedUrls[numPreReqs-1] = urls[i];
							}
						allPreReqs = true;
					}
					
				}
			}

			if(fileName.find("graduation") != string::npos)
				generateGrad(x,y,z,subject,numPreReqs,preReqs,associatedDispNames,associatedUrls);
			else
				generateYML(dispName,fileName,url,dispUrl,x,y,z,subject,isMath,isReading,numPreReqs,preReqs,associatedDispNames,associatedUrls);
			cout << fileName << " Sentry Generated." << endl;

			ticker = 1;
			dispName = "";
			entry++;
		}
	}

	input.close();
	generateMasterSentry(entry, displayNamesSmall, highestX, highestY, highestZ, subject);
	cout << "Sentry Generation COMPLETE" << endl;

}

void populate(bool isMath)
{
	ifstream input;
	input.open("Ignore.txt");
	if(input.fail())
	{
		cout << "Could not open data." << endl;
		exit(1);
	}
	int ticker = 1;
	int entry = 0;
	string DN="";
	string FN="";
	string U="";
	string buffer;
	string PRdump[1000];
	while(!input.eof())
	{
		input >> buffer;
		if(ticker==1 && buffer.find(",") == -1)
		{
			DN = DN+buffer+" ";
		}
		else if(ticker==1 && buffer.find(",") != -1)
		{
			buffer.replace(buffer.find(","), 1, "");
			DN = DN+buffer;
			ticker++;
		}
		else if(ticker==2)
		{
			displayNames[entry] = DN;

			string dispNameSmall = DN;
			while(dispNameSmall.length()>16)
			{
				cout << "[WARNING]: Display name length exceeds 16, please rename: " << dispNameSmall << endl;
				getline(cin,dispNameSmall);
			}
			displayNamesSmall[entry] = dispNameSmall;
			
			buffer.replace(buffer.find(","), 1, "");
			FN = buffer;
			while(FN.find_first_of(".") != string::npos )
				FN.replace(FN.find("."), 1, "");
			fileNames[entry] = FN;
			ticker++;
		}
		else if(ticker==6)
		{
			buffer.replace(buffer.find(","), 1, "");
			U = buffer;
			if(isMath)
				if(U.find_first_of(KALITEURL) != string::npos )
					U.replace(U.find(KALITEURL), 27, "");
			urls[entry] = U;
			ticker++;
		}
		else if(ticker==7)
		{
			bool allPreReqs = false;
			int numPreReqs = 0;
			string preReqNext = buffer;
			if(preReqNext.find("N/A") == string::npos)
				numPreReqs++;
			else
				PRdump[0] = "N/A";

			while(!allPreReqs)
			{
				if(numPreReqs >= 1)
				{
					PRdump[numPreReqs-1] = preReqNext;
					PRdump[numPreReqs-1].replace(PRdump[numPreReqs-1].find("\""), 1, "");
					PRdump[numPreReqs-1].replace(PRdump[numPreReqs-1].find("\""), 1, "");
					if(numPreReqs == 1)
						PRdump[numPreReqs-1].replace(PRdump[numPreReqs-1].find("["), 1, "");
					if(PRdump[numPreReqs-1].find(",") != string::npos)
					{
						PRdump[numPreReqs-1].replace(PRdump[numPreReqs-1].find(","), 1, "");
						numPreReqs++;
						input >> preReqNext;
					}

					else
					{
						PRdump[numPreReqs-1].replace(PRdump[numPreReqs-1].find("]"), 1, "");
						allPreReqs = true;
					}
				}
				else
					allPreReqs = true;
			}
	
			ticker = 1;
			entry++;
			DN = "";
			FN = "";
		}
		else
			ticker++;
		
	}
	input.close();
}

//Truncate URLs to allow them to fit better into Minecraft
string truncURL(string url)
{
	ofstream output;

	output.open("temp.txt");
	output.close();

	output.open("curlbat.bat");
	output << "curl us.curlsh.me?url=" << url << " >> \"temp.txt\"";
	output.close();
	
	system("curlbat.bat");
	
	ifstream input;
	input.open("temp.txt");
	string dispUrl;
	input >> dispUrl;
	input.close();
	
	return dispUrl;
}

//Create standard sentries here. Some code is commented out temporarily until further Learning Landscape development occurs. This includes code such as preventing the player from leaving a node until they are done.
void generateYML(string dispName, string fileName, string url, string dispUrl, double x, double y, double z, string subject, bool isMath, bool isReading, int numPreReqs, string preReqs[], string associatedDispNames[], string associatedUrls[])
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
	output << "    On Unavailable:" << endl;
	output << "      - CHAT \"I'm busy! Just a moment, please!\"" << endl;
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
	output << "              - ENGAGE" << endl;
	output << "              - CHAT \"This is the home of " << dispName << "!\"" << endl;
	output << "              - WAIT 2" << endl;
	if(isMath){
	output << "              - RUNTASK \"Sentry_" << fileName << "_KA\"" << endl;
	}
	else if(isReading){
	output << "              - CHAT \"Here you can...\"" << endl;
	output << "              - WAIT 2" << endl;
	output << "              - CHAT \"...read the story\"" << endl;
	output << "              - WAIT 2" << endl;
	output << "              - CHAT \"...record the story\"" << endl;
	output << "              - WAIT 2" << endl;
	output << "              - CHAT \"...or share your recording with me for a reward!\"" << endl;
	output << "              - WAIT 3" << endl;
	output << "              - CHAT \"If you want to give me a recording, say <Gold>submit<green> so I can hear it!\"" << endl;
	output << "              - WAIT 3" << endl;
	output << "              - CHAT \"Otherwise, <white>click<green> this link to the story! "<< dispUrl << "\"" << endl;
	output << "              - DISENGAGE" << endl;
	output << "        2:" << endl;
	output << "            Trigger: I'm ready to /Regex:Submit|submit/ my reading!" << endl;
	output << "            Script:" << endl;
	output << "               - ENGAGE" << endl;
	output << "               - CHAT \"Great! Let's hear it! <Gold>[Paste your recording's URL!]<green>\"" << endl;
	output << "               - DISENGAGE" << endl;
	output << "        3:" << endl;             
    output << "            Trigger: /REGEX:www.astorybeforebed.+/" << endl;
	output << "            Script:" << endl;
	output << "               - ENGAGE" << endl;
    output << "               - FLAG NPC <player.name>:<player.chat_history>" << endl;
	output << "               - TWEET \"Check it out! <player.name> reads " << dispName << "! <FLAG.N:<player.name>>\" learninglandscape"<< endl;
	output << "               - CHAT \"Got it, thanks!\"" << endl;
	output << "               - DISENGAGE" << endl;
	}
	else{
	output << "              - CHAT \"You can learn more at " << dispUrl << "\"" << endl;
	output << "              - WAIT 3" << endl;
	output << "              - CHAT \"Say <Gold>quiz<green> to begin!\"" << endl;
	output << "              - DISENGAGE" << endl;
	output << "        2:" << endl;
	output << "            Trigger: /Regex:Quiz|quiz/ me!" << endl;
	output << "            Script:" << endl;
	output << "               - ENGAGE" << endl;
	output << "               - execute as_server \"npc select <npc.id>\"" << endl;
	output << "               - execute as_server \"npc assignment --set questions_" << fileName << endl;
	output << "               - CHAT \"Right click me for a question!\"" << endl;
	output << "               - DISENGAGE" << endl;
	}
	output << "      Proximity Trigger:" << endl;
	output << "        entry:" << endl;
	output << "          Script:" << endl;
	output << "            - ENGAGE" << endl;
	output << "            - CHAT \"This is the home of " << dispName << "!\"" << endl;
	output << "            - WAIT 2" << endl;
	if(isMath){
	output << "            - RUNTASK \"Sentry_" << fileName << "_KA\"" << endl;
	}
	else if(isReading){
	output << "            - CHAT \"Here you can...\"" << endl;
	output << "            - WAIT 2" << endl;
	output << "            - CHAT \"...read the story\"" << endl;
	output << "            - WAIT 2" << endl;
	output << "            - CHAT \"...record the story\"" << endl;
	output << "            - WAIT 2" << endl;
	output << "            - CHAT \"...or share your recording with me for a reward!\"" << endl;
	output << "            - WAIT 3" << endl;
	output << "            - CHAT \"If you want to give me a recording, say <gold>submit<green> so I can hear it!\"" << endl;
	output << "            - WAIT 3" << endl;
	output << "            - CHAT \"Otherwise, <white>click<green> this link to the story! "<< dispUrl << "\"" << endl;
	output << "            - DISENGAGE" << endl;
	}
	else{
	output << "            - CHAT \"You can learn more at " << dispUrl << "\"" << endl;
	output << "            - WAIT 3" << endl;
	output << "            - CHAT \"Say <gold>quiz<green> to begin!\"" << endl;
	output << "            - DISENGAGE" << endl;
	}
	if(isMath){
	output << "'Sentry_" << fileName << "_KA':" << endl;
	output << "  Type: Task" << endl;
	output << "  Script:" << endl;
	output << "    - FLAG NPC <player.name>:0" << endl;
	output << "    - FLAG NPC <player.name>_PR:" << numPreReqs << endl; //Number of completed pre-reqs. 0 is none. Max is all.
	output << "    - FLAG GLOBAL <player.name>_" << fileName << ":0" << endl; //0 represents an incomplete peak. 1 represents a completed peak.
	output << "    - URL \"" << URLFUNCTION + url << "\" <player.name>" << endl;
	output << "    - IF \"<FLAG.N:<player.name>>\" == \"Complete\" FLAG GLOBAL <player.name>_" << fileName <<":1 ELSE FLAG GLOBAL <player.name>_" << fileName << ":0" << endl;
	output << "    - IF \"<FLAG.G:<player.name>_" << fileName <<">\" == \"0\" CHAT \"You have not finished this peak yet!\"" << endl;
	output << "    - IF \"<FLAG.G:<player.name>_" << fileName <<">\" == \"0\" WAIT 2" << endl;
	output << "    - IF \"<FLAG.G:<player.name>_" << fileName <<">\" == \"0\" CHAT \"You can learn and practice this topic at " << dispUrl << "\"" << endl;
	output << "    - IF \"<FLAG.G:<player.name>_" << fileName <<">\" == \"1\" CHAT \"I see you have completed this topic! Congratulations!\"" << endl;

	for(int i=0; i<numPreReqs; i++)
	{
		output << "    - FLAG GLOBAL <player.name>_" << preReqs[i] << ":0" << endl;
		output << "    - URL \"" << URLFUNCTION + associatedUrls[i] << "\" <player.name>" << endl;
		output << "    - IF \"<FLAG.N:<player.name>>\" == \"Complete\" FLAG GLOBAL <player.name>_" << preReqs[i] <<":1 ELSE FLAG GLOBAL <player.name>_" << preReqs[i] << ":0" << endl;
		output << "    - IF \"<FLAG.G:<player.name>_" << preReqs[i] <<">\" == \"0\" FLAG NPC \"<player.name>_PR:-:1\"" << endl;
	}

	if(numPreReqs!=0)
	{
		output << "    - IF \"<FLAG.G:<player.name>_" << fileName <<">\" == \"1\" FLAG NPC <player.name>_PR:"<< numPreReqs << endl;
		output << "    - IF \"<FLAG.N:<player.name>_PR>\" != \"" << numPreReqs << "\" WAIT 2" << endl;
		output << "    - IF \"<FLAG.N:<player.name>_PR>\" != \"" << numPreReqs << "\" CHAT \"This peak may be <white>above your current skill level<green>!\"" << endl;
		output << "    - IF \"<FLAG.N:<player.name>_PR>\" != \"" << numPreReqs << "\" WAIT 2" << endl;
		output << "    - IF \"<FLAG.N:<player.name>_PR>\" == \"" << numPreReqs - 1 << "\" CHAT \"You may wish to try the earlier topic first...\"" << endl;
		output << "    - IF \"<FLAG.N:<player.name>_PR>\" < \"" << numPreReqs - 1 << "\" CHAT \"You may wish to try the earlier topics first...\"" << endl;
	}

	for(int i=0; i<numPreReqs; i++)
		output << "    - IF \"<FLAG.G:<player.name>_" << preReqs[i] <<">\" == \"0\" CHAT \"<white>" << associatedDispNames[i] << "<green>\"" << endl;
	output << "    - DISENGAGE" << endl;

	}

	output.close();
}

void generateGrad(double x, double y,double z, string subject, int numPreReqs, string preReqs[], string associatedDispNames[] , string associatedUrls[])
{
	ofstream output;

	output.open("Sentry_graduation_"+subject+".yml");

	output << "#This Sentry will appear at node: Graduation" << endl;
	output << "\"AssignmentSentry_graduation_" << subject << "\":" << endl;
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
	output << "    On Unavailable:" << endl;
	output << "      - CHAT \"I'm busy! Just a moment, please!\"" << endl;
	output << "  Interact Scripts:" << endl;
	output << "  - 1 Greeting_graduation_" << subject << endl;
	output << "'Greeting_graduation_" << subject << "':" << endl;
	output << "  Type: Interact" << endl;
	output << "  Requirements:" << endl;
	output << "    Mode: All" << endl;
	output << "  Steps:" << endl;
	output << "    1:" << endl;
	output << "      Chat Trigger:" << endl;
	output << "        1:" << endl;
	output << "            Trigger: /Regex:Hi|hi|Hello|hello|Hey|hey/, where am I?" << endl;
	output << "            Script:" << endl;
	output << "              - ENGAGE" << endl;
	output << "              - CHAT \"This is the highest peak on " << subject << " Mountain!\"" << endl;
	output << "              - WAIT 2" << endl;
	output << "              - RUNTASK \"Sentry_graduation_" << subject << "_KA\"" << endl;
	output << "'Sentry_graduation_" << subject << "_KA':" << endl;
	output << "  Type: Task" << endl;
	output << "  Script:" << endl;
	output << "    - FLAG NPC <player.name>:0" << endl;
	output << "    - FLAG NPC <player.name>_PR:" << numPreReqs << endl; //Number of completed pre-reqs. 0 is none. Max is all.
	output << "    - FLAG GLOBAL <player.name>_graduation_" << subject << ":0" << endl; //0 represents an incomplete peak. 1 represents a completed peak.


	for(int i=0; i<numPreReqs; i++)
	{
		output << "    - FLAG GLOBAL <player.name>_" << preReqs[i] << ":0" << endl;
		output << "    - URL \"" << URLFUNCTION + associatedUrls[i] << "\" <player.name>" << endl;
		output << "    - IF \"<FLAG.N:<player.name>>\" == \"Complete\" FLAG GLOBAL <player.name>_" << preReqs[i] <<":1 ELSE FLAG GLOBAL <player.name>_" << preReqs[i] << ":0" << endl;
		output << "    - IF \"<FLAG.G:<player.name>_" << preReqs[i] <<">\" == \"0\" FLAG NPC \"<player.name>_PR:-:1\"" << endl;
	}

	if(numPreReqs!=0)
	{
		output << "    - IF \"<FLAG.N:<player.name>_PR\" == \"" << numPreReqs << "\" FLAG GLOBAL <player.name>_graduation_" << subject << ":1 ELSE FLAG GLOBAL <player.name>_graduation_" << subject << ":0" << endl;
		output << "    - IF \"<FLAG.N:<player.name>_PR\" != \"" << numPreReqs << "\" CHAT \"To truly master " << subject << ", you should try the following first..." << endl; 
		output << "    - IF \"<FLAG.N:<player.name>_PR\" != \"" << numPreReqs << "\" WAIT 2" << endl; 
	}

	for(int i=0; i<numPreReqs; i++)
		output << "    - IF \"<FLAG.G:<player.name>_" << preReqs[i] <<">\" == \"0\" CHAT \"<white>" << associatedDispNames[i] << "<green>\"" << endl;

	output << "    - IF \"<FLAG.G:<player.name>_graduation_" << subject <<">\" == \"1\" CHAT \"You have mastered " << subject << "! Congratulations!\"" << endl;
	output << "    - DISENGAGE" << endl;


	output.close();
}

//Create the Master Sentry here.
void generateMasterSentry(int entry, string displayNamesSmall[], double highestX, double highestY, double highestZ, string subject)
{
	ofstream output;

	output.open("MASTER_Sentry_"+subject+".yml");

	output << "#This is the MASTER SENTRY for " << entry << " different " << subject << " Sentries." << endl;
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
    output << "                - execute as_server \"denizen save\"" << endl;
	output << "                - execute as_npc \"npc select master\"" << endl;
	output << "                - execute as_npc \"npc remove\"" << endl;
	output << "            2:" << endl;
	output << "                Trigger: /Delete/ the sentries on this landscape." << endl;
	output << "                Script:" << endl;

	for(int i=0; i<1000; i++)
		if(fileNames[i]!="BLANK")
		{
			output << "                - execute as_npc \"npc select " << displayNamesSmall[i] << "\"" << endl;
			output << "                - execute as_npc \"npc remove\"" << endl;
		}

	output.close();
}