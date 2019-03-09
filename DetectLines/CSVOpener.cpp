#include "SVOpener.h"


CSVOpener::CSVOpener(void)
{
	file.clear();
}

CSVOpener::CSVOpener(string fileName)
{
	file.open(fileName);
	while (file.good())
	{
		 getline( file, fileName, ',' ); 
		 nameOfFiles.push_back(fileName);
	}
}

void CSVOpener::openCSVFile(string fileName)
{
	file.open(fileName);
	while (file.good())
	{
		 getline( file, fileName, ',' ); 
		 nameOfFiles.push_back(fileName);
	}
}

void CSVOpener::displayFileNames()
{
	for (int i = 0; i < nameOfFiles.size(); i++)
		cout << nameOfFiles.at(i) << endl;
}


CSVOpener::~CSVOpener(void)
{
}
