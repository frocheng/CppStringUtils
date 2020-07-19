/*******************************************************
 * @FileName: StringUtils.h
 * @Author: Frodo Cheng
 * @CreatedTime: May 30 2019
 * @Description:
 *		Some std::string opeartion methods.
********************************************************/
#pragma once

#if defined _WIN32 || defined _WIN64
#  ifndef WIN32
#    define WIN32
#  endif
#endif

#ifdef WIN32
#	include <Windows.h>
#	define	STRAPI	WINAPI
#else
#	define	STRAPI
#endif // WIN32

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <thread>

/**
 * @Descrition:
 *		template method, object to string.
 * @Param: t of type t, must overload operator<< const method. 
 * @Return: std::string type value.
**/
template<typename T>
std::string objectToString(T const& t)
{
	std::ostringstream stream;
	stream << t;
	return stream.str();
}

extern "C" int STRAPI stringSubCount(std::string const& src, std::string const& sub);

extern "C" bool STRAPI stringContains(std::string const& src, std::string const& sub);

extern "C" bool STRAPI stringHasPrefix(std::string const& src, std::string const& prefix);

extern "C" bool STRAPI stringHasSurfix(std::string const & src, std::string const & surfix);

extern "C" std::string& STRAPI stringReplaceAll(std::string & src, std::string const& orgStr, std::string const& repStr);

extern "C" bool STRAPI stringToInt64(int64_t & res, std::string const& src);

extern "C" bool STRAPI stringToDouble(double & res, std::string const& src);

extern "C" bool STRAPI stringToBool(bool & res, std::string const& src);

extern "C" bool STRAPI stringSplit(std::vector<std::string> & outVec, std::string const& src, char delim, bool skipWhiteSpace = true);

extern "C" bool STRAPI stringKeyValueToMap(std::map<std::string, std::string> & outMap, std::string const& src, char pairDelim = ';', char kvDelim = '=');

extern "C" bool STRAPI stringMapToKeyValue(std::string & res, std::map<std::string, std::string> const & m, char pairDelim = ';', char kvDelim = '=');

extern "C" std::string & STRAPI stringTrim(std::string & s);
extern "C" std::string & STRAPI stringTrimLeft(std::string & s);
extern "C" std::string & STRAPI stringTrimRight(std::string & s);

extern "C" std::string & STRAPI stringUppercase(std::string & s);
extern "C" std::string & STRAPI stringLowercase(std::string & s);

/**
 * @Description:
 *     Get GMT Time Stamp as Foramt: 2019-05-30 12:31:02.534+0000
 * @param
 *     dayHourDelim: default value " ", You can pass one your self.
 *     tzPostfix: default value "+0000", Time Zone
**/
std::string STRAPI getTimeStampGMT(std::string const & dayHourDelim = " ", std::string const& tzPostfix = "+0000");

/**
 * @Description:
 *     Get Local Time Stamp as Foramt: 2019-05-30 20:33:47.939+0800
 * @param
 *     dayHourDelim: default value " ", You can pass one your self.
 *     tzPostfix: default value "+0800", Time Zone
**/
std::string STRAPI getTimeStampLocal(std::string const & dayHourDelim = " ", std::string const& tzPostfix = "+0800");

/**
 * @Description:
 *     Get GMT Time Stamp as Foramt: Thu May 30 12:33:47 2019
**/
std::string STRAPI getASCTimeStampGMT();

/**
 * @Description:
 *     Get Local Time Stamp as Foramt: Thu May 30 20:33:47 2019
**/
std::string STRAPI getASCTimeStampLocal();

/**
 * @Description:
 *     Format string.
 * @param
 *    predictedSize: size of the formatted string you predicted,
 *        if too little, the string will be truncated.
 *        if too large, the memory cannot be alloced, return empty string.
**/
std::string STRAPI stringFormat(int predictedSize, char const* format, ...);


template<typename T>
void test(std::ostream & os, T & t)
{
	os << t;
}

template<typename T>
void test(std::ostream & os, T && t)
{
	os << t;
}


template <typename T=int>
void StringUtilsTest(T)
{
	std::cout << objectToString(std::this_thread::get_id()) << std::endl;
	std::cout << getTimeStampGMT() << std::endl;
	std::cout << getTimeStampLocal() << std::endl;
	std::cout << getASCTimeStampGMT() << std::endl;
	std::cout << getASCTimeStampLocal() << std::endl;

	std::string s = "HelloWorldHelloWorldHelloWorld";

	std::cout << std::boolalpha << stringHasPrefix(s, "Hello") << std::endl;
	std::cout << std::boolalpha << stringHasSurfix(s, "World") << std::endl;

	std::cout << stringReplaceAll(s, "Hello", "World") << std::endl;

	std::cout << "Find " << s.find("") << std::endl;
	std::cout << std::boolalpha << stringContains(s, "") << std::endl;
	std::cout << std::boolalpha << stringContains("", s) << std::endl;

	test(std::cout, "SayHello!\n");

	std::fstream f;
	f.open("out.log", std::ios::app | std::ios::binary);
	test(f, "SayHello!\n");
	f.close();

	std::cout << stringFormat(32, "%s%d", "StringFormat\n",20);
	char bf[2] = { 0 };
	//std::cout << "count " << vsnprintf(bf, 1, "%s", "HelloWorld");
	std::string src = "Hello_Hello_Hello";
	std::string subStr = "Hello";
	std::string::size_type pos = src.find(subStr), subSize = subStr.size();
	int cnt = 0;
	while (pos != std::string::npos)
	{
		cnt++;
		pos = src.find(subStr, pos + subSize);
	}
	std::cout << "count " << cnt << std::endl;
	std::cout << src << std::endl;
	std::cout << "StringSubCount: " << stringSubCount(src, subStr) << std::endl;
}