#pragma once
#include <string>

class FileInfo
{
public:
	FileInfo(const char* szFile, long long lFileSize);
	~FileInfo();

private:
	long long nFileSize;
	std::string nFileName;
};

