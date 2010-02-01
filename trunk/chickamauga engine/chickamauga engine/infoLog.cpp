#include "infoLog.h"
#include <string>
using namespace std;

infoLog::infoLog(int s)
{
	setupSize(s);
}
void infoLog::setupSize(int s)
{
	size = s;
	info = new string[size];
	for(int i = 0; i < size; i++)
		info[i] = "";
}
void infoLog::addString(string newLine)
{
	for(int i = 1; i < size; i++)
		info[i-1] = info[i];
	info[size-1] = newLine;
}
string infoLog::operator[](int index)
{
	return info[index];
}
string infoLog::at(int index)
{
	return info[index];
}
