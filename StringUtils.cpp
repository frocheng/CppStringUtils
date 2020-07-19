#include "StringUtils.h"

#include <algorithm>
#include <sstream>
#include <ctime>
#include <cstdarg>

#define STR_FORMAT_YYYYMMDD_HHMMSSMMM_GMT		("%04u-%02u-%02u%s%02u:%02u:%02u.%03lu%s")
#define STR_FORMAT_YYYYMMDD_HHMMSSMMM_LOCAL		("%04u-%02u-%02u%s%02u:%02u:%02u.%03lu%s")

#ifdef WIN32

#else
#include <sys/time.h>
#endif // WIN32

extern "C" int STRAPI stringSubCount(std::string const & src, std::string const & subStr)
{
	if (src.empty() || subStr.empty())
	{
		return 0;
	}
	int cnt = 0;
	std::string::size_type pos = src.find(subStr), subSize = subStr.size();
	while (pos != std::string::npos)
	{
		cnt++;
		pos = src.find(subStr, pos + subSize);
	}
	return cnt;
}

extern "C" bool STRAPI stringContains(std::string const & src, std::string const & sub)
{
	if (sub.empty())
	{
		return false;
	}
	return src.find(sub) != std::string::npos;
}

extern "C" bool STRAPI stringHasPrefix(std::string const & src, std::string const & prefix)
{
	if (src.size() < prefix.size() || src.empty())
	{
		return false;
	}
	for (std::string::size_type i = 0; i < prefix.size(); i++)
	{
		if (src.at(i) != prefix.at(i))
		{
			return false;
		}
	}
	return true;
}

extern "C" bool STRAPI stringHasSurfix(std::string const & src, std::string const & surfix)
{
	if (src.size() < surfix.size())
	{
		return false;
	}
	for (std::string::size_type i = 1; i < surfix.size(); i++)
	{
		if (src.at(src.size() - i) != surfix.at(surfix.size() - i))
		{
			return false;
		}
	}
	return true;
}

extern "C" std::string &STRAPI stringReplaceAll(std::string & src, std::string const & orgStr, std::string const & repStr)
{
	std::string::size_type pos = src.find(orgStr), orgSize = orgStr.size(), repSize = repStr.size();
	while (pos != std::string::npos)
	{
		src = src.replace(pos, orgSize, repStr);
		pos = src.find(orgStr, pos + repSize);
	}
	return src;
}

extern "C" bool STRAPI stringToInt64(int64_t & res, std::string const & src)
{
	res = 0;
	if (src.empty())
	{
		return true;
	}

	// signed ?
	int signedFlag = 1;
	if (src.at(0) == '-')
	{
		// "-0...." return false;
		if (src.size() >= 2 && src.at(1) == '0')
		{
			return false;
		}
		signedFlag = -1;
	}
	else if (src.at(0) == '0' && src.size() >= 2)
	{
		// "0..." return false;
		return false;
	}
	

	char c = 0;
	for (std::size_t i = (signedFlag == 1 ? 0 : 1); i < src.size(); i++)
	{
		c = src.at(i) - '0';
		if (c < 0 || c > 9)
		{
			res = 0;
			return false;
		}
		res *= 10;
		res += c;
	}

	res *= signedFlag;
	return true;
}

extern "C" bool STRAPI stringToDouble(double & res, std::string const& src)
{
	res = 0;
	if (src.empty())
	{
		return true;
	}

	// signed ?
	int signedFlag = 1;
	if (src.at(0) == '-')
	{
		signedFlag = -1;
	}

	int	intPart = 0;
	double floadPart = 0;
	bool dotFound = false;
	char c = 0;
	for (std::size_t i = (signedFlag == 1 ? 0 : 1); i < src.size(); i++)
	{
		c = src.at(i);
		if (c == '.')
		{
			dotFound = true;
			break;
		}
		c -= '0';
		if (c < 0 || c > 9)
		{
			res = 0.;
			return false;
		}
		intPart *= 10;
		intPart += c;
	}
	if (dotFound)
	{
		for (std::size_t i = src.size() - 1; i >= 0; i--)
		{
			c = src.at(i);
			if (c == '.')
			{
				break;
			}
			c -= '0';
			if (c < 0 || c > 9)
			{
				res = 0.;
				return false;
			}
			floadPart *= 0.1;
			floadPart += c;
		}
	}
	res = floadPart * 0.1 + (double)intPart;
	res *= signedFlag;
	return true;
}

extern "C" bool STRAPI stringToBool(bool & res, std::string const& src)
{
	if (src == "true" || src == "True" || src == "TRUE" || src == "1")
	{
		res = true;
		return true;
	}
	if (src == "false" || src == "False" || src == "FALSE" || src == "0")
	{
		res = false;
		return true;
	}
	return false;
}

extern "C" bool STRAPI stringSplit(std::vector<std::string> & res, std::string const& src, char delim, bool skipEmpty)
{
	res.clear();
	if (src.empty())
	{
		return false;
	}

	std::istringstream iss(src);
	std::string temp = "";

	while (getline(iss, temp, delim))
	{
		stringTrim(temp);
		if (temp.empty() && skipEmpty)
		{
			continue;
		}
		res.emplace_back(temp);
	}
	
	return true;
}

extern "C" bool STRAPI stringKeyValueToMap(std::map<std::string, std::string> & res, std::string const& src, char pairDelim, char kvDelim)
{
	if (src.empty())
	{
		res.clear();
		return false;
	}
	std::vector<std::string> vec;
	bool bret = stringSplit(vec, src, pairDelim);
	if (!bret)
	{
		return false;
	}
	std::vector<std::string> kvVec;
	for (auto s : vec)
	{
		bret = stringSplit(kvVec, s, kvDelim);
		if (!bret || kvVec.size() != 2)
		{
			res.clear();
			return false;
		}
		if (res.find(kvVec[0]) != res.end())
		{
			res.clear();
			return false;
		}
		res[kvVec[0]] = kvVec[1];
	}
	return true;
}

extern "C" bool STRAPI stringMapToKeyValue(std::string & res, std::map<std::string, std::string> const & m, char pairDelim, char kvDelim)
{
	res.clear();
	if (m.empty())
	{
		return false;
	}
	std::string kv = "";
	for (auto pair = m.begin(); pair != m.end(); ++pair)
	{
		kv = kv.append(pair->first);
		kv.push_back(kvDelim);
		kv = kv.append(pair->second);
		res = res.append(kv);
		res.push_back(pairDelim);
		kv.clear();
	}

	if (res.back() == pairDelim)
	{
		res.pop_back();
	}

	return true;
}

extern "C" std::string & STRAPI stringTrimLeft(std::string & s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(0, s.find_first_not_of(" \r\n\t"));
	return s;
}

extern "C" std::string & STRAPI stringTrimRight(std::string & s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(s.find_last_not_of(" \r\n\t") + 1);
	return s;
}

extern "C" std::string & STRAPI stringTrim(std::string & s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(0, s.find_first_not_of(" \r\n\t"));
	s.erase(s.find_last_not_of(" \r\n\t") + 1);
	return s;
}

extern "C" std::string & STRAPI stringUppercase(std::string & s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

extern "C" std::string & STRAPI stringLowercase(std::string & s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

std::string STRAPI getTimeStampGMT(std::string const & dayHourDelim, std::string const& tzPostfix)
{
	char bf[64] = {0};
#ifdef WIN32
	SYSTEMTIME t;
	GetSystemTime(&t);
	sprintf_s(bf, 63, STR_FORMAT_YYYYMMDD_HHMMSSMMM_GMT, t.wYear, t.wMonth, t.wDay, dayHourDelim.c_str(),t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, tzPostfix.c_str());
#else
	time_t t = time(0);
	struct tm gt;
	::gmtime_r(&t, &gt);
	struct timeval tv;
	gettimeofday(&tv, NULL);
	snprintf(bf, 63, STR_FORMAT_YYYYMMDD_HHMMSSMMM_GMT, gt.tm_year + 1900, gt.tm_mon, gt.tm_mday, dayHourDelim.c_str(), gt.tm_hour, gt.tm_min, gt.tm_sec, tv.tv_usec/1000, tzPostfix.c_str());
#endif // WIN32
	return bf;
}

std::string STRAPI getTimeStampLocal(std::string const & dayHourDelim, std::string const & tzPostfix)
{
	char bf[64] = { 0 };
#ifdef WIN32
	SYSTEMTIME t;
	GetLocalTime(&t);
	sprintf_s(bf, 63, STR_FORMAT_YYYYMMDD_HHMMSSMMM_LOCAL, t.wYear, t.wMonth, t.wDay, dayHourDelim.c_str(), t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, tzPostfix.c_str());
#else
	time_t t = time(0);
	struct tm gt;
	::localtime_r(&t, &gt);
	struct timeval tv;
	gettimeofday(&tv, NULL);
	snprintf(bf, 63, STR_FORMAT_YYYYMMDD_HHMMSSMMM_LOCAL, gt.tm_year + 1900, gt.tm_mon, gt.tm_mday, dayHourDelim.c_str(), gt.tm_hour, gt.tm_min, gt.tm_sec, tv.tv_usec / 1000, tzPostfix.c_str());
#endif // WIN32
	return bf;
}

std::string STRAPI getASCTimeStampGMT()
{
	time_t t = time(0);
	struct tm info;
	char bf[64] = { 0 };
#ifdef WIN32
	errno_t e = ::gmtime_s(&info, &t);
	if (e != 0)
	{
		return "";
	}
	e = ::asctime_s(bf, 63, &info);
	if (e != 0)
	{
		return "";
	}
#else
	//struct tm *gmtime_r(const time_t *timep, struct tm *result);
	struct tm *ptm = gmtime_r(&t, &info);
	if (ptm == NULL)
	{
		return "";
	}
	// char *asctime_r(const struct tm *tm, char *buf);
	char* pc = asctime_r(&info, bf);
	if (pc == NULL)
	{
		return "";
	}
#endif
	std::string s = bf;
	if (s.back() == '\n')
	{
		s.pop_back();
	}
	return s;
}

std::string STRAPI getASCTimeStampLocal()
{
	time_t t = time(0);
	struct tm info;
	char bf[64] = {0};
#ifdef WIN32
	errno_t e = ::localtime_s(&info, &t);
	if (e != 0)
	{
		return "";
	}
	e = ::asctime_s(bf, 63, &info);
	if (e != 0)
	{
		return "";
	}
#else
	// struct tm *localtime_r(const time_t *timep, struct tm *result);
	struct tm *ptm = ::localtime_r(&t, &info);
	if (ptm == NULL)
	{
		return "";
	}
	// char *asctime_r(const struct tm *tm, char *buf);
	char *pc = ::asctime_r(&info, bf);
	if (pc == NULL)
	{
		return "";
	}
#endif
	std::string s = bf;
	if (s.back() == '\n')
	{
		s.pop_back();
	}
	return s;
}
#include <iostream>
std::string STRAPI stringFormat(int predictedSize, char const * format, ...)
{
	if (format == nullptr || *format == '\0'
		|| predictedSize <= 1)
	{
		return "";
	}
	char* bf = new (std::nothrow) char[predictedSize + 1];
	bf[predictedSize] = 0;
	va_list lst;
	va_start(lst, format);
#ifdef WIN32
	int count = vsprintf_s(bf, predictedSize, format, lst);
	//std::cout << "count: " << count << std::endl;
	//bf = new (std::nothrow) char[count + 1];
	//bf[count] = 0;
	//if (bf == nullptr)
	//{
	//	return "";
	//}
	//vsprintf_s(bf, count, format, lst);
#else
	int count = snprintf(NULL, 0, format, lst);
	bf = new (std::nothrow) char[count + 1];
	bf[count] = 0;
	if (bf == nullptr)
	{
		return "";
	}
	vsnprintf(bf, count, format, lst);
#endif // WIN32

	va_end(lst);
	std::string s = bf;
	delete[] bf;
	return s;
}
