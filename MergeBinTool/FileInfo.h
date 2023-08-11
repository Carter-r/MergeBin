#pragma once
#include <string>

class FileInfo
{
public:
	FileInfo(const char* szFile, long long lFileSize);
	~FileInfo();

	std::string getFileName();

private:
	long long nFileSize;
	std::string nFileName;
};

