#include "CSVReader.h"

CSVReader::CSVReader(void)
{
	file.clear();
}

CSVReader::CSVReader(string fileName)
{
	fileName_ = fileName;
	file.open(fileName);
}

void CSVReader::readLines(char delim) {
	if (!file.is_open())
		return;

	string line;
	while (file.good())
	{
		getline(file, line, delim); 
		lines.push_back(line);
	}
}
vector<string> CSVReader::readLines(string fileName, char delim)
{
	vector<string> l;
	ifstream f;
	string line;
	f.open(fileName);
	while (f.good())
	{
		 getline(f, line, delim);
		 l.push_back(line);
	}
	f.close();
	return l;
}

vector<string>& CSVReader::getLines()
{
	return lines;
}


CSVReader::~CSVReader(void)
{
	file.close();
}
