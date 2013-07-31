// FlagScrubber.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string const listPath = "C:/minecraft_server/plugins/Denizen/FlagsToScrub.txt";
	string const savesPath = "C:/minecraft_server/plugins/Denizen/saves.yml";
	string const tempSavesPath = "C:/minecraft_server/plugins/Denizen/saves.yml";

	string const indent = "      ";

	string flagsToScrub[1000];
	string saves[10000];

	ifstream input;
	ofstream output;

	input.open(listPath);
	if(input.fail())
	{
		cout << "Could not open List." << endl;
	}
	int ticker = 0;
	while(!input.eof())
	{
		string buffer;
		getline(input,buffer);
		flagsToScrub[ticker] = buffer;
		ticker++;
	}
	input.close();

	input.open(savesPath);
	if(input.fail())
	{
		cout << "Could not open Saves." << endl;
	}
	int savesLength = 0;
	while(!input.eof())
	{
		string buffer;
		getline(input,buffer);
		saves[savesLength] = buffer;
		savesLength++;
	}

	input.close();

	output.open(tempSavesPath);
	if(output.fail())
	{
		cout << "Could not open Saves." << endl;
	}
	ticker = 0;
	while(ticker < savesLength)
	{
		string temp = indent+saves[ticker];
		for(int i=0; i<flagsToScrub->size(); i++)
		{
			if(indent+flagsToScrub[i]+":" == saves[ticker])
				ticker+=2;
		}
		output << saves[ticker] << endl;
		ticker++;
	}
	output.close();

	return 0;
}

