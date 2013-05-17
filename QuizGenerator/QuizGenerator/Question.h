#pragma once

using namespace System;
using namespace System::Collections::Generic;

ref class Question
{
public:
	Question(String^ q, Dictionary<String^,bool>^ ans, List<String^>^ h, List<String^>^ r);
public:
	String^ questionStr;
	Dictionary<String^,bool>^ answers;
	List<String^>^ hint;
	List<String^>^ response;

	
};

