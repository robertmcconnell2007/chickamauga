#pragma once
#include <string>
using namespace std;

class infoLog
{
private:
	string* info;
public:
	int size;
	infoLog();
	infoLog(int s);
	void addString(string newLine);
	string operator[](int index);
	string at(int index);
	void setupSize(int s);
};