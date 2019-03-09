#include<fstream>
#include<vector>
#include <string.h>
#include <iostream>
#include <string>
using namespace std;


class CSVOpener
{
	vector<string> nameOfFiles;
	ifstream file;
	string fileName;
public:
	CSVOpener(void);
	CSVOpener(string fileName);
	void openCSVFile(string fileName);
	void displayFileNames();
	~CSVOpener(void);
};

