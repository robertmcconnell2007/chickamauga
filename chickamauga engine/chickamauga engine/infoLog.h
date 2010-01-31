#pragma once
#include <string>
using namespace std;

class infoLog
{
private:
	int size;
	string* info;
public:
	infoLog();
	infoLog(int s);
	void addString(string newLine);
	string operator[](int index);
	void setupSize(int s);
};