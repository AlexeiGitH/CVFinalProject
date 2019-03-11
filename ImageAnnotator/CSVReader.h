#include<fstream>
#include<vector>
#include <iostream>
#include <string>  

using namespace std;

class CSVReader
{
	vector<string> lines;
	ifstream file;
	string fileName_;
public:
	CSVReader(void);
	CSVReader(string fileName);
	void readLines();
	vector<string> readLines(string fileName);
	vector<string>& getLines();
	~CSVReader(void);
};

