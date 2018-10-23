#ifndef _lf_file_utils_h_ 
#define _lf_file_utils_h_ 
using namespace std;

typedef vector<string>      TLFStrings;

extern "C"
{
#ifdef WIN32
	#include <Rpc.h>
	#define c_separator  "\\"
#else
	#include <uuid/uuid.h>
	typedef uuid_t UUID;
	#define c_separator  "/"
#endif
}

#ifdef WIN32
	const UUID c_uuidZero = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
#else
	const UUID c_uuidZero = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif

#ifdef WIN32
#define LF_UUID_CREATE(v)  UuidCreate(&v);
#define LF_NULL_UUID_CREATE(v) memset(&v, 0, sizeof(UUID));
#else
#define LF_UUID_CREATE(v)  uuid_generate(v);
#define LF_NULL_UUID_CREATE(v) memset(v, 0, sizeof(UUID));
#endif
//functions to work with file names
#if __BCPLUSPLUS__ != 0
	string LFGetFilePath(const string& strPath)
	{
		const string c = c_separator;
		int len = strPath.find_last_of(c);
		return strPath.substr(0, len);
	}

	string LFGetFileExt(const std::string&  strFileName)
	{
		int len = strFileName.find_last_of('.');
		if (len > 0)
			return strFileName.substr(len, strFileName.length() - len);
		else
			return "";
	}
	string LFGetFileName(const std::string&  strFileName)
	{
		int len = strFileName.find_last_of('\\');
		string str = strFileName.substr(len, strFileName.length() - len);
		len = str.find_last_of('.');
		return str.substr(0, len);

	}
	string LFChangeFileExt(std::string& strFileName, std::string strExt)
	{
		int len = strFileName.find_last_of('.');
		return strFileName.substr(0, len) + strExt;
	}
	string LFMakeFileName(string& strPath, string strName, string strExt)
	{
		if (strName.length() == 0)
			return "";
		if (strPath.find_last_of('\\') != strPath.length() - 1)
			strPath += "\\";
		if (strExt.find_first_of('.') != 0)
		{
			string tmp = ".";
			tmp += strExt;
			strExt = tmp;
		}
		return strPath + strName + strExt;
	}
#else
	string LFGetFilePath(const string& strPath);
	string LFGetFileExt(const string&  strFileName);
	string LFGetFileName(const string&  strFileName);
	string LFChangeFileExt(string& strFileName, string strExt);
	string LFMakeFileName(string& strPath, string strName, string strExt);
#endif
//functions to work with file system
bool LFCreateDir(const char* lpPath);
bool LFDirExist(const char* lpPath);
bool LFRemoveDir(const char* lpPath);
bool LFFileExists(const string& strFileName);

bool LFGetDirFiles(const char* lpDir, TLFStrings& names);
// functions to convert data
string LFIntToStr(int value);
string LFGUIDToString(UUID* id);
// functions to work with time
unsigned long LFGetTickCount();
#endif //_lf_file_utils_h_ 
