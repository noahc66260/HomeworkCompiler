// HomeworkCompiler.cpp -- Create a LaTeX document according to specifications

#include<iostream>
#include<string>
#include<cctype>
#include<fstream>
#include<cstdlib>
#include<list>
#include<utility>
#include<vector>
using namespace std;
const string TAB = "\t";
const string DEFAULT_INPUT = "specification.txt"; 

/*
		This would be better if implemented by classes. Then
		I actually could include macros. It would also allow me
		the option to include extras, such as double spacing,
		two columns, homework or lab report

		Something that would be very helpful is if templates were 
		included at the bottom such as including a picture,
		using the align environment, multcolumn environment, 
		including a table, itemize, enumerate, a bibliography, etc. 


		I just had a great idea! If I put "Null" for the problem,
		No parentheses should be typeset! Hooray!
*/

// typeset the preamble and the \begin{document} command
void initializeDocument(ostream & fout);

// parses data from specification document and typesets title in LaTeX
void makeTitle(istream & fin, ostream & fout);

// parses more data from specification document and typesets a 
// short header that follows the title
void addHeader(istream & fin, ostream & fout);

// parses data from specification document and typesets the 
// problems that follow the header
void addQuestions(istream & fin, ostream & fout);

// parse a line containing field separated by delimiters and 
// inserts each field into container
void parse(string line, char delimiter, list<string> & container);

// moves file pointer to first nonwhitespace character
// returns the number of white spaces skipped
int eatWhiteSpace(istream & is);

// removes leading and trailing whitespaces surrounding string s
void cutWhiteSpace(string & s);

// displays the contents of a list for debugging purposes
void displayList(ostream & os, list<string> & l);

// typesets the command \end{document} 
inline void concludeDocument(ostream & fout) 
	{fout << "\\end{document}" << endl;}

// returns true if file already exists, false otherwise
bool fexists(string s);


int main(int argc, char ** argv)
{
	// Allow the user to enter the input file or use the default
	ifstream fin;
	string inputFile;
	if (argc == 2)
	{
//		cout << "Using " << *argv << " for input" << endl;
		inputFile = argv[1];
	}
	else
	{
//		cout << "Using " << DEFAULT_INPUT << " for input" << endl;
		inputFile = DEFAULT_INPUT;
	}

	fin.open(inputFile.c_str());
	if (!fin)
	{
		cerr << "Could not open " << inputFile
			<< endl << "Program abort" << endl;
		exit(EXIT_FAILURE);
	}
	string field;
	getline(fin,field,':');
	ofstream fout;

	if (field != "Save as")
	{
		cerr << "You must have a field that specifies what to save" 
			<< "the file as." << endl;
		cerr << "Program abort" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		getline(fin,field);
		cutWhiteSpace(field);
		// we don't want to clobber an extant file
		if (fexists(field))
		{
			cerr << "Error: " << field << " already exists." << endl;
			exit(EXIT_FAILURE);
		}
		else
			fout.open(field.c_str());
	}

	initializeDocument(fout);
	fout << "\\allowdisplaybreaks" << endl;
	makeTitle(fin,fout);
	addHeader(fin,fout);
	eatWhiteSpace(fin);
	if(fin)
		addQuestions(fin,fout);
	concludeDocument(fout);
//	cout << "Document compilation complete." << endl;

	return 0;
}

/*
	I need to edit this later to have formatting of the form
	\usepackage{...}
	\usepackage{...}
	...

	and also I need to ensure that the relevant packages are included.
	This is not the case right now.
*/
void initializeDocument(ostream & fout)
{
	fout << "\\documentclass[12 pt]{amsart}" 
		<< endl << endl
		<< "\\usepackage{amssymb}" 
		<< endl
		<< "\\usepackage{latexsym}"
		<< endl
		<< "\\usepackage{graphicx}" 
		<< endl
		<< "\\usepackage{verbatim}" 
		<< endl
		<< "\\usepackage{enumerate}" 
		<< endl
		<< "\\usepackage{amsmath}"
		<< endl
		<< "\\usepackage{fullpage}"
		<< endl << endl
		<< "\\begin{document}" 
		<< endl << endl;
}


void makeTitle(istream & fin, ostream & fout)
{
	string field;
	getline(fin,field,':'); // should be "Title"
	getline(fin, field);
//	cout << "our title is " << field << endl;
	list<string> container;
	parse(field,',',container);
	string subtitle = container.front();
	fout << "\\title" << endl;
	fout << "[" << subtitle << "]" << endl;
	fout << "{";
	while (!container.empty())
	{
		fout << container.front();
		container.pop_front();
		if (!container.empty())
		{
			fout << " \\\\" << endl;
		}
	}
	fout << "}" << endl << endl;

	getline(fin,field,':'); // should be "Document Author(s)"
	getline(fin,field);
	container.clear();
	parse(field,',',container);
	while (!container.empty())
	{
		fout << "\\author{" << container.front() << "}" << endl;
		container.pop_front();
	}
	fout << endl;

	getline(fin,field,':'); // Should be "Date"
	getline(fin,field);
	fout << "\\date{" << field << "}" << endl << endl;
	fout << "\\maketitle" << endl;
}

// function to add Book title
// we enter this function after getting field "Book Problems"
void addHeader(istream & fin, ostream & fout)
{
	fout << "\\begin{center}" << endl;
	fout << TAB << "Problems ";
	string field;
	getline(fin,field,':'); // Should read Book Problems
//	cout << "This should read Book Problems: " << field << endl;
	getline(fin,field);
	list<string> container;
	parse(field,',',container);
	displayList(fout,container);
	fout << endl;
	getline(fin,field,':'); // Should be Book title
//	cout << "This should read Book Title: " << field << endl;
	getline(fin,field);
	cutWhiteSpace(field);
//	cout << "And the book title is " << field << endl;
	if (!field.empty())
	{
		fout << TAB; 
		fout << "from \\emph{" << field << "}, "; 
		fout << endl;
	}
	getline(fin,field,':'); // Book Edition
//	cout << "Should read Book Edition: " << field << endl;
	int edition;
	getline(fin,field);
	cutWhiteSpace(field);
//	cout << "and the edition is " << field << endl;
	if (!field.empty())
	{
		edition = atoi(field.c_str());
		fout << TAB; 
		switch (edition)
		{
			case 1: 
				fout << "$" << 1 << "^{\\text{st}}$ ";
				break;
			case 2:
				fout << "$" << 2 << "^{\\text{nd}}$ ";
				break;
			case 3:
				fout << "$" << 3 << "^{\\text{rd}}$ ";
				break;
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
				fout << "$" << edition << "^{\\text{th}}$ ";
				break;
			default:
				cout << "Edition " << edition << "not recognized." << endl;
				//exit(EXIT_FAILURE); // We don't need to stop the program over this
		}
		fout << "edition," << endl;
	}
	getline(fin,field,':'); // Book Author(s)
//	cout << "Should read Book Authors: " << field << endl;
	getline(fin,field);
	cutWhiteSpace(field);
	container.clear();
	if (!field.empty())
	{
		parse(field,',',container);
		fout << TAB; 
		fout << "by ";
		displayList(fout,container);
		fout << endl;
	}
	fout << "\\end{center}" << endl << endl;
}

// function to add book author
void addQuestions(istream & fin, ostream & fout)
{
	string question;
	string parts; // a,b,c,...
	vector< pair<string, string> > questionList;
	list<string> container;
	fin.peek(); // not necessary since main() checks for us
	pair<string, string> data;
	while(fin)
	{
		getline(fin, question,':');
		getline(fin, parts);
		data.first = question;
		data.second = parts;
		if ( !fin.good() )
			break;
		questionList.push_back(data);
	}
//	cout << "questionList.size() = " << questionList.size() << endl;
	for (unsigned int i = 0; i < questionList.size() ; i++)
	{
		fout << "% PROBLEM " << (i+1) << endl;
		fout << "\\phantom{\\quad} \\vfill" << endl;
		fout << "\\noindent" << endl;
		fout << "\\textbf{Problem} (" << questionList[i].first << ") ";
		fout << "\\\\[4ex]" << endl;
		fout << "\\emph{Solution.} ";
		fout << "\\\\[2ex]" << endl;
		parse(questionList[i].second,',',container);
		if (!container.empty())
		{
			fout << TAB << "\\begin{enumerate}" << endl;
			while (!container.empty())
			{
				fout << TAB << TAB << "\\item[" << container.front() << ".]";
				fout << endl << endl;
				container.pop_front();
			}
			fout << TAB << "\\end{enumerate}" << endl;
		}
		fout << "\\vfill" << endl;
		if (i != questionList.size()-1)
			fout << "\\newpage" << endl;
		fout << endl << endl << endl;
	}
	return;
}


void parse(string line, char delimiter, list<string> & container)
{
//	cout << "We have the line: " << line << endl;
	string element;
	unsigned int begin, n;
	begin = 0;
	while (begin < line.length())
	{
		n = 0;
		while(line[begin + n] != delimiter && (begin + n) < line.length())
			n++;
		element = string(line,begin,n);
		cutWhiteSpace(element);
		if (element.size() > 0)
		{
			container.push_back(element);
//			cout << "Pushed " << element << endl;
		}
		begin = begin + n + 1;
	}
	list<string> l = container;
	if (l.size() == 0)
		return;
//	cout << "The elements of the size " 
//			<< l.size() << " container are: " << endl;
	while (l.size() > 0)
	{
//		cout << l.front() << endl;
		l.pop_front();
	}
}


// remove leading white space
int eatWhiteSpace(istream & is)
{
	int i = 0;
	char ch;
	while(isspace( ch = is.peek()))
	{
		is.get();
		i++;
	}
	return i;
}

void cutWhiteSpace(string & str)
{
	unsigned int i;
	i = str.size()-1;
	while(isspace(str[i]) && i >= 0)
		i--;
	str.erase(i+1, str.size()-1-i);
	i = 0;
	if (str.size() == 0)
		return;
	while(isspace(str[i]) && i < str.length())
		i++;
	str.erase(0,i);
}

void displayList(ostream & os, list<string> & l)
{
	if (l.size() == 1)
	{
		os << l.front();
		l.pop_front();
	}
	else if (l.size() == 2)
	{
		os << l.front() << " and ";
		l.pop_front();
		os << l.front();
		l.pop_front();
	}
	else
	{
		while(!l.empty())
		{
			if (l.size() == 1)
				os << l.front() << " ";
			else if (l.size() == 2)
				os << l.front() << ", and ";
			else
				os << l.front() << ", ";
			l.pop_front();
		}
	}
}

bool fexists(string s)
{
	ifstream ifs(s.c_str());
	if (ifs.good())
	{
		ifs.close();
		return true;
	}
	else
		return false;
}

/*
Format of specification.txt

Save as: Homework 1.tex
Title: (Homework 10), MATH 161, Probability, Professor Roberts
Document Author(s): First Author, Second Author, ...
Date: Today's date
Book Problems: 10.1d, 10.2cef, 10.7, Bonus Problem, 10.12abc, ...
Book Title: Probability and Stochastic Processes
Book Edition: 5
Book Author(s): Author 1, Author 2, ...

10.1: d
10.2: c,e,f
10.7:
Bonus Problem:
10.12: a,b,c

*/

// Let's make a simple version, then we'll make it more robust as we go along.
