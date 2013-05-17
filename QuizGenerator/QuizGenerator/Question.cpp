#include "StdAfx.h"
#include "Question.h"


Question::Question(String^ q, Dictionary<String^,bool>^ ans, List<String^>^ h, List<String^>^ r)
{
	questionStr = q;
	answers = ans;
	hint = h;
	response = r;
}
