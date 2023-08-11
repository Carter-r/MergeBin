#include "FileInfo.h"

FileInfo::FileInfo(const char* szFile, long long lFileSize)
	: nFileName(szFile),
	nFileSize(lFileSize)
{

}

FileInfo::~FileInfo()
{

}

std::string FileInfo::getFileName()
{
	return nFileName;
}
