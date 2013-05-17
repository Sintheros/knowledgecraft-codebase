#pragma once

namespace QuizGenerator {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Collections::Generic;


	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			//Instantiate a list to hold questions (defined below)
			questions = gcnew List<Question^>();

			//Instantiate a list to hold answers (defined below)
			answers = gcnew Dictionary<String^,bool>();

			hint = gcnew List<String^>();
			response = gcnew List<String^>();

			alphabet = gcnew List<String^>();
			questionCounter = 0;

			//hint[0] = "no hint";//default hint

			

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lblQuestion;
	private: System::Windows::Forms::Label^  lblCAnswer;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  lblFilename;
	private: System::Windows::Forms::TextBox^  txtTitle;
	private: System::Windows::Forms::TextBox^  txtCorrect;
	private: System::Windows::Forms::TextBox^  txtIncorrect;



	private: System::Windows::Forms::TextBox^  txtQuestion;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  lblHint;
	private: System::Windows::Forms::TextBox^  txtHint;

	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Button^  btnGen;
	private: System::Windows::Forms::Button^  btnClear;
	private: System::Windows::Forms::Button^  btnAdd;







	protected: 

	protected: 

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		String^ quizTitle;

		int questionCounter;

		//Create a list to hold questions
		List<Question^>^ questions;

		//Create a list to hold correct answers
		Dictionary<String^, bool>^ answers;

		List<String^>^ hint;
		List<String^>^ response;

		List<String^>^ alphabet;


	private: System::Windows::Forms::Button^  btnFilegen;







		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->lblQuestion = (gcnew System::Windows::Forms::Label());
			this->lblCAnswer = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->lblFilename = (gcnew System::Windows::Forms::Label());
			this->txtTitle = (gcnew System::Windows::Forms::TextBox());
			this->txtCorrect = (gcnew System::Windows::Forms::TextBox());
			this->txtIncorrect = (gcnew System::Windows::Forms::TextBox());
			this->txtQuestion = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->lblHint = (gcnew System::Windows::Forms::Label());
			this->txtHint = (gcnew System::Windows::Forms::TextBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->btnGen = (gcnew System::Windows::Forms::Button());
			this->btnClear = (gcnew System::Windows::Forms::Button());
			this->btnAdd = (gcnew System::Windows::Forms::Button());
			this->btnFilegen = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// lblQuestion
			// 
			this->lblQuestion->AutoSize = true;
			this->lblQuestion->Location = System::Drawing::Point(19, 30);
			this->lblQuestion->Name = L"lblQuestion";
			this->lblQuestion->Size = System::Drawing::Size(52, 13);
			this->lblQuestion->TabIndex = 0;
			this->lblQuestion->Text = L"Question:";
			this->lblQuestion->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// lblCAnswer
			// 
			this->lblCAnswer->AutoSize = true;
			this->lblCAnswer->Location = System::Drawing::Point(27, 31);
			this->lblCAnswer->Name = L"lblCAnswer";
			this->lblCAnswer->Size = System::Drawing::Size(44, 13);
			this->lblCAnswer->TabIndex = 0;
			this->lblCAnswer->Text = L"Correct:";
			this->lblCAnswer->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(19, 78);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(52, 13);
			this->label2->TabIndex = 0;
			this->label2->Text = L"Incorrect:";
			this->label2->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// lblFilename
			// 
			this->lblFilename->AutoSize = true;
			this->lblFilename->Location = System::Drawing::Point(28, 21);
			this->lblFilename->Name = L"lblFilename";
			this->lblFilename->Size = System::Drawing::Size(54, 13);
			this->lblFilename->TabIndex = 0;
			this->lblFilename->Text = L"Quiz Title:";
			this->lblFilename->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// txtTitle
			// 
			this->txtTitle->Location = System::Drawing::Point(88, 18);
			this->txtTitle->Name = L"txtTitle";
			this->txtTitle->Size = System::Drawing::Size(251, 20);
			this->txtTitle->TabIndex = 1;
			// 
			// txtCorrect
			// 
			this->txtCorrect->Location = System::Drawing::Point(77, 28);
			this->txtCorrect->Multiline = true;
			this->txtCorrect->Name = L"txtCorrect";
			this->txtCorrect->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->txtCorrect->Size = System::Drawing::Size(251, 41);
			this->txtCorrect->TabIndex = 4;
			this->txtCorrect->TextChanged += gcnew System::EventHandler(this, &Form1::txtCorrect_TextChanged);
			// 
			// txtIncorrect
			// 
			this->txtIncorrect->Location = System::Drawing::Point(77, 76);
			this->txtIncorrect->Multiline = true;
			this->txtIncorrect->Name = L"txtIncorrect";
			this->txtIncorrect->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->txtIncorrect->Size = System::Drawing::Size(251, 42);
			this->txtIncorrect->TabIndex = 5;
			// 
			// txtQuestion
			// 
			this->txtQuestion->Location = System::Drawing::Point(77, 27);
			this->txtQuestion->Multiline = true;
			this->txtQuestion->Name = L"txtQuestion";
			this->txtQuestion->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->txtQuestion->Size = System::Drawing::Size(251, 39);
			this->txtQuestion->TabIndex = 2;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->txtIncorrect);
			this->groupBox1->Controls->Add(this->txtCorrect);
			this->groupBox1->Controls->Add(this->lblCAnswer);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Location = System::Drawing::Point(12, 196);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(349, 137);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Answers (1 per line)";
			// 
			// lblHint
			// 
			this->lblHint->AutoSize = true;
			this->lblHint->Location = System::Drawing::Point(42, 76);
			this->lblHint->Name = L"lblHint";
			this->lblHint->Size = System::Drawing::Size(29, 13);
			this->lblHint->TabIndex = 0;
			this->lblHint->Text = L"Hint:";
			this->lblHint->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// txtHint
			// 
			this->txtHint->Location = System::Drawing::Point(77, 73);
			this->txtHint->Multiline = true;
			this->txtHint->Name = L"txtHint";
			this->txtHint->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->txtHint->Size = System::Drawing::Size(251, 39);
			this->txtHint->TabIndex = 3;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->txtHint);
			this->groupBox2->Controls->Add(this->txtQuestion);
			this->groupBox2->Controls->Add(this->lblHint);
			this->groupBox2->Controls->Add(this->lblQuestion);
			this->groupBox2->Location = System::Drawing::Point(12, 54);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(350, 127);
			this->groupBox2->TabIndex = 2;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Questions";
			this->groupBox2->Enter += gcnew System::EventHandler(this, &Form1::groupBox2_Enter);
			// 
			// btnGen
			// 
			this->btnGen->Location = System::Drawing::Point(135, 347);
			this->btnGen->Name = L"btnGen";
			this->btnGen->Size = System::Drawing::Size(115, 42);
			this->btnGen->TabIndex = 7;
			this->btnGen->Text = L"Generate";
			this->btnGen->UseVisualStyleBackColor = true;
			this->btnGen->Click += gcnew System::EventHandler(this, &Form1::btnGen_Click);
			// 
			// btnClear
			// 
			this->btnClear->Location = System::Drawing::Point(256, 347);
			this->btnClear->Name = L"btnClear";
			this->btnClear->Size = System::Drawing::Size(105, 42);
			this->btnClear->TabIndex = 8;
			this->btnClear->Text = L"Clear";
			this->btnClear->UseVisualStyleBackColor = true;
			this->btnClear->Click += gcnew System::EventHandler(this, &Form1::btnClear_Click);
			// 
			// btnAdd
			// 
			this->btnAdd->Location = System::Drawing::Point(12, 347);
			this->btnAdd->Name = L"btnAdd";
			this->btnAdd->Size = System::Drawing::Size(117, 42);
			this->btnAdd->TabIndex = 6;
			this->btnAdd->Text = L"Add Entry";
			this->btnAdd->UseVisualStyleBackColor = true;
			this->btnAdd->Click += gcnew System::EventHandler(this, &Form1::btnAdd_Click);
			// 
			// btnFilegen
			// 
			this->btnFilegen->Location = System::Drawing::Point(12, 395);
			this->btnFilegen->Name = L"btnFilegen";
			this->btnFilegen->Size = System::Drawing::Size(350, 42);
			this->btnFilegen->TabIndex = 9;
			this->btnFilegen->Text = L"Generate quiz from file";
			this->btnFilegen->UseVisualStyleBackColor = true;
			this->btnFilegen->Click += gcnew System::EventHandler(this, &Form1::btnFilegen_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(380, 452);
			this->Controls->Add(this->btnFilegen);
			this->Controls->Add(this->btnAdd);
			this->Controls->Add(this->btnClear);
			this->Controls->Add(this->btnGen);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->txtTitle);
			this->Controls->Add(this->lblFilename);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Mindcraft Quiz Generator";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void clearText() 
		 {

			 //This method clears all textboxes
			 txtQuestion->Text = "";
			 txtHint->Text = "";
			 txtCorrect->Text = "";
			 txtIncorrect->Text = "";

		 }

private: System::Void readFile() 
		 {
			 //This method will read in the file and store it in different lists
			 String^ fileName;

			 String^ qString;
			 String^ lastStr = "";

			 if(txtTitle->Text->Contains(".txt"))
			 {
				 fileName= txtTitle->Text;
				 txtTitle->Text = txtTitle->Text->Substring(0, txtTitle->Text->LastIndexOf("."));


			 }
			 else
			 {
				 fileName= txtTitle->Text + ".txt";
			 }
			 

			 try 
			 {
				 StreamReader^ din = File::OpenText(fileName);

				 String^ str;

				 //Read until end of file
				 while ((str = din->ReadLine()) != nullptr) 
				 {
					 //Ignore comment lines
					 if(!str->StartsWith("##"))
					 {	
						 if(str->StartsWith(">Filename: "))
					     {
							 quizTitle = str->Substring(11);
						 }
						 else if(str->StartsWith(">Q: "))
						 {
							 qString = str->Substring(4);
						 }
						 else if(str->StartsWith(">C: "))
						 {
							 answers->Add(str->Substring(4), true);
						 }
						 else if(str->StartsWith(">I: "))
						 {
							 answers->Add(str->Substring(4), false);
						 }
						 else if(str->StartsWith(">H: "))
						 {
							 hint->Add(str->Substring(4));
							 lastStr = "hint";
						 }
						 else if(str->StartsWith(">R: "))
						 {
							 response->Add(str->Substring(4));
							 lastStr = "response";
						 }
						 else if(String::IsNullOrEmpty(str))
						 {
							 if(qString != "")
							 {
								questions->Add(gcnew Question(qString, answers, hint, response));
								questionCounter++;
							 }

							 qString = "";
							 answers = gcnew Dictionary<String^,bool>();
							 hint = gcnew List<String^>();
							 response = gcnew List<String^>();
					     }
						 else
						 {
							 if(lastStr == "hint")
							 {
								 hint->Add(str);
							 }
							 else if(lastStr == "response")
							 {
								 response->Add(str);
							 }
						 }

					 }
					 

				 }
			 }
			 catch (Exception^ e)
			 {
				 if (dynamic_cast<FileNotFoundException^>(e))
					MessageBox::Show("File not found. Make sure you entered the filename correctly.", "Mindcraft", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				 else
					 MessageBox::Show("Problem reading file " + fileName, "Mindcraft", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
			 }

		 }
private: System::Void generateFromFile() 
		 {
				 alphabet->Add("A");
				 alphabet->Add("B");
				 alphabet->Add("C");
				 alphabet->Add("D");
				 alphabet->Add("E");
				 alphabet->Add("F");
				 alphabet->Add("G");
				 alphabet->Add("H");
				 alphabet->Add("I");
				 alphabet->Add("J");
				 alphabet->Add("K");
				 alphabet->Add("L");
				 alphabet->Add("M");
				 alphabet->Add("N");
				 alphabet->Add("O");
				 alphabet->Add("P");
				 alphabet->Add("Q");
				 alphabet->Add("R");
				 alphabet->Add("S");
				 alphabet->Add("T");
				 alphabet->Add("U");
				 alphabet->Add("V");
				 alphabet->Add("W");
				 alphabet->Add("Z");
				 alphabet->Add("Y");
				 alphabet->Add("Z");
			 
				 //Keep track of the number of answers
				 int answerCounter = 1;

				 //Separate counter to assign the alphabet to answers
				 int anscount = 0;

				 questionCounter = 1;

				 //Store filename
				 String^ fileName = "questions_" + quizTitle + ".yml";

				 //Create a streamwriter to write to file
				 StreamWriter^ sw = gcnew StreamWriter(fileName);

				 //Header
				 sw->WriteLine("\"questions_" + quizTitle + "\":");
				 sw->WriteLine("  Type: Assignment");
				 sw->WriteLine("  Actions:");
				 sw->WriteLine("    On Assignment:");
				 sw->WriteLine("        - TRIGGER NAME:Proximity TOGGLE:True RADIUS:8");

				 sw->WriteLine("  Interact Scripts:");
				 sw->WriteLine("  - 10 Quiz_" + quizTitle);

				 sw->WriteLine("'Quiz_" + quizTitle + "':");
				 sw->WriteLine("  Type: Interact");
				 sw->WriteLine("  Requirements:");
				 sw->WriteLine("    Mode: All");
				 sw->WriteLine("  Steps:");

				 for(int i = 0; i < questions->Count; i++)
				 {
					 //Question phase


					 //Check for first question
					 if(questionCounter == 1)
					 {
						 sw->WriteLine("    'Q" + questionCounter + "*':");
					 }
					 else
					 {
						 sw->WriteLine("    'Q" + questionCounter + "':");
					 }
					 
					 sw->WriteLine("        Click Trigger:");
					 sw->WriteLine("            Script:");
					 sw->WriteLine("            - CHAT \"" + questions[i]->questionStr + "\"");
					 sw->WriteLine("            - WAIT 1");

					 //Loop through answers
					 for each(KeyValuePair<String^, bool> kvp in questions[i]->answers)
					 {
						 sw->WriteLine("            - CHAT \"" + alphabet[anscount] + ". " + kvp.Key + "\"");
						 anscount++;
					 }

					 sw->WriteLine("            - ZAP 'step:A" + questionCounter + "'");

					 //Answering phaseS
					 sw->WriteLine("    'A" + questionCounter + "':");
					 sw->WriteLine("        Chat Trigger:");


					 //Loop through and write answer choices and results
					 for each(KeyValuePair<String^, bool> kvp in questions[i]->answers)
					 {
						 //Check if the answer is correct
						 if(kvp.Value == true)
						 {
							 sw->WriteLine("            " + answerCounter + ":");
							 sw->WriteLine("                Trigger: I choose /" + alphabet[answerCounter -1] + "/.");
							 sw->WriteLine("                Script:");
							 
							 //Check for custom response and write it
							 if(questions[i]->response->Count > 0)
							 {
								 for(int j = 0; j < questions[i]->response->Count; j++)
								 {
									 sw->WriteLine("                - CHAT \"" + questions[i]->response[j] + "\"");
								 }
								 
							 }
							 else
							 {
								 //Default correct answer output
								 sw->WriteLine("                - CHAT \"Correct answer!\"");
							 }

							 //Check for last question to reassign
							 if(questionCounter < questions->Count)
							 {
								 
								 sw->WriteLine("                - WAIT 1");
								 sw->WriteLine("                - CHAT \"Right click me for another question\"");
								 sw->WriteLine("                - ZAP 'step:Q" + (questionCounter + 1) + "'");//Go to next question
							 }
							 else
							 {
								 //End of quiz
								 sw->WriteLine("                - WAIT 1");
								 sw->WriteLine("                - CHAT \"That's all I've got! Why don't you try the next peak?\"");

								 //Reassign sentry
								 sw->WriteLine("                - EXECUTE as_server \"npc select <npc.id>\"");
								 sw->WriteLine("                - EXECUTE as_server \"npc assignment --set AssignmentSentry_" + quizTitle + "\"");
							 }
							 

							 answerCounter++;
						 }
						 else
						 {
							 sw->WriteLine("            " + answerCounter + ":");
							 sw->WriteLine("                Trigger: I choose /" + alphabet[answerCounter -1] + "/.");
							 sw->WriteLine("                Script:");
							 sw->WriteLine("                - CHAT \"Incorrect answer!\"");
							 
							 //Check for last question to reassign the sentry
							 if(questionCounter < questions->Count)
							 {
								 sw->WriteLine("                - WAIT 1");
								 sw->WriteLine("                - CHAT \"Right click me for another question\"");
								 sw->WriteLine("                - ZAP 'step:Q" + (questionCounter + 1) + "'");//Go to next question
							 }
							 else
							 {
								 //End of quiz
								 sw->WriteLine("                - WAIT 1");
								 sw->WriteLine("                - CHAT \"That's all I've got! Why don't you try the next peak?\"");

								 //Reassign sentry
								 sw->WriteLine("                - EXECUTE as_server \"npc select <npc.id>\"");
								 sw->WriteLine("                - EXECUTE as_server \"npc assignment --set AssignmentSentry_" + quizTitle + "\"");
							 }

							 answerCounter++;
						 }

					 }

					 //Hint
					 sw->WriteLine("            " + answerCounter + ":");
					 sw->WriteLine("                Trigger: I would like a /Hint/.");
					 sw->WriteLine("                Script:");

					 //Write out the hint line by line
					 for(int k = 0; k < questions[i]->hint->Count; k++)
					 {
						 sw->WriteLine("                - CHAT \"" + questions[i]->hint[k] + "\"");
					 }
					 
					 sw->WriteLine("                - ZAP 'step:Q" + (questionCounter) + "'");//Go back to question

					 //proximity checks
					 sw->WriteLine("        Proximity Trigger:");
					 sw->WriteLine("            exit:");
					 sw->WriteLine("                Script:");
					 sw->WriteLine("                - EXECUTE as_server \"npc select <npc.id>\"");
				     sw->WriteLine("                - EXECUTE as_server \"npc assignment --set AssignmentSentry_" + quizTitle + "\"");

					 questionCounter++;

					 //Reset counters
					 answerCounter = 1;
					 anscount = 0;
					 
				 }				

				 //Close the stream
				 sw->Close();

				 //Show messagebox
				 MessageBox::Show("Sucessfully generated " + "questions_" + quizTitle + ".yml.", "Mindcraft", MessageBoxButtons::OK, MessageBoxIcon::Asterisk);
				 clearText();
			 
		 }

	//////OBSOLETE METHOD needs to update
private: System::Void generateQuiz() 
		 {
			//Check for empty input
			if(txtTitle->Text == "" || txtQuestion->Text == "" || txtHint->Text == "" || txtCorrect->Text == "" || txtIncorrect->Text == "")
			{
				 MessageBox::Show("One or more fields are empty.", "Mindcraft", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
			}
			else
			{
				//Populate the answers
				for each(String^ str in txtCorrect->Lines)
				{
					if(str != "")
					{
						answers->Add(str, true);
					}
					
				}

				for each(String^ str in txtIncorrect->Lines)
				{
					if(str != "")
					{
						answers->Add(str, false);
					}
				}

				//Keep track of the number of answers
				int answerCounter = 1;

				//Store filename
				String^ fileName = "questions_" + txtTitle->Text + ".yml";

				//Create a streamwriter to write to file
				StreamWriter^ sw = gcnew StreamWriter(fileName);

				//Header
				sw->WriteLine("\"questions_" + txtTitle->Text + "\":");
				sw->WriteLine("  Type: Assignment");
				sw->WriteLine("  Interact Scripts:");
				sw->WriteLine("  - 10 Quiz");

				sw->WriteLine("'Quiz':");
				sw->WriteLine("  Type: Interact");
				sw->WriteLine("  Requirements:");
				sw->WriteLine("    Mode: All");
				sw->WriteLine("  Steps:");

				//Question phase
				sw->WriteLine("    'Q1':");
				sw->WriteLine("        Chat Trigger:");
				sw->WriteLine("            1:");
				sw->WriteLine("                Trigger: /Quiz/ me!");
				sw->WriteLine("                Script:");
				sw->WriteLine("                - CHAT \"" + txtQuestion->Text + "\"");
				sw->WriteLine("                - WAIT 1");

				//Loop through answers
				for each(KeyValuePair<String^, bool> kvp in answers)
				{
					sw->WriteLine("                - CHAT \"" + kvp.Key + "\"");
				}

				sw->WriteLine("                - ZAP 'step:A1'");

				//Answering phase
				sw->WriteLine("    'A1':");
				sw->WriteLine("        Chat Trigger:");

				
				//Loop through and write answer choices and results
				for each(KeyValuePair<String^, bool> kvp in answers)
				{
					//Check if the answer is correct
					if(kvp.Value == true)
					{
						sw->WriteLine("            " + answerCounter + ":");
						sw->WriteLine("                Trigger: I choose /" + answerCounter + "/.");
						sw->WriteLine("                Script:");
						sw->WriteLine("                - CHAT \"Correct answer!\"");
						sw->WriteLine("                - ZAP 'step:Q1'");

						answerCounter++;
					}
					else
					{
						sw->WriteLine("            " + answerCounter + ":");
						sw->WriteLine("                Trigger: I choose /" + answerCounter + "/.");
						sw->WriteLine("                Script:");
						sw->WriteLine("                - CHAT \"Incorrect answer!\"");
						sw->WriteLine("                - ZAP 'step:Q1'");

						answerCounter++;
					}
					
				}

				//Hint
				sw->WriteLine("            " + answerCounter + ":");
				sw->WriteLine("                Trigger: I would like a /Hint/.");
				sw->WriteLine("                Script:");
				sw->WriteLine("                - CHAT \"" + txtHint->Text + "\"");
				sw->WriteLine("                - ZAP 'step:Q1'");

				//Close the stream
				sw->Close();

				//Show messagebox
				MessageBox::Show("Sucessfully generated " + "questions_" + txtTitle->Text + ".yml.", "Mindcraft", MessageBoxButtons::OK, MessageBoxIcon::Asterisk);
				clearText();
			}
		 }
		

private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
	    }
private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void btnClear_Click(System::Object^  sender, System::EventArgs^  e) {

			 //Clear textboxes
			 clearText();

			 //Clear the title
			 txtTitle->Text = "";

		 }
private: System::Void btnGen_Click(System::Object^  sender, System::EventArgs^  e) {

			 generateQuiz();
		   
			
		 }
private: System::Void txtCorrect_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void btnAdd_Click(System::Object^  sender, System::EventArgs^  e) {


		 }
private: System::Void btnFilegen_Click(System::Object^  sender, System::EventArgs^  e) {
			 readFile();

			 generateFromFile();

			 questions = gcnew List<Question^>();
		 }
private: System::Void groupBox2_Enter(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}

